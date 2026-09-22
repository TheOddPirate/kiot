// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "bluetoothplugin.h"

#include <QCoreApplication>

using KIOTShared::Entities::BinarySensor;
using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(plugin_logger,Bluetooth)

BluetoothPlugin::BluetoothPlugin(QObject *parent)
    : QObject(parent)
{
}

QString BluetoothPlugin::name() const
{
    return QString(PLUGIN_NAME).replace("\"", "");
}

QString BluetoothPlugin::description() const
{
    return QString(PLUGIN_DESCRIPTION).replace("\"", "") + QString(" ") + QString(PLUGIN_DOMAIN).replace("\"", "");
}
QUrl BluetoothPlugin::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}
QVersionNumber BluetoothPlugin::version() const
{
    QString version = QString(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool BluetoothPlugin::checkCompatibility()
{
    return true;
}

bool BluetoothPlugin::startPlugin()
{
    if(m_adapterWatcher)
        stopPlugin();

    m_adapterWatcher = new BluetoothAdapterWatcher(this);
    qCInfo(plugin_logger) << name() << " plugin started successfully";
    return true;
}

bool BluetoothPlugin::stopPlugin()
{
    if(m_adapterWatcher)
    {
        m_adapterWatcher->deleteLater();
        m_adapterWatcher = nullptr;
    }
    qCInfo(plugin_logger) << name() << " plugin stopped";
    return true;
}

#include "bluetoothplugin.moc"