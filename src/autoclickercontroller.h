#pragma once

#include <QObject>
#include <QTimer>
#include <QAbstractListModel>
#include <QRandomGenerator>
#include <QtQml/qqml.h>
#include "models/clickprofile.h"
#include "clicker.h"
#include "keybinder.h"

class AutoClickerController : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(int profileCount READ profileCount NOTIFY profileCountChanged)
    Q_PROPERTY(int listeningProfileId READ listeningProfileId NOTIFY listeningProfileIdChanged)
    Q_PROPERTY(bool isListeningForTarget READ isListeningForTarget NOTIFY isListeningChanged)
    Q_PROPERTY(bool isListeningForKeybind READ isListeningForKeybind NOTIFY isListeningChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TargetButtonRole,
        TargetButtonCodeRole,
        KeybindRole,
        KeybindCodeRole,
        FrequencyRole,
        RandomFrequencyEnabledRole,
        MaxFrequencyRole,
        ModeRole,
        IsActiveRole
    };

    explicit AutoClickerController(QObject* parent = nullptr);
    ~AutoClickerController() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Property getters
    int profileCount() const { return m_profiles.size(); }
    int listeningProfileId() const { return m_listeningProfileId; }
    bool isListeningForTarget() const { return m_isListeningForTarget; }
    bool isListeningForKeybind() const { return m_isListeningForKeybind; }

    // QML invokable methods
    Q_INVOKABLE void addProfile();
    Q_INVOKABLE void removeProfile(int profileId);
    Q_INVOKABLE void startListeningForTarget(int profileId);
    Q_INVOKABLE void startListeningForKeybind(int profileId);
    Q_INVOKABLE void stopListening();
    Q_INVOKABLE void setFrequency(int profileId, double frequency);
    Q_INVOKABLE void setRandomFrequencyEnabled(int profileId, bool enabled);
    Q_INVOKABLE void setMaxFrequency(int profileId, double frequency);
    Q_INVOKABLE void setMode(int profileId, int mode);
    Q_INVOKABLE void toggleProfile(int profileId);

signals:
    void profileCountChanged();
    void listeningProfileIdChanged();
    void isListeningChanged();

private slots:
    void onTargetButtonDetected(int profileId, int keyCode, const QString& keyName);
    void onKeybindDetected(int profileId, int keyCode, const QString& keyName);
    void onHotkeyPressed(int profileId);
    void onHotkeyReleased(int profileId);

private:
    Clicker* m_clicker;
    KeyBinder* m_keyBinder;
    QList<ClickProfile*> m_profiles;
    QHash<int, QTimer*> m_timers;
    QHash<int, bool> m_holdStates;
    int m_nextProfileId = 1;
    int m_listeningProfileId = -1;
    bool m_isListeningForTarget = false;
    bool m_isListeningForKeybind = false;

    ClickProfile* findProfile(int profileId);
    int intervalForProfile(const ClickProfile* profile) const;
    void scheduleNextClick(ClickProfile* profile);
};
