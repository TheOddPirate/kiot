// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "mprisplugin.h"
#include <QCoreApplication>

using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(plugin_logger,Mpris)

MprisPlugin::MprisPlugin(QObject *parent)
    : QObject(parent)
{
}

QString MprisPlugin::name() const
{
    return QString(PLUGIN_NAME).replace("\"", "");
}

QString MprisPlugin::description() const
{
    return QString(PLUGIN_DESCRIPTION).replace("\"", "") + QString(" ") + QString(PLUGIN_DOMAIN).replace("\"", "");
}
QUrl MprisPlugin::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}
QVersionNumber MprisPlugin::version() const
{
    QString version = QString(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool MprisPlugin::checkCompatibility()
{
    return true;
}

bool MprisPlugin::startPlugin()
{
    if(m_multiplexer)
        stopPlugin();
    m_multiplexer = new MprisMultiplexer(this);

    qCInfo(plugin_logger) << name() << " plugin started successfully";
    return true;
}

bool MprisPlugin::stopPlugin()
{
    if(m_multiplexer)
    {
        m_multiplexer->deleteLater();
        m_multiplexer = nullptr;
    }
    qCInfo(plugin_logger) << name() << " plugin stopped";
    return true;
}

#include "mprisplugin.moc"