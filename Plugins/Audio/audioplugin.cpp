// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file template.cpp
 * @brief Implementation of the KIOT plugin template.
 */

#include "audioplugin.h"

#include <QCoreApplication>

// Add the entities you need from the shared lib like this:
// using KIOTShared::Entities::Sensor;

using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(plugin_logger_audio, Audio)


AudioPlugin::AudioPlugin(QObject *parent)
    : QObject(parent)
{
}

QString AudioPlugin::name() const
{
    return QString(PLUGIN_NAME).replace("\"", "");
}

QString AudioPlugin::description() const
{
    return QString(PLUGIN_DESCRIPTION).replace("\"", "") + QString(" ") + QString(PLUGIN_DOMAIN).replace("\"", "");
}
QUrl AudioPlugin::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}
QVersionNumber AudioPlugin::version() const
{
    QString version = QString(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool AudioPlugin::checkCompatibility()
{
    return true;
}

bool AudioPlugin::startPlugin()
{
    m_audio = new Audio(this);

    qCInfo(plugin_logger_audio) << name() << " plugin started successfully";
    return true;
}

bool AudioPlugin::stopPlugin()
{
    if(m_audio)
    {
        m_audio->deleteLater();
        m_audio = nullptr;
    }
    qCInfo(plugin_logger_audio) << name() << " plugin stopped";
    return true;
}

#include "audioplugin.moc"