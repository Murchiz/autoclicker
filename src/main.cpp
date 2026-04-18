#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "autoclickercontroller.h"
#include "clicker.h"
#include "translationmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("AutoClicker");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("AutoClicker");

    // Initialize key map
    Clicker::initializeKeyMap();

    // Create controller
    AutoClickerController controller;
    TranslationManager translationManager;

    QQmlApplicationEngine engine;

    // Register types for QML
    qmlRegisterUncreatableType<ClickProfile>("AutoClicker", 1, 0, "ClickProfile",
        "Cannot create ClickProfile in QML");
    qmlRegisterType<AutoClickerController>("AutoClicker", 1, 0, "AutoClickerController");

    // Set context properties
    engine.rootContext()->setContextProperty("autoClickerController", &controller);
    engine.rootContext()->setContextProperty("translationManager", &translationManager);

    // Load QML
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
