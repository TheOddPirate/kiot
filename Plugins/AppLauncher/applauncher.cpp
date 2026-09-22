// SPDX-FileCopyrightText: 2025-2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "applauncher.h"

#include <KIOTShared/kiotshared.h>
#include <KService>
#include <KServiceGroup>
#include <KProcess>
#include <KSandbox>
#include <KApplicationTrader>
#include <KIO/ApplicationLauncherJob>
#include <KConfigGroup>
#include <KSharedConfig>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariantMap>
#include <QTimer>
#include <QCollator>
#include <QLocale>
#include <QSet>
#include <QDateTime>
#include <QRegularExpression>

using KIOTShared::Entities::Select;
using KIOTShared::PlatformHelper;

Q_DECLARE_LOGGING_CATEGORY(appla_logger)

namespace
{
static const QRegularExpression invalidCharRegex("[^a-zA-Z0-9_-]");
}

AppLauncher::AppLauncher(QObject *parent)
    : QObject(parent)
    , m_select(nullptr)
{
    QStringList allowedCategories = ensureConfigAndGetAllowedCategories();
    discoverAllApplications(allowedCategories);
    
    if (m_appList.isEmpty()) {
        qCWarning(appla_logger) << "No applications found matching the configured categories. AppLauncher disabled.";
        return;
    }

    createAppLauncherEntity();
}

void AppLauncher::onOptionSelected(const QString &option)
{
    if (option == "Default" || !m_select) {
        return;
    }

    if (!m_appList.contains(option)) {
        qCWarning(appla_logger) << "Application not found in data:" << option;
        setToDefault();
        return;
    }

    AppData app = m_appList[option];

    if (PlatformHelper::isFlatpak()) {
        qCDebug(appla_logger) << "Launching application via host context (Flatpak):" << app.execCommand;
        QStringList parts = QProcess::splitCommand(app.execCommand);
        if (!parts.isEmpty()) {
            QString program = parts.first();
            QStringList arguments = parts.mid(1);
    
            qCDebug(appla_logger) << "Program:" << program;
            qCDebug(appla_logger) << "Arguments:" << arguments;
    
            KProcess *m_process = new KProcess(this);
            m_process->setProgram(program);
            m_process->setArguments(arguments);
            KSandbox::ProcessContext ctx = KSandbox::makeHostContext(*m_process);
            m_process->setProgram(ctx.program);
            m_process->setArguments(ctx.arguments);
            m_process->startDetached();
        }            
    } else {
        KService::Ptr service = KService::serviceByDesktopName(app.desktopFileName);
        if (service) {
            qCDebug(appla_logger) << "Launching application natively:" << app.name;

            auto *job = new KIO::ApplicationLauncherJob(service);
            job->start();
        } else {
            qCWarning(appla_logger) << "Failed to resolve KService for application:" << app.name;
            setToDefault();
            return;
        }
    }

    QVariantMap attributes = app.toVariantMap();
    attributes["last_launched_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    m_select->setAttributes(attributes);

    setToDefault();
}

void AppLauncher::setToDefault()
{
    if (m_select) {
        // Reserved for resetting state if needed
    }
}

QString AppLauncher::sanitizeCategoryName(const QString &category)
{
    QString id = category.toLower();
    id.replace(invalidCharRegex, QStringLiteral("_"));
    if (!id.isEmpty() && id[0].isDigit()) {
        id.prepend("cat_");
    }
    return id;
}

QList<QString> AppLauncher::sortAlphabetically(const QList<QString> &input)
{
    QList<QString> sorted = input;

    QCollator collator(QLocale::system());
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setNumericMode(true);

    std::sort(sorted.begin(), sorted.end(),
              [&collator](const QString &a, const QString &b) {
                  return collator.compare(a, b) < 0;
              });

    return sorted;
}

QStringList AppLauncher::ensureConfigAndGetAllowedCategories()
{
    QSet<QString> systemCategories;
    const KService::List services = KService::allServices();
    for (const KService::Ptr &service : services) {
        if (service->isApplication() && !service->noDisplay()) {
            const QStringList cats = service->categories();
            for (const QString &cat : cats) {
                if (!cat.trimmed().isEmpty()) {
                    systemCategories.insert(cat.trimmed());
                }
            }
        }
    }

    auto config = KSharedConfig::openConfig(PlatformHelper::configFilePath(), KConfig::SimpleConfig);
    KConfigGroup group(config, "AppLauncher");
 
    bool configChanged = false;
    QStringList allowedCategories;

    for (const QString &cat : systemCategories) {
        QString configKey = sanitizeCategoryName(cat);
        if (!group.hasKey(configKey)) {
            group.writeEntry(configKey, true);
            configChanged = true;
            qCDebug(appla_logger) << "Added new application category to config:" << configKey << "= true";
        }

        if (group.readEntry(configKey, true)) {
            allowedCategories.append(cat);
        }
    }

    if (configChanged) {
        group.sync(); 
        qCInfo(appla_logger) << "AppLauncher config updated with system categories.";
    }

    return allowedCategories;
}

void AppLauncher::discoverAllApplications(const QStringList &allowedCategories)
{
    QMap<QString, AppData> apps;
    const KService::List services = KService::allServices();

    for (const KService::Ptr &service : services) {
        if (!service->isApplication() || service->noDisplay()) {
            continue;
        }

        const QStringList serviceCategories = service->categories();
        bool matchesCategory = allowedCategories.isEmpty();

        if (!matchesCategory) {
            for (const QString &cat : allowedCategories) {
                if (serviceCategories.contains(cat, Qt::CaseInsensitive)) {
                    matchesCategory = true;
                    break;
                }
            }
        }

        if (!matchesCategory) {
            continue;
        }

        AppData data;
        data.name = service->name();
        data.desktopFileName = service->desktopEntryName();
        data.execCommand = service->exec();
        data.iconName = service->icon();
        data.categories = serviceCategories;

        apps[data.name] = data;
    }

    m_appList = apps;
    qCInfo(appla_logger) << "Total applications discovered:" << m_appList.size();
}

void AppLauncher::createAppLauncherEntity()
{
    m_select = new Select(this);
    m_select->setId("app_launcher");
    m_select->setName("Application Launcher");
    m_select->setDiscoveryConfig("icon", "mdi:application-cog");

    QStringList options;
    for (auto it = m_appList.constBegin(); it != m_appList.constEnd(); ++it) {
        options.append(it.key());
    }

    options = sortAlphabetically(options);
    options.prepend("Default");
    m_select->setOptions(options);
    m_select->setState("Default");

    connect(m_select, &Select::optionSelected, this, &AppLauncher::onOptionSelected);

    qCInfo(appla_logger) << "Exposed" << options.size() - 1 << "applications in HA select entity";
}