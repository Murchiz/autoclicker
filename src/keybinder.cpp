#include "keybinder.h"
#include "clicker.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMetaObject>

KeyBinder* KeyBinder::s_instance = nullptr;

namespace {
constexpr DWORD kInjectedMouseFlags = LLMHF_INJECTED | LLMHF_LOWER_IL_INJECTED;
constexpr DWORD kInjectedKeyboardFlags = LLKHF_INJECTED | LLKHF_LOWER_IL_INJECTED;

int mouseMessageToKeyCode(WPARAM message, const MSLLHOOKSTRUCT& info)
{
    switch (message) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        return VK_LBUTTON;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        return VK_RBUTTON;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        return VK_MBUTTON;
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
        return HIWORD(info.mouseData) == XBUTTON2 ? VK_XBUTTON2 : VK_XBUTTON1;
    default:
        return 0;
    }
}

bool isMousePressMessage(WPARAM message)
{
    return message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN ||
           message == WM_MBUTTONDOWN || message == WM_XBUTTONDOWN;
}

bool isMouseReleaseMessage(WPARAM message)
{
    return message == WM_LBUTTONUP || message == WM_RBUTTONUP ||
           message == WM_MBUTTONUP || message == WM_XBUTTONUP;
}
}

KeyBinder::KeyBinder(QObject* parent)
    : QObject(parent)
{
    s_instance = this;
    QCoreApplication::instance()->installNativeEventFilter(this);
    m_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, keyboardHookProc, nullptr, 0);
    m_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, mouseHookProc, nullptr, 0);

    if (!m_keyboardHook || !m_mouseHook) {
        qWarning() << "Failed to install input hooks:" << GetLastError();
    }

    m_hotkeyPollTimer = new QTimer(this);
    m_hotkeyPollTimer->setInterval(8);
    connect(m_hotkeyPollTimer, &QTimer::timeout, this, &KeyBinder::pollHotkeys);
    m_hotkeyPollTimer->start();
}

KeyBinder::~KeyBinder()
{
    unregisterAllHotkeys();

    if (m_keyboardHook) {
        UnhookWindowsHookEx(m_keyboardHook);
        m_keyboardHook = nullptr;
    }

    if (m_mouseHook) {
        UnhookWindowsHookEx(m_mouseHook);
        m_mouseHook = nullptr;
    }

    if (QCoreApplication::instance()) {
        QCoreApplication::instance()->removeNativeEventFilter(this);
    }

    if (s_instance == this) {
        s_instance = nullptr;
    }
}

bool KeyBinder::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)

    auto* msg = reinterpret_cast<MSG*>(message);
    if (!msg || msg->message != WM_HOTKEY) {
        return false;
    }

    const int profileId = static_cast<int>(msg->wParam);
    if (!m_hotkeyMap.contains(profileId)) {
        return false;
    }

    const int keyCode = m_hotkeyMap.value(profileId);
    emitHotkeyState(keyCode, true);
    return false;
}

void KeyBinder::startListeningForTarget(int profileId)
{
    m_isListening = true;
    m_listeningProfileId = profileId;
    m_isListeningForTarget = true;
}

void KeyBinder::startListeningForKeybind(int profileId)
{
    m_isListening = true;
    m_listeningProfileId = profileId;
    m_isListeningForTarget = false;
}

void KeyBinder::stopListening()
{
    m_isListening = false;
    m_listeningProfileId = -1;
}

void KeyBinder::registerHotkey(int profileId, int keyCode) {
    if (keyCode == 0) {
        unregisterHotkey(profileId);
        return;
    }
    unregisterWindowsHotkey(profileId);
    m_hotkeyMap[profileId] = keyCode;
    m_hotkeyIdMap[keyCode] = profileId;
    // Mark the key as already pressed so the still-held key
    // doesn't immediately trigger hotkeyPressed after listening ends
    m_pressedHotkeys.insert(keyCode);
    registerWindowsHotkey(profileId, keyCode);
}

void KeyBinder::unregisterHotkey(int profileId)
{
    if (!m_hotkeyMap.contains(profileId)) {
        return;
    }

    const int keyCode = m_hotkeyMap.take(profileId);
    m_hotkeyIdMap.remove(keyCode);
    m_pressedHotkeys.remove(keyCode);
    unregisterWindowsHotkey(profileId);
}

