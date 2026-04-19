#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include "clicker.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("AutoClicker");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("AutoClicker");

    // Initialize key map (must be called before any AutoClickerController is created)
    Clicker::initializeKeyMap();

    QQmlApplicationEngine engine;

    // Types are auto-registered through QML_ELEMENT/QML_SINGLETON macros
    // in the AutoClicker QML module (see qt_add_qml_module in CMakeLists.txt)

    // Load QML
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}