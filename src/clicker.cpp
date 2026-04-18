#include "clicker.h"
#include <windows.h>

QHash<int, QString> Clicker::s_keyNames;
QHash<QString, int> Clicker::s_nameToKey;
bool Clicker::s_initialized = false;

Clicker::Clicker(QObject* parent)
    : QObject(parent)
{
    initializeKeyMap();
}

void Clicker::initializeKeyMap()
{
    if (s_initialized) return;
    s_initialized = true;

    // Mouse buttons
    s_keyNames[0x01] = "Left Mouse";
    s_keyNames[0x02] = "Right Mouse";
    s_keyNames[0x04] = "Middle Mouse";
    s_keyNames[0x05] = "X1 Mouse";
    s_keyNames[0x06] = "X2 Mouse";

    // Number keys (top row)
    s_keyNames[0x30] = "0";
    s_keyNames[0x31] = "1";
    s_keyNames[0x32] = "2";
    s_keyNames[0x33] = "3";
    s_keyNames[0x34] = "4";
    s_keyNames[0x35] = "5";
    s_keyNames[0x36] = "6";
    s_keyNames[0x37] = "7";
    s_keyNames[0x38] = "8";
    s_keyNames[0x39] = "9";

    // Letter keys (A-Z)
    s_keyNames[0x41] = "A";
    s_keyNames[0x42] = "B";
    s_keyNames[0x43] = "C";
    s_keyNames[0x44] = "D";
    s_keyNames[0x45] = "E";
    s_keyNames[0x46] = "F";
    s_keyNames[0x47] = "G";
    s_keyNames[0x48] = "H";
    s_keyNames[0x49] = "I";
    s_keyNames[0x4A] = "J";
    s_keyNames[0x4B] = "K";
    s_keyNames[0x4C] = "L";
    s_keyNames[0x4D] = "M";
    s_keyNames[0x4E] = "N";
    s_keyNames[0x4F] = "O";
    s_keyNames[0x50] = "P";
    s_keyNames[0x51] = "Q";
    s_keyNames[0x52] = "R";
    s_keyNames[0x53] = "S";
    s_keyNames[0x54] = "T";
    s_keyNames[0x55] = "U";
    s_keyNames[0x56] = "V";
    s_keyNames[0x57] = "W";
    s_keyNames[0x58] = "X";
    s_keyNames[0x59] = "Y";
    s_keyNames[0x5A] = "Z";

    // Function keys
    s_keyNames[0x70] = "F1";
    s_keyNames[0x71] = "F2";
    s_keyNames[0x72] = "F3";
    s_keyNames[0x73] = "F4";
    s_keyNames[0x74] = "F5";
    s_keyNames[0x75] = "F6";
    s_keyNames[0x76] = "F7";
    s_keyNames[0x77] = "F8";
    s_keyNames[0x78] = "F9";
    s_keyNames[0x79] = "F10";
    s_keyNames[0x7A] = "F11";
    s_keyNames[0x7B] = "F12";
    s_keyNames[0x7C] = "F13";
    s_keyNames[0x7D] = "F14";
    s_keyNames[0x7E] = "F15";
    s_keyNames[0x7F] = "F16";
    s_keyNames[0x80] = "F17";
    s_keyNames[0x81] = "F18";
    s_keyNames[0x82] = "F19";
    s_keyNames[0x83] = "F20";
    s_keyNames[0x84] = "F21";
    s_keyNames[0x85] = "F22";
    s_keyNames[0x86] = "F23";
    s_keyNames[0x87] = "F24";

    // Navigation keys
    s_keyNames[0x21] = "Page Up";
    s_keyNames[0x22] = "Page Down";
    s_keyNames[0x23] = "End";
    s_keyNames[0x24] = "Home";
    s_keyNames[0x25] = "Left";
    s_keyNames[0x26] = "Up";
    s_keyNames[0x27] = "Right";
    s_keyNames[0x28] = "Down";
    s_keyNames[0x2D] = "Insert";
    s_keyNames[0x2E] = "Delete";

    // Special keys
    s_keyNames[0x08] = "Backspace";
    s_keyNames[0x09] = "Tab";
    s_keyNames[0x0C] = "Clear";
    s_keyNames[0x0D] = "Enter";
    s_keyNames[0x10] = "Shift";
    s_keyNames[0x11] = "Ctrl";
    s_keyNames[0x12] = "Alt";
    s_keyNames[0x13] = "Pause";
    s_keyNames[0x14] = "Caps Lock";
    s_keyNames[0x1B] = "Escape";
    s_keyNames[0x20] = "Space";

    // Symbols and punctuation
    s_keyNames[0xBA] = ";";
    s_keyNames[0xBB] = "=";
    s_keyNames[0xBC] = ",";
    s_keyNames[0xBD] = "-";
    s_keyNames[0xBE] = ".";
    s_keyNames[0xBF] = "/";
    s_keyNames[0xC0] = "`";
    s_keyNames[0xDB] = "[";
    s_keyNames[0xDC] = "\\";
    s_keyNames[0xDD] = "]";
    s_keyNames[0xDE] = "'";

    // Numpad keys
    s_keyNames[0x60] = "Num 0";
    s_keyNames[0x61] = "Num 1";
    s_keyNames[0x62] = "Num 2";
    s_keyNames[0x63] = "Num 3";
    s_keyNames[0x64] = "Num 4";
    s_keyNames[0x65] = "Num 5";
    s_keyNames[0x66] = "Num 6";
    s_keyNames[0x67] = "Num 7";
    s_keyNames[0x68] = "Num 8";
    s_keyNames[0x69] = "Num 9";
    s_keyNames[0x6A] = "Num *";
    s_keyNames[0x6B] = "Num +";
    s_keyNames[0x6C] = "Num Enter";
    s_keyNames[0x6D] = "Num -";
    s_keyNames[0x6E] = "Num .";
    s_keyNames[0x6F] = "Num /";

    // Lock keys
    s_keyNames[0x90] = "Num Lock";
    s_keyNames[0x91] = "Scroll Lock";

    // OEM specific
    s_keyNames[0x92] = "OEM 1";
    s_keyNames[0x93] = "OEM 2";
    s_keyNames[0x94] = "OEM 3";
    s_keyNames[0x95] = "OEM 4";
    s_keyNames[0x96] = "OEM 5";

    // Multimedia keys
    s_keyNames[0xA0] = "Left Shift";
    s_keyNames[0xA1] = "Right Shift";
    s_keyNames[0xA2] = "Left Ctrl";
    s_keyNames[0xA3] = "Right Ctrl";
    s_keyNames[0xA4] = "Left Alt";
    s_keyNames[0xA5] = "Right Alt";
    s_keyNames[0xA6] = "Browser Back";
    s_keyNames[0xA7] = "Browser Forward";
    s_keyNames[0xA8] = "Browser Refresh";
    s_keyNames[0xA9] = "Browser Stop";
    s_keyNames[0xAA] = "Browser Search";
    s_keyNames[0xAB] = "Browser Favorites";
    s_keyNames[0xAC] = "Browser Home";
    s_keyNames[0xAD] = "Volume Mute";
    s_keyNames[0xAE] = "Volume Down";
    s_keyNames[0xAF] = "Volume Up";
    s_keyNames[0xB0] = "Media Next";
    s_keyNames[0xB1] = "Media Prev";
    s_keyNames[0xB2] = "Media Stop";
    s_keyNames[0xB3] = "Media Play/Pause";
    s_keyNames[0xB4] = "Launch Mail";
    s_keyNames[0xB5] = "Launch Media";
    s_keyNames[0xB6] = "Launch App1";
    s_keyNames[0xB7] = "Launch App2";

    // Additional OEM keys
    s_keyNames[0xE2] = "OEM \\";

    // Build reverse lookup map
    for (auto it = s_keyNames.begin(); it != s_keyNames.end(); ++it) {
        s_nameToKey[it.value().toLower()] = it.key();
    }
}

