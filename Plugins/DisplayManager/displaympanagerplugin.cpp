// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file template.cpp
 * @brief Implementation of the KIOT plugin template.
 */

#include "displaympanagerplugin.h"

#include <QCoreApplication>

// Add the entities you need from the shared lib like this:
// using KIOTShared::Entities::Sensor;

using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(disp_logger , DisplayManager)

DisplayManager::DisplayManager(QObject *parent)
    : QObject(parent)
{
}

DisplayManager::~DisplayManager()
{
    stopPlugin();
}

QString DisplayManager::name() const
{
    return QString(PLUGIN_NAME).replace("\"", "");
}

QString DisplayManager::description() const
{
    return QString(PLUGIN_DESCRIPTION).replace("\"", "") +QStringLiteral(" ") + QString(PLUGIN_DOMAIN).replace("\"", "");
}

QUrl DisplayManager::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}

QVersionNumber DisplayManager::version() const
{
    QString version = QString(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool DisplayManager::checkCompatibility()
{
    auto desktopEnviornment = PlatformHelper::detectDesktopEnvironment();
    if(desktopEnviornment == "kde")
        return true;
    return false;
}

bool DisplayManager::enabledByDefault()
{
    if(!checkCompatibility())
        return false;

    return true;
}


bool DisplayManager::startPlugin()
{
    if (m_screenController) {
        stopPlugin();
    }
    m_screenController = new ScreenController(this);
    qCInfo(disp_logger) << name() << "plugin started successfully";
    return true;
}

bool DisplayManager::stopPlugin()
{
    if (m_screenController) {
        delete m_screenController;
        m_screenController = nullptr;
    }
    
    qCInfo(disp_logger) << name() << "plugin stopped";
    return true;
}

#include "displaympanagerplugin.moc"