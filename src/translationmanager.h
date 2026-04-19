#pragma once

#include <QObject>
#include <QVariantList>
#include <QtQml/qqml.h>

class TranslationManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QString currentLanguage READ currentLanguage NOTIFY currentLanguageChanged)
    Q_PROPERTY(QVariantList availableLanguages READ availableLanguages CONSTANT)

public:
    explicit TranslationManager(QObject* parent = nullptr);

    QString currentLanguage() const;
    QVariantList availableLanguages() const;

    Q_INVOKABLE QString textFor(const QString& key) const;
    Q_INVOKABLE void setLanguage(const QString& languageCode);

signals:
    void currentLanguageChanged();

private:
    QString m_currentLanguage;
    QVariantList m_availableLanguages;
};
