#include "clickprofile.h"

ClickProfile::ClickProfile(int id, QObject* parent)
    : QObject(parent)
    , m_id(id)
{
}

int ClickProfile::id() const
{
    return m_id;
}

QString ClickProfile::targetButton() const
{
    return m_targetButton;
}

void ClickProfile::setTargetButton(const QString& button)
{
    if (m_targetButton != button) {
        m_targetButton = button;
        emit targetButtonChanged();
    }
}

int ClickProfile::targetButtonCode() const
{
    return m_targetButtonCode;
}

void ClickProfile::setTargetButtonCode(int code)
{
    if (m_targetButtonCode != code) {
        m_targetButtonCode = code;
        emit targetButtonChanged();
    }
}

QString ClickProfile::keybind() const
{
    return m_keybind;
}

void ClickProfile::setKeybind(const QString& key)
{
    if (m_keybind != key) {
        m_keybind = key;
        emit keybindChanged();
    }
}

int ClickProfile::keybindCode() const
{
    return m_keybindCode;
}

void ClickProfile::setKeybindCode(int code)
{
    if (m_keybindCode != code) {
        m_keybindCode = code;
        emit keybindChanged();
    }
}

double ClickProfile::frequency() const
{
    return m_frequency;
}

void ClickProfile::setFrequency(double freq)
{
    if (!qFuzzyCompare(m_frequency, freq)) {
        m_frequency = freq;
        emit frequencyChanged();
    }
}

bool ClickProfile::randomFrequencyEnabled() const
{
    return m_randomFrequencyEnabled;
}

void ClickProfile::setRandomFrequencyEnabled(bool enabled)
{
    if (m_randomFrequencyEnabled != enabled) {
        m_randomFrequencyEnabled = enabled;
        emit randomFrequencyEnabledChanged();
    }
}

double ClickProfile::maxFrequency() const
{
    return m_maxFrequency;
}

void ClickProfile::setMaxFrequency(double freq)
{
    if (!qFuzzyCompare(m_maxFrequency, freq)) {
        m_maxFrequency = freq;
        emit maxFrequencyChanged();
    }
}

int ClickProfile::mode() const
{
    return m_mode;
}

void ClickProfile::setMode(int newMode)
{
    if (m_mode != newMode) {
        m_mode = newMode;
        emit modeChanged();
    }
}

bool ClickProfile::isActive() const
{
    return m_isActive;
}

void ClickProfile::setIsActive(bool active)
{
    if (m_isActive != active) {
        m_isActive = active;
        emit isActiveChanged();
    }
}