bool Clicker::isMouseButton(int keyCode)
{
    return keyCode == 0x01 || keyCode == 0x02 ||
           keyCode == 0x04 || keyCode == 0x05 ||
           keyCode == 0x06;
}

Clicker::MouseButton Clicker::keyCodeToMouseButton(int keyCode)
{
    switch (keyCode) {
    case 0x01: return LeftButton;
    case 0x02: return RightButton;
    case 0x04: return MiddleButton;
    case 0x05: return XButton1;
    case 0x06: return XButton2;
    default: return LeftButton;
    }
}

QString Clicker::keyCodeToString(int keyCode)
{
    if (s_keyNames.contains(keyCode)) {
        return s_keyNames[keyCode];
    }
    return QString("Key 0x%1").arg(keyCode, 2, 16, QChar('0'));
}

int Clicker::stringToKeyCode(const QString& keyName)
{
    QString lower = keyName.toLower().trimmed();
    
    if (s_nameToKey.contains(lower)) {
        return s_nameToKey[lower];
    }
    
    // Try single character
    if (lower.length() == 1) {
        QChar c = lower[0].toUpper();
        if (c.isLetterOrNumber()) {
            return c.unicode();
        }
    }
    
    return 0;
}

void Clicker::performClick(int keyCode)
{
    if (isMouseButton(keyCode)) {
        performMouseClick(keyCodeToMouseButton(keyCode));
    } else {
        performKeyPress(keyCode);
        performKeyRelease(keyCode);
    }
}

void Clicker::performKeyPress(int keyCode)
{
    if (!isMouseButton(keyCode)) {
        sendKeyboardInput(keyCode, true);
    }
}

void Clicker::performKeyRelease(int keyCode)
{
    if (!isMouseButton(keyCode)) {
        sendKeyboardInput(keyCode, false);
    }
}

void Clicker::performMouseClick(MouseButton button)
{
    performMouseDown(button);
    performMouseUp(button);
}

void Clicker::performMouseDown(MouseButton button)
{
    INPUT input = {};
    input.type = INPUT_MOUSE;

    switch (button) {
    case LeftButton:
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        break;
    case RightButton:
        input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        break;
    case MiddleButton:
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        break;
    case XButton1:
        input.mi.dwFlags = MOUSEEVENTF_XDOWN;
        input.mi.mouseData = XBUTTON1;
        break;
    case XButton2:
        input.mi.dwFlags = MOUSEEVENTF_XDOWN;
        input.mi.mouseData = XBUTTON2;
        break;
    }

    SendInput(1, &input, sizeof(INPUT));
}

void Clicker::performMouseUp(MouseButton button)
{
    INPUT input = {};
    input.type = INPUT_MOUSE;

    switch (button) {
    case LeftButton:
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        break;
    case RightButton:
        input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        break;
    case MiddleButton:
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        break;
    case XButton1:
        input.mi.dwFlags = MOUSEEVENTF_XUP;
        input.mi.mouseData = XBUTTON1;
        break;
    case XButton2:
        input.mi.dwFlags = MOUSEEVENTF_XUP;
        input.mi.mouseData = XBUTTON2;
        break;
    }

    SendInput(1, &input, sizeof(INPUT));
}

void Clicker::sendKeyboardInput(WORD vk, bool keyDown)
{
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.wScan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    input.ki.dwFlags = keyDown ? 0 : KEYEVENTF_KEYUP;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    SendInput(1, &input, sizeof(INPUT));
}