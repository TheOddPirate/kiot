// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later


#pragma once

#include <KIOTShared/kiotshared.h>
#include <QCoreApplication>
#include <QSocketNotifier>
#include <QTimer>
#include <libudev.h>
#include <unistd.h>

using KIOTShared::Entities::BinarySensor;

using KIOTShared::PlatformHelper;

class Gamepad : public QObject
{
    Q_OBJECT
public:
    explicit Gamepad(QObject *parent = nullptr);
    ~Gamepad();

private Q_SLOTS:
    void udevEvent();

private:
    void updateState();
    BinarySensor *m_sensor;
    struct udev *m_udev = nullptr;
    struct udev_monitor *m_monitor = nullptr;
    QSocketNotifier *m_notifier = nullptr;
};