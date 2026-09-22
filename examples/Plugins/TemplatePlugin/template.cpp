// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file template.cpp
 * @brief Implementation of the KIOT plugin template.
 */

#include "template.h"

#include <QCoreApplication>

// Add the entities you need from the shared lib like this:
// using KIOTShared::Entities::Sensor;

using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(plugin_logger, PLUGIN_NAME)

TemplatePlugin::TemplatePlugin(QObject *parent)
    : QObject(parent)
{
}

TemplatePlugin::~TemplatePlugin()
{
    stopPlugin();
}

QString TemplatePlugin::name() const
{
    return QString(PLUGIN_NAME).replace("\"", "");
}

QString TemplatePlugin::description() const
{
    return QString(PLUGIN_DESCRIPTION).replace("\"", "") + QString(" ") + QString(PLUGIN_DOMAIN).replace("\"", "");
}

QUrl TemplatePlugin::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}

QVersionNumber TemplatePlugin::version() const
{
    QString version = QString(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool TemplatePlugin::checkCompatibility()
{
    // Add custom system checks here if needed
    return true;
}

bool TemplatePlugin::enabledByDefault()
{
    if(!checkCompatibility())
        return false;

    return true;
}

bool TemplatePlugin::startPlugin()
{
    qCInfo(plugin_logger) << name() << "plugin started successfully";
    return true;
}

bool TemplatePlugin::stopPlugin()
{
    qCInfo(plugin_logger) << name() << "plugin stopped";
    return true;
}

#include "template.moc"