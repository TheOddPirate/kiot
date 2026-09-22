// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "active.h"
#include <KIdleTime>
#include <QCoreApplication>

using KIOTShared::Entities::BinarySensor;
using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(plugin_logger,ActivePlugin)

ActivePlugin::ActivePlugin(QObject *parent)
    : QObject(parent)
{
}

QString ActivePlugin::name() const
{
    return QStringLiteral(PLUGIN_NAME).replace("\"", "");
}

QString ActivePlugin::description() const
{
    return QStringLiteral(PLUGIN_DESCRIPTION).replace("\"", "") + QStringLiteral(" ").replace("\"", "") + QStringLiteral(PLUGIN_DOMAIN).replace("\"", "");
}
QUrl ActivePlugin::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}
QVersionNumber ActivePlugin::version() const
{
    QString version = QStringLiteral(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool ActivePlugin::checkCompatibility()
{
    return true;
}

bool ActivePlugin::startPlugin()
{
    auto sensor = new BinarySensor(this);
    sensor->setId("active");
    sensor->setName("Active");
    sensor->setDiscoveryConfig("device_class", "presence");

    // Idle-logikk fra din originale kode
    auto kidletime = KIdleTime::instance();
    auto id = kidletime->addIdleTimeout(60 * 1000);
    
    QObject::connect(kidletime, &KIdleTime::resumingFromIdle, this, [sensor]() {
        sensor->setState(true);
    });
    
    QObject::connect(kidletime, &KIdleTime::timeoutReached, this, [id, kidletime, sensor](int _id) {
        if (_id != id) {
            return;
        }
        sensor->setState(false);
        kidletime->catchNextResumeEvent();
    });
    
    sensor->setState(true);
    qCInfo(plugin_logger) << name() << " plugin started successfully";
    return true;
}

bool ActivePlugin::stopPlugin()
{
    qCInfo(plugin_logger) << name() << " plugin stopped";
    return true;
}

#include "active.moc"