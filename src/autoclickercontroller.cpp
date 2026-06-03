#include "autoclickercontroller.h"
#include <QDebug>
#include <QSettings>

AutoClickerController::AutoClickerController(QObject* parent)
    : QAbstractListModel(parent)
    , m_clicker(new Clicker(this))
    , m_keyBinder(new KeyBinder(this))
{
    connect(m_keyBinder, &KeyBinder::targetButtonDetected,
            this, &AutoClickerController::onTargetButtonDetected);
    connect(m_keyBinder, &KeyBinder::keybindDetected,
            this, &AutoClickerController::onKeybindDetected);
    connect(m_keyBinder, &KeyBinder::hotkeyPressed,
            this, &AutoClickerController::onHotkeyPressed);
    connect(m_keyBinder, &KeyBinder::hotkeyReleased,
            this, &AutoClickerController::onHotkeyReleased);

    loadProfiles();
}

AutoClickerController::~AutoClickerController()
{
    saveProfiles();
    qDeleteAll(m_profiles);
    qDeleteAll(m_timers);
}

int AutoClickerController::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_profiles.size();
}

QVariant AutoClickerController::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_profiles.size()) {
        return QVariant();
    }

    ClickProfile* profile = m_profiles[index.row()];

    switch (role) {
    case IdRole:
        return profile->id();
    case TargetButtonRole:
        return profile->targetButton();
    case TargetButtonCodeRole:
        return profile->targetButtonCode();
    case KeybindRole:
        return profile->keybind();
    case KeybindCodeRole:
        return profile->keybindCode();
    case FrequencyRole:
        return profile->frequency();
    case RandomFrequencyEnabledRole:
        return profile->randomFrequencyEnabled();
    case MaxFrequencyRole:
        return profile->maxFrequency();
    case ModeRole:
        return profile->mode();
    case IsActiveRole:
        return profile->isActive();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> AutoClickerController::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "profileId";
    roles[TargetButtonRole] = "targetButton";
    roles[TargetButtonCodeRole] = "targetButtonCode";
    roles[KeybindRole] = "keybind";
    roles[KeybindCodeRole] = "keybindCode";
    roles[FrequencyRole] = "frequency";
    roles[RandomFrequencyEnabledRole] = "randomFrequencyEnabled";
    roles[MaxFrequencyRole] = "maxFrequency";
    roles[ModeRole] = "mode";
    roles[IsActiveRole] = "isActive";
    return roles;
}

void AutoClickerController::addProfile()
{
    beginInsertRows(QModelIndex(), m_profiles.size(), m_profiles.size());
    
    ClickProfile* profile = new ClickProfile(m_nextProfileId++, this);
    m_profiles.append(profile);
    
    // Create timer for this profile
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    m_timers[profile->id()] = timer;
    m_holdStates[profile->id()] = false;
    
    connect(timer, &QTimer::timeout, this, [this, profile]() {
        if (profile->isActive()) {
            m_clicker->performClick(profile->targetButtonCode());
            scheduleNextClick(profile);
        }
    });
    
    endInsertRows();
    emit profileCountChanged();
}

void AutoClickerController::removeProfile(int profileId)
{
    int index = -1;
    for (int i = 0; i < m_profiles.size(); ++i) {
        if (m_profiles[i]->id() == profileId) {
            index = i;
            break;
        }
    }

    if (index < 0) return;

    // Stop and remove timer
    if (m_timers.contains(profileId)) {
        m_timers[profileId]->stop();
        delete m_timers.take(profileId);
    }
    
    // Unregister hotkey
    m_keyBinder->unregisterHotkey(profileId);
    
    m_holdStates.remove(profileId);

    beginRemoveRows(QModelIndex(), index, index);
    delete m_profiles.takeAt(index);
    endRemoveRows();
    emit profileCountChanged();
}

void AutoClickerController::startListeningForTarget(int profileId)
{
    m_listeningProfileId = profileId;
    m_isListeningForTarget = true;
    m_isListeningForKeybind = false;
    emit listeningProfileIdChanged();
    emit isListeningChanged();
    
    m_keyBinder->startListeningForTarget(profileId);
}

void AutoClickerController::startListeningForKeybind(int profileId)
{
    m_listeningProfileId = profileId;
    m_isListeningForTarget = false;
    m_isListeningForKeybind = true;
    emit listeningProfileIdChanged();
    emit isListeningChanged();
    
    m_keyBinder->startListeningForKeybind(profileId);
}

void AutoClickerController::stopListening()
{
    m_listeningProfileId = -1;
    m_isListeningForTarget = false;
    m_isListeningForKeybind = false;
    emit listeningProfileIdChanged();
    emit isListeningChanged();
    
    m_keyBinder->stopListening();
}

void AutoClickerController::setFrequency(int profileId, double frequency)
{
    ClickProfile* profile = findProfile(profileId);
    if (!profile) return;
    
    profile->setFrequency(frequency);
    if (profile->maxFrequency() < frequency) {
        profile->setMaxFrequency(frequency);
    }
    
    // Update timer interval if active
    if (m_timers.contains(profileId) && profile->isActive()) {
        scheduleNextClick(profile);
    }
    
    QModelIndex idx = index(m_profiles.indexOf(profile));
    emit dataChanged(idx, idx, {FrequencyRole, MaxFrequencyRole});
}

void AutoClickerController::setRandomFrequencyEnabled(int profileId, bool enabled)
{
    ClickProfile* profile = findProfile(profileId);
    if (!profile) return;

    profile->setRandomFrequencyEnabled(enabled);

    if (m_timers.contains(profileId) && profile->isActive()) {
        scheduleNextClick(profile);
    }

    QModelIndex idx = index(m_profiles.indexOf(profile));
    emit dataChanged(idx, idx, {RandomFrequencyEnabledRole});
}

void AutoClickerController::setMaxFrequency(int profileId, double frequency)
{
    ClickProfile* profile = findProfile(profileId);
    if (!profile) return;

    const double normalized = frequency < profile->frequency()
        ? profile->frequency()
        : frequency;
    profile->setMaxFrequency(normalized);

    if (m_timers.contains(profileId) && profile->isActive()) {
        scheduleNextClick(profile);
    }

    QModelIndex idx = index(m_profiles.indexOf(profile));
    emit dataChanged(idx, idx, {MaxFrequencyRole});
}

void AutoClickerController::setMode(int profileId, int mode)
{
    ClickProfile* profile = findProfile(profileId);
    if (!profile) return;

    if (profile->isActive()) {
        profile->setIsActive(false);
        if (m_timers.contains(profileId)) {
            m_timers[profileId]->stop();
        }
        m_holdStates[profileId] = false;
    }
    
    profile->setMode(mode);
    
    QModelIndex idx = index(m_profiles.indexOf(profile));
    emit dataChanged(idx, idx, {ModeRole, IsActiveRole});
}

void AutoClickerController::toggleProfile(int profileId) {
    ClickProfile* profile = findProfile(profileId);
    if (!profile) return;
    
    // Only allow toggle mode to be toggled via this method
    if (profile->mode() == ClickProfile::Hold) return;
    
    bool newState = !profile->isActive();
    profile->setIsActive(newState);
    if (m_timers.contains(profileId)) {
        QTimer* timer = m_timers[profileId];
        if (newState) {
            scheduleNextClick(profile);
        } else {
            timer->stop();
        }
    }
    QModelIndex idx = index(m_profiles.indexOf(profile));
    emit dataChanged(idx, idx, {IsActiveRole});
}

void AutoClickerController::onTargetButtonDetected(int profileId, int keyCode, const QString& keyName)
{
    ClickProfile* profile = findProfile(profileId);
    if (!profile) return;
    
    profile->setTargetButton(keyName);
    profile->setTargetButtonCode(keyCode);
    
    QModelIndex idx = index(m_profiles.indexOf(profile));
    emit dataChanged(idx, idx, {TargetButtonRole, TargetButtonCodeRole});
    
    stopListening();
}

void AutoClickerController::onKeybindDetected(int profileId, int keyCode, const QString& keyName)
{
    ClickProfile* profile = findProfile(profileId);
    if (!profile) return;
    
    // Unregister old hotkey if exists
    m_keyBinder->unregisterHotkey(profileId);
    
    profile->setKeybind(keyName);
    profile->setKeybindCode(keyCode);
    
    // Register new hotkey
    m_keyBinder->registerHotkey(profileId, keyCode);
    
    QModelIndex idx = index(m_profiles.indexOf(profile));
    emit dataChanged(idx, idx, {KeybindRole, KeybindCodeRole});
    
    stopListening();
}

void AutoClickerController::onHotkeyPressed(int profileId) {
    ClickProfile* profile = findProfile(profileId);
    if (!profile) return;
    
    if (profile->mode() == ClickProfile::Toggle) {
        // Toggle mode: press once to start, press again to stop
        toggleProfile(profileId);
    } else {
        // Hold mode: click while key is held
        if (m_holdStates.value(profileId, false)) {
            return;
        }
        m_holdStates[profileId] = true;
        profile->setIsActive(true);
        m_clicker->performClick(profile->targetButtonCode());
        scheduleNextClick(profile);
        QModelIndex idx = index(m_profiles.indexOf(profile));
        emit dataChanged(idx, idx, {IsActiveRole});
    }
}

void AutoClickerController::onHotkeyReleased(int profileId)
{
    ClickProfile* profile = findProfile(profileId);
    if (!profile) return;
    
    if (profile->mode() == ClickProfile::Hold) {
        // Hold mode: stop when key is released
        m_holdStates[profileId] = false;
        profile->setIsActive(false);
        
        if (m_timers.contains(profileId)) {
            m_timers[profileId]->stop();
        }
        
        QModelIndex idx = index(m_profiles.indexOf(profile));
        emit dataChanged(idx, idx, {IsActiveRole});
    }
}

ClickProfile* AutoClickerController::findProfile(int profileId)
{
    for (ClickProfile* profile : m_profiles) {
        if (profile->id() == profileId) {
            return profile;
        }
    }
    return nullptr;
}

int AutoClickerController::intervalForProfile(const ClickProfile* profile) const
{
    if (!profile) {
        return 10;
    }

    double minFrequency = profile->frequency();
    double maxFrequency = profile->randomFrequencyEnabled()
        ? qMax(profile->frequency(), profile->maxFrequency())
        : profile->frequency();

    double selectedFrequency = minFrequency;
    if (profile->randomFrequencyEnabled() && !qFuzzyCompare(minFrequency, maxFrequency)) {
        const double span = maxFrequency - minFrequency;
        selectedFrequency = minFrequency + (QRandomGenerator::global()->generateDouble() * span);
    }

    return qMax(10, static_cast<int>(selectedFrequency * 1000.0));
}

void AutoClickerController::scheduleNextClick(ClickProfile* profile)
{
    if (!profile || !profile->isActive() || !m_timers.contains(profile->id())) {
        return;
    }

    QTimer* timer = m_timers[profile->id()];
    timer->start(intervalForProfile(profile));
}

void AutoClickerController::saveProfiles()
{
    QSettings settings;
    settings.beginWriteArray("Profiles");
    for (int i = 0; i < m_profiles.size(); ++i) {
        settings.setArrayIndex(i);
        ClickProfile* p = m_profiles[i];
        settings.setValue("targetButton", p->targetButton());
        settings.setValue("targetButtonCode", p->targetButtonCode());
        settings.setValue("keybind", p->keybind());
        settings.setValue("keybindCode", p->keybindCode());
        settings.setValue("frequency", p->frequency());
        settings.setValue("randomFrequencyEnabled", p->randomFrequencyEnabled());
        settings.setValue("maxFrequency", p->maxFrequency());
        settings.setValue("mode", p->mode());
    }
    settings.endArray();
    settings.setValue("nextProfileId", m_nextProfileId);
}

void AutoClickerController::loadProfiles()
{
    QSettings settings;
    int size = settings.beginReadArray("Profiles");
    
    if (size > 0) {
        beginInsertRows(QModelIndex(), 0, size - 1);
        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);
            ClickProfile* p = new ClickProfile(m_nextProfileId++, this);
            p->setTargetButton(settings.value("targetButton", "").toString());
            p->setTargetButtonCode(settings.value("targetButtonCode", 0).toInt());
            p->setKeybind(settings.value("keybind", "").toString());
            p->setKeybindCode(settings.value("keybindCode", 0).toInt());
            p->setFrequency(settings.value("frequency", 1.0).toDouble());
            p->setRandomFrequencyEnabled(settings.value("randomFrequencyEnabled", false).toBool());
            p->setMaxFrequency(settings.value("maxFrequency", 1.0).toDouble());
            p->setMode(settings.value("mode", ClickProfile::Toggle).toInt());
            p->setIsActive(false);
            
            m_profiles.append(p);
            
            if (p->keybindCode() > 0) {
                m_keyBinder->registerHotkey(p->id(), p->keybindCode());
            }

            QTimer* timer = new QTimer(this);
            timer->setSingleShot(true);
            m_timers[p->id()] = timer;
            m_holdStates[p->id()] = false;

            connect(timer, &QTimer::timeout, this, [this, p]() {
                if (p->isActive()) {
                    m_clicker->performClick(p->targetButtonCode());
                    scheduleNextClick(p);
                }
            });
        }
        endInsertRows();
        emit profileCountChanged();
    }
    settings.endArray();
    
    if (settings.contains("nextProfileId")) {
        m_nextProfileId = qMax(m_nextProfileId, settings.value("nextProfileId").toInt());
    }
}