void KeyBinder::unregisterAllHotkeys()
{
    for (auto it = m_windowsHotkeyByProfile.cbegin(); it != m_windowsHotkeyByProfile.cend(); ++it) {
        UnregisterHotKey(nullptr, it.key());
    }
    m_windowsHotkeyByProfile.clear();
    m_hotkeyMap.clear();
    m_hotkeyIdMap.clear();
    m_pressedHotkeys.clear();
}

LRESULT CALLBACK KeyBinder::keyboardHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (code >= 0 && s_instance) {
        const auto* info = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        const bool pressed = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        const bool released = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
        if (pressed || released) {
            s_instance->handleKeyEvent(static_cast<int>(info->vkCode), pressed,
                                       (info->flags & kInjectedKeyboardFlags) != 0);
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

LRESULT CALLBACK KeyBinder::mouseHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (code >= 0 && s_instance) {
        const auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        const int keyCode = mouseMessageToKeyCode(wParam, *info);
        if (keyCode != 0) {
            const bool pressed = isMousePressMessage(wParam);
            const bool released = isMouseReleaseMessage(wParam);
            if (pressed || released) {
                s_instance->handleKeyEvent(keyCode, pressed,
                                           (info->flags & kInjectedMouseFlags) != 0);
            }
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void KeyBinder::handleKeyEvent(int keyCode, bool pressed, bool injected)
{
    if (injected && shouldIgnoreInjectedEvent(keyCode)) {
        return;
    }

    if (m_isListening && pressed) {
        emitDetection(keyCode);
        return;
    }

    emitHotkeyState(keyCode, pressed);
}

void KeyBinder::emitDetection(int keyCode)
{
    const int profileId = m_listeningProfileId;
    const QString keyName = Clicker::keyCodeToString(keyCode);

    if (m_isListeningForTarget) {
        emit targetButtonDetected(profileId, keyCode, keyName);
    } else {
        emit keybindDetected(profileId, keyCode, keyName);
    }
}

void KeyBinder::emitHotkeyState(int keyCode, bool pressed)
{
    if (!m_hotkeyIdMap.contains(keyCode)) {
        return;
    }

    const int profileId = m_hotkeyIdMap.value(keyCode);

    if (pressed) {
        if (m_pressedHotkeys.contains(keyCode)) {
            return;
        }
        m_pressedHotkeys.insert(keyCode);
        emit hotkeyPressed(profileId);
        return;
    }

    if (!m_pressedHotkeys.contains(keyCode)) {
        return;
    }

    m_pressedHotkeys.remove(keyCode);
    emit hotkeyReleased(profileId);
}

bool KeyBinder::shouldIgnoreInjectedEvent(int keyCode) const
{
    return m_hotkeyIdMap.contains(keyCode);
}

void KeyBinder::pollHotkeys()
{
    if (m_isListening || m_hotkeyIdMap.isEmpty()) {
        return;
    }

    for (auto it = m_hotkeyIdMap.cbegin(); it != m_hotkeyIdMap.cend(); ++it) {
        const int keyCode = it.key();
        const bool pressed = (GetAsyncKeyState(static_cast<int>(keyCode)) & 0x8000) != 0;
        emitHotkeyState(keyCode, pressed);
    }
}

bool KeyBinder::canUseWindowsHotkeyApi(int keyCode) const
{
    if (Clicker::isMouseButton(keyCode)) {
        return false;
    }

    return keyCode != VK_SHIFT && keyCode != VK_CONTROL && keyCode != VK_MENU;
}

void KeyBinder::registerWindowsHotkey(int profileId, int keyCode)
{
    if (!canUseWindowsHotkeyApi(keyCode)) {
        return;
    }

    if (RegisterHotKey(nullptr, profileId, MOD_NOREPEAT, static_cast<UINT>(keyCode)) != 0) {
        m_windowsHotkeyByProfile.insert(profileId, keyCode);
        return;
    }

    qWarning() << "RegisterHotKey failed for profile" << profileId
               << "key" << keyCode << "error:" << GetLastError();
}

void KeyBinder::unregisterWindowsHotkey(int profileId)
{
    if (!m_windowsHotkeyByProfile.contains(profileId)) {
        return;
    }

    UnregisterHotKey(nullptr, profileId);
    m_windowsHotkeyByProfile.remove(profileId);
}
