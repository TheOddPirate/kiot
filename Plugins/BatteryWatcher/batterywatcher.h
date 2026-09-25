// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>
#include <Solid/Battery>
#include <Solid/Device>
#include <Solid/DeviceInterface>
#include <Solid/DeviceNotifier>
using KIOTShared::Entities::Sensor;
class BatteryWatcher : public QObject
{
    Q_OBJECT
public:
    explicit BatteryWatcher(QObject *parent = nullptr);

private Q_SLOTS:
    void deviceAdded(const QString &udi);
    void deviceRemoved(const QString &udi);

private:
    void ensureConfig();
    void setupSolidWatching();
    void registerBattery(const QString &udi);
    void updateBatteryAttributes(const QString &udi);
    QHash<QString, Sensor *> m_udiToSensor;
    bool m_autoRemove = false;
};