#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QDebug>
#include <QFile>
#include <QTextStream>
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

    const QString startupLogPath = QGuiApplication::applicationDirPath() + "/autoclicker-startup.log";
    const auto appendStartupLog = [&startupLogPath](const QString &message) {
        QFile logFile(startupLogPath);
        if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
            return;
        }

        QTextStream stream(&logFile);
        stream << message << '\n';
    };

    // Register the module types explicitly so the app does not depend on Qt's
    // generated QML registration glue.
    qmlRegisterSingletonInstance("AutoClicker", 1, 0, "AutoClickerController", &autoClickerController);
    qmlRegisterSingletonInstance("AutoClicker", 1, 0, "TranslationManager", &translationManager);
    qmlRegisterUncreatableType<ClickProfile>("AutoClicker", 1, 0, "ClickProfile",
                                             "Cannot create ClickProfile in QML");

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlEngine::warnings, &app, [](const QList<QQmlError> &warnings) {
        for (const QQmlError &warning : warnings) {
            qWarning().noquote() << warning.toString();
        }
    });
    QObject::connect(&engine, &QQmlEngine::warnings, &app, [&appendStartupLog](const QList<QQmlError> &warnings) {
        for (const QQmlError &warning : warnings) {
            appendStartupLog(warning.toString());
        }
    });

    // Load QML
    const QUrl url(QStringLiteral("qrc:/AutoClicker/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url, &appendStartupLog](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qWarning().noquote() << "Failed to create root QML object from" << url.toString();
            appendStartupLog(QStringLiteral("Failed to create root QML object from %1").arg(url.toString()));
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
