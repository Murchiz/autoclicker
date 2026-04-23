#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QtQml/qqml.h>
#include "autoclickercontroller.h"
#include "clicker.h"
#include "models/clickprofile.h"
#include "translationmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("AutoClicker");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("AutoClicker");

    // Initialize key map (must be called before any AutoClickerController is created)
    Clicker::initializeKeyMap();

    AutoClickerController autoClickerController;
    TranslationManager translationManager;

    // Register the module types explicitly so the app does not depend on Qt's
    // generated QML registration glue.
    qmlRegisterSingletonInstance("AutoClicker", 1, 0, "AutoClickerController", &autoClickerController);
    qmlRegisterSingletonInstance("AutoClicker", 1, 0, "TranslationManager", &translationManager);
    qmlRegisterUncreatableType<ClickProfile>("AutoClicker", 1, 0, "ClickProfile",
                                             "Cannot create ClickProfile in QML");

    QQmlApplicationEngine engine;

    // Load QML
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
