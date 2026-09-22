// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file systemdplugin.cpp
 * @brief Implementation of the KIOT plugin systemdplugin.
 */

#include "systemdplugin.h"
#include <QString>
#include <QCoreApplication>

// Add the entities you need from the shared lib like this:
// using KIOTShared::Entities::Sensor;

using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(plugin_logger, SystemD)

SystemdPlugin::SystemdPlugin(QObject *parent)
    : QObject(parent)
{
}

SystemdPlugin::~SystemdPlugin()
{
    stopPlugin();
}

QString SystemdPlugin::name() const
{
    return QString(PLUGIN_NAME).replace("\"", "");
}

QString SystemdPlugin::description() const
{
    return QString(PLUGIN_DESCRIPTION).replace("\"", "") + QString(" ") + QString(PLUGIN_DOMAIN).replace("\"", "");
}
QUrl SystemdPlugin::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}
QVersionNumber SystemdPlugin::version() const
{
    QString version = QString(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool SystemdPlugin::checkCompatibility()
{
    // Add custom system checks here if needed
    return true;
}

bool SystemdPlugin::startPlugin()
{
    if(m_watcher)
        stopPlugin();
    m_watcher = new SystemDWatcher(this);
    qCInfo(plugin_logger) << name() << "plugin started successfully";
    return true;
}

bool SystemdPlugin::stopPlugin()
{
    if(m_watcher)
    {
        m_watcher->deleteLater();
        m_watcher = nullptr;
    }
    qCInfo(plugin_logger) << name() << "plugin stopped";
    return true;
}

#include "systemdplugin.moc"