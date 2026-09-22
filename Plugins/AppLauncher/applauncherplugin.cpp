// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file template.cpp
 * @brief Implementation of the KIOT plugin template.
 */

#include "applauncherplugin.h"

#include <QCoreApplication>

// Add the entities you need from the shared lib like this:
// using KIOTShared::Entities::Sensor;

using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(appla_logger, PLUGIN_NAME)


AppLauncherPlugin::AppLauncherPlugin(QObject *parent)
    : QObject(parent)
{
}
AppLauncherPlugin::~AppLauncherPlugin()
{
    stopPlugin();
}
QString AppLauncherPlugin::name() const
{
    return QString(PLUGIN_NAME).replace("\"", "");
}

QString AppLauncherPlugin::description() const
{
    return QString(PLUGIN_DESCRIPTION).replace("\"", "") + QString(" ") + QString(PLUGIN_DOMAIN).replace("\"", "");
}
QUrl AppLauncherPlugin::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}
QVersionNumber AppLauncherPlugin::version() const
{
    QString version = QString(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool AppLauncherPlugin::checkCompatibility()
{
    if(PlatformHelper::isFlatpak())
    {
        qCInfo(appla_logger) << name() << " is not supported on Flatpak, we recommend disabling this plugin for now";
        return false;
    }
    auto desktopEnviornment = PlatformHelper::detectDesktopEnvironment();
    if(desktopEnviornment == "kde")
        return true;
    return false;
}

bool AppLauncherPlugin::enabledByDefault()
{
    if(!checkCompatibility())
        return false;

    return true;
}


bool AppLauncherPlugin::startPlugin()
{
    if(!checkCompatibility())
        return false;
    if(m_appLauncher)
        stopPlugin();
    m_appLauncher = new AppLauncher(this);

    qCInfo(appla_logger) << name() << " plugin started successfully";
    return true;
}

bool AppLauncherPlugin::stopPlugin()
{
    if(m_appLauncher)
    {
        m_appLauncher->deleteLater();
        m_appLauncher = nullptr;
    }
    qCInfo(appla_logger) << name() << " plugin stopped";
    return true;
}

#include "applauncherplugin.moc"