#pragma once

#include <QObject>
#include <QString>

class ClickProfile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString targetButton READ targetButton WRITE setTargetButton NOTIFY targetButtonChanged)
    Q_PROPERTY(int targetButtonCode READ targetButtonCode WRITE setTargetButtonCode NOTIFY targetButtonChanged)
    Q_PROPERTY(QString keybind READ keybind WRITE setKeybind NOTIFY keybindChanged)
    Q_PROPERTY(int keybindCode READ keybindCode WRITE setKeybindCode NOTIFY keybindChanged)
    Q_PROPERTY(double frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(bool randomFrequencyEnabled READ randomFrequencyEnabled WRITE setRandomFrequencyEnabled NOTIFY randomFrequencyEnabledChanged)
    Q_PROPERTY(double maxFrequency READ maxFrequency WRITE setMaxFrequency NOTIFY maxFrequencyChanged)
    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE setIsActive NOTIFY isActiveChanged)

public:
    enum ClickMode {
        Toggle = 0,
        Hold = 1
    };
    Q_ENUM(ClickMode)

    explicit ClickProfile(int id, QObject* parent = nullptr);
    ~ClickProfile() override = default;

    int id() const;
    
    QString targetButton() const;
    void setTargetButton(const QString& button);
    
    int targetButtonCode() const;
    void setTargetButtonCode(int code);
    
    QString keybind() const;
    void setKeybind(const QString& key);
    
    int keybindCode() const;
    void setKeybindCode(int code);
    
    double frequency() const;
    void setFrequency(double freq);

    bool randomFrequencyEnabled() const;
    void setRandomFrequencyEnabled(bool enabled);

    double maxFrequency() const;
    void setMaxFrequency(double freq);
    
    int mode() const;
    void setMode(int newMode);
    
    bool isActive() const;
    void setIsActive(bool active);

signals:
    void targetButtonChanged();
    void keybindChanged();
    void frequencyChanged();
    void randomFrequencyEnabledChanged();
    void maxFrequencyChanged();
    void modeChanged();
    void isActiveChanged();

private:
    int m_id;
    QString m_targetButton;
    int m_targetButtonCode = 0;
    QString m_keybind;
    int m_keybindCode = 0;
    double m_frequency = 1.0;
    bool m_randomFrequencyEnabled = false;
    double m_maxFrequency = 1.0;
    int m_mode = Toggle;
    bool m_isActive = false;
};
