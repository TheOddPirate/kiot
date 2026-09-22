#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonValue>
#include <QLoggingCategory>
#include <KIOTShared/kiotshared_export.h>

class QFileSystemWatcher; // Forward deklarasjon

namespace KIOTShared {
namespace Config {

class KIOT_SHARED_EXPORT ConfigManager : public QObject
{
    Q_OBJECT

public:
    struct JsonResult {
        bool success;
        QJsonObject data;
    };

    // --- KUN CORE SKAL BRUKE DISSE ---
    // Core uten defaults (Kan KUN kalles fra main/systemet)
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager() override;
    // Core MED defaults (Brukt i main.cpp)
    explicit ConfigManager(const QJsonObject &defaultConfig, QObject *parent = nullptr);

    // --- PLUGINS MÅ BRUKE DISSE ---
    // Plugin-konfigurasjon (MÅ angi modulnavn!)
    explicit ConfigManager(const QString &moduleName, QObject *parent = nullptr);

    // Plugin-konfigurasjon MED default data (MÅ angi modulnavn!)
    explicit ConfigManager(const QString &moduleName, const QJsonObject &defaultConfig, QObject *parent = nullptr);

    // Statisk validering av JSON-streng
    static JsonResult validateJsonString(const QString &json);

    // Metoder for lesing og skriving
    QJsonValue value(const QString &keyPath, const QJsonValue &defaultValue = QJsonValue()) const;
    void setValue(const QString &keyPath, const QJsonValue &value);
    bool contains(const QString &keyPath) const;
    void remove(const QString &keyPath);
    void resetToDefaults();
    QString filePath();
    void setDefaultConfig(const QJsonObject &defaultConfig);

signals:
    void configChanged(const QString &keyPath, const QJsonValue &newValue);
    void configParseError(const QString &filePath, const QString &errorString); 

private:
    enum class ConfigType {
        Core,
        Plugin
    };

    // MASTER-KONSTRUKTØREN ER NÅ PRIVATE!
    // Ingen eksterne filer (hverken main eller plugins) kan kalle denne direkte.
    explicit ConfigManager(ConfigType type, const QString &moduleName, const QJsonObject &defaultConfig, QObject *parent);

    ConfigType m_type;
    QString m_moduleName;
    QString m_filePath;
    QJsonObject m_data;
    QJsonObject m_defaultData;

    QFileSystemWatcher *m_fileWatcher = nullptr;
    bool m_isSaving = false; // For å unngå at vi triggermelding på egne lagringer
    
    void setupFilePath(ConfigType type, const QString &moduleName);
    QString sanitizeModuleName(const QString &rawName) const;
    void validateAndMergeDefaults();
    bool mergeObjects(QJsonObject &target, const QJsonObject &defaults);

    QJsonValue getRawValue(const QJsonObject &source, const QString &keyPath) const;
    void setRawValue(QJsonObject &target, const QString &keyPath, const QJsonValue &value);

private slots:
    void handleFileChanged(const QString &path);
};

}
}