#include "translationmanager.h"

#include <QSettings>
#include <QVariantMap>
#include <unordered_map>

namespace {
using TranslationMap = std::unordered_map<QString, QString>;

const TranslationMap& englishTranslations()
{
    static const TranslationMap translations = {
        {QStringLiteral("app_title"), QStringLiteral("AutoClicker")},
        {QStringLiteral("profiles_count"), QStringLiteral("Profiles: %1")},
        {QStringLiteral("language"), QStringLiteral("Language")},
        {QStringLiteral("press_target"), QStringLiteral("Press any key or mouse button to set target...")},
        {QStringLiteral("press_keybind"), QStringLiteral("Press any key or mouse button to set keybind...")},
        {QStringLiteral("empty_profiles"), QStringLiteral("No profiles added.\nClick 'Add Profile' to create one.")},
        {QStringLiteral("add_profile"), QStringLiteral("Add Profile")},
        {QStringLiteral("footer_hint"), QStringLiteral("Toggle: Press keybind once to start/stop | Hold: Keep keybind pressed")},
        {QStringLiteral("waiting_input"), QStringLiteral("Waiting for input...")},
        {QStringLiteral("cancel"), QStringLiteral("Cancel")},
        {QStringLiteral("target_button"), QStringLiteral("Target Button:")},
        {QStringLiteral("keybind"), QStringLiteral("Keybind:")},
        {QStringLiteral("set_target"), QStringLiteral("Select button")},
        {QStringLiteral("set_keybind"), QStringLiteral("Assign hotkey")},
        {QStringLiteral("frequency_sec"), QStringLiteral("Frequency (sec):")},
        {QStringLiteral("min_frequency_sec"), QStringLiteral("Min interval (sec):")},
        {QStringLiteral("max_frequency_sec"), QStringLiteral("Max interval (sec):")},
        {QStringLiteral("random_frequency"), QStringLiteral("Random")},
        {QStringLiteral("mode"), QStringLiteral("Mode:")},
        {QStringLiteral("toggle"), QStringLiteral("Toggle")},
        {QStringLiteral("hold"), QStringLiteral("Hold")},
        {QStringLiteral("active"), QStringLiteral("ACTIVE")},
        {QStringLiteral("inactive"), QStringLiteral("INACTIVE")},
        {QStringLiteral("use_key"), QStringLiteral("Use Key")},
        {QStringLiteral("stop"), QStringLiteral("Stop")},
        {QStringLiteral("start"), QStringLiteral("Start")}
    };
    return translations;
}

const TranslationMap& russianTranslations()
{
    static const TranslationMap translations = {
        {QStringLiteral("app_title"), QStringLiteral("Автокликер")},
        {QStringLiteral("profiles_count"), QStringLiteral("Профилей: %1")},
        {QStringLiteral("language"), QStringLiteral("Язык")},
        {QStringLiteral("press_target"), QStringLiteral("Нажмите любую клавишу или кнопку мыши, чтобы выбрать кнопку нажатия...")},
        {QStringLiteral("press_keybind"), QStringLiteral("Нажмите любую клавишу или кнопку мыши, чтобы назначить горячую клавишу...")},
        {QStringLiteral("empty_profiles"), QStringLiteral("Пока нет ни одного профиля.\nНажмите «Добавить профиль», чтобы создать первый.")},
        {QStringLiteral("add_profile"), QStringLiteral("Добавить профиль")},
        {QStringLiteral("footer_hint"), QStringLiteral("Переключение: одно нажатие запускает или останавливает профиль | Удержание: профиль работает, пока клавиша зажата")},
        {QStringLiteral("waiting_input"), QStringLiteral("Ожидание ввода")},
        {QStringLiteral("cancel"), QStringLiteral("Отмена")},
        {QStringLiteral("target_button"), QStringLiteral("Кнопка нажатия:")},
        {QStringLiteral("keybind"), QStringLiteral("Горячая клавиша:")},
        {QStringLiteral("set_target"), QStringLiteral("Выбрать кнопку")},
        {QStringLiteral("set_keybind"), QStringLiteral("Задать клавишу")},
        {QStringLiteral("frequency_sec"), QStringLiteral("Интервал (сек):")},
        {QStringLiteral("min_frequency_sec"), QStringLiteral("Мин. интервал (сек):")},
        {QStringLiteral("max_frequency_sec"), QStringLiteral("Макс. интервал (сек):")},
        {QStringLiteral("random_frequency"), QStringLiteral("Случайно")},
        {QStringLiteral("mode"), QStringLiteral("Режим:")},
        {QStringLiteral("toggle"), QStringLiteral("Перекл.")},
        {QStringLiteral("hold"), QStringLiteral("Удерж.")},
        {QStringLiteral("active"), QStringLiteral("ВКЛЮЧЕН")},
        {QStringLiteral("inactive"), QStringLiteral("ВЫКЛЮЧЕН")},
        {QStringLiteral("use_key"), QStringLiteral("Зажмите")},
        {QStringLiteral("stop"), QStringLiteral("Остановить")},
        {QStringLiteral("start"), QStringLiteral("Старт")}
    };
    return translations;
}

const TranslationMap& translationsForLanguage(const QString& languageCode)
{
    if (languageCode == QStringLiteral("ru")) {
        return russianTranslations();
    }
    return englishTranslations();
}
}

TranslationManager::TranslationManager(QObject* parent)
    : QObject(parent)
{
    m_availableLanguages = {
        QVariantMap{
            {QStringLiteral("code"), QStringLiteral("en")},
            {QStringLiteral("name"), QStringLiteral("English")}
        },
        QVariantMap{
            {QStringLiteral("code"), QStringLiteral("ru")},
            {QStringLiteral("name"), QStringLiteral("Русский")}
        }
    };

    QSettings settings;
    const QString storedLanguage = settings.value(QStringLiteral("ui/language"),
                                                  QStringLiteral("ru")).toString();
    m_currentLanguage = storedLanguage == QStringLiteral("en")
        ? QStringLiteral("en")
        : QStringLiteral("ru");
}

QString TranslationManager::currentLanguage() const
{
    return m_currentLanguage;
}

QVariantList TranslationManager::availableLanguages() const
{
    return m_availableLanguages;
}

QString TranslationManager::textFor(const QString& key) const
{
    const TranslationMap& activeTranslations = translationsForLanguage(m_currentLanguage);
    const auto activeIt = activeTranslations.find(key);
    if (activeIt != activeTranslations.end()) {
        return activeIt->second;
    }

    const TranslationMap& fallbackTranslations = englishTranslations();
    const auto fallbackIt = fallbackTranslations.find(key);
    if (fallbackIt != fallbackTranslations.end()) {
        return fallbackIt->second;
    }

    return key;
}

void TranslationManager::setLanguage(const QString& languageCode)
{
    const QString normalizedLanguage = languageCode == QStringLiteral("ru")
        ? QStringLiteral("ru")
        : QStringLiteral("en");

    if (m_currentLanguage == normalizedLanguage) {
        return;
    }

    m_currentLanguage = normalizedLanguage;

    QSettings settings;
    settings.setValue(QStringLiteral("ui/language"), m_currentLanguage);

    emit currentLanguageChanged();
}
