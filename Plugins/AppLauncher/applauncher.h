// SPDX-FileCopyrightText: 2025-2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariantMap>
#include <KIOTShared/kiotshared.h>

using KIOTShared::Entities::Select;

/**
 * @class AppLauncher
 * @brief Application launcher integration core logic for KIOT.
 *
 * @details Discovers installed desktop applications using KDE's KService framework,
 *          manages categories, and handles execution/launching.
 */
class AppLauncher : public QObject
{
    Q_OBJECT

public:
    explicit AppLauncher(QObject *parent = nullptr);
    ~AppLauncher() override = default;

private Q_SLOTS:
    void onOptionSelected(const QString &option);

private:
    struct AppData {
        QString name;           
        QString desktopFileName;
        QString execCommand;    
        QString iconName;        
        QStringList categories;  

        QVariantMap toVariantMap() const {
            QVariantMap map;
            map["name"] = name;
            map["desktopFileName"] = desktopFileName;
            map["execCommand"] = execCommand;
            map["iconName"] = iconName;
            map["categories"] = QVariant::fromValue(categories);
            return map;
        }
    };

    void setToDefault();
    QString sanitizeCategoryName(const QString &category);
    QList<QString> sortAlphabetically(const QList<QString> &input);
    QStringList ensureConfigAndGetAllowedCategories();
    void discoverAllApplications(const QStringList &allowedCategories);
    void createAppLauncherEntity();

    KIOTShared::Entities::Select *m_select;
    QMap<QString, AppData> m_appList;
};