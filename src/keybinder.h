#pragma once

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QHash>
#include <QSet>
#include <QTimer>
#include <windows.h>

class KeyBinder : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit KeyBinder(QObject* parent = nullptr);
    ~KeyBinder() override;

    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

    Q_INVOKABLE void startListeningForTarget(int profileId);
    Q_INVOKABLE void startListeningForKeybind(int profileId);
    Q_INVOKABLE void stopListening();

    bool isListening() const { return m_isListening; }
    int listeningProfileId() const { return m_listeningProfileId; }
    bool isListeningForTarget() const { return m_isListeningForTarget; }

signals:
    void targetButtonDetected(int profileId, int keyCode, const QString& keyName);
    void keybindDetected(int profileId, int keyCode, const QString& keyName);
    void hotkeyPressed(int profileId);
    void hotkeyReleased(int profileId);

public slots:
    void registerHotkey(int profileId, int keyCode);
    void unregisterHotkey(int profileId);
    void unregisterAllHotkeys();

private:
    static LRESULT CALLBACK keyboardHookProc(int code, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK mouseHookProc(int code, WPARAM wParam, LPARAM lParam);

    void handleKeyEvent(int keyCode, bool pressed, bool injected);
    void emitDetection(int keyCode);
    void emitHotkeyState(int keyCode, bool pressed);
    bool shouldIgnoreInjectedEvent(int keyCode) const;
    void pollHotkeys();
    bool canUseWindowsHotkeyApi(int keyCode) const;
    void registerWindowsHotkey(int profileId, int keyCode);
    void unregisterWindowsHotkey(int profileId);

    bool m_isListening = false;
    int m_listeningProfileId = -1;
    bool m_isListeningForTarget = true;
    QHash<int, int> m_hotkeyMap;
    QHash<int, int> m_hotkeyIdMap;
    QHash<int, int> m_windowsHotkeyByProfile;
    QSet<int> m_pressedHotkeys;
    QTimer* m_hotkeyPollTimer = nullptr;
    HHOOK m_keyboardHook = nullptr;
    HHOOK m_mouseHook = nullptr;

    static KeyBinder* s_instance;
};
