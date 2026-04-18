#pragma once

#include <QObject>
#include <QHash>
#include <windows.h>

class Clicker : public QObject
{
    Q_OBJECT

public:
    enum MouseButton {
        LeftButton = 0,
        RightButton = 1,
        MiddleButton = 2,
        XButton1 = 3,
        XButton2 = 4
    };
    Q_ENUM(MouseButton)

    explicit Clicker(QObject* parent = nullptr);
    ~Clicker() override = default;

    static bool isMouseButton(int keyCode);
    static MouseButton keyCodeToMouseButton(int keyCode);
    static QString keyCodeToString(int keyCode);
    static int stringToKeyCode(const QString& keyName);
    static void initializeKeyMap();

    Q_INVOKABLE void performClick(int keyCode);
    Q_INVOKABLE void performKeyPress(int keyCode);
    Q_INVOKABLE void performKeyRelease(int keyCode);

private:
    void performMouseClick(MouseButton button);
    void performMouseDown(MouseButton button);
    void performMouseUp(MouseButton button);
    void sendKeyboardInput(WORD vk, bool keyDown);

    static QHash<int, QString> s_keyNames;
    static QHash<QString, int> s_nameToKey;
    static bool s_initialized;
};