// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

// SPDX-FileCopyrightText: 1998 Sven Radej <sven@lisa.exp.univie.ac.at>
//      SPDX-FileCopyrightText: 2006 Dirk Mueller <mueller@kde.org>
//          SPDX-FileCopyrightText: 2007 Flavio Castelli <flavio.castelli@gmail.com>
#pragma once


#include <QObject>
#include <QSocketNotifier>
#include <QHash>
#include <QTimer>
#include <QDir>
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <KIOTShared/kiotshared.h>

using KIOTShared::Entities::BinarySensor;
using KIOTShared::PlatformHelper;

class CameraWatcher : public QObject
{
    Q_OBJECT
public:
    CameraWatcher(QObject *parent);
    ~CameraWatcher();

private:
    BinarySensor *m_sensor;
    void onInotifyCallback();
    void onInotifyEvent(const inotify_event *event);
    void onVideoDeviceAdded(const QString &devicePath);
    void onVideoDeviceRemoved(const QString &devicePath);

    int m_inotifyFd = -1;
    QSocketNotifier *m_notifier = nullptr;
    QHash<QString, int> m_watchFds;
    QHash<QString, int> m_deviceOpenCounts;
    QTimer *m_hysterisisDelay = nullptr;

    void updateSensorState();
};
