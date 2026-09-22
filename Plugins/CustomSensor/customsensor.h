// SPDX-FileCopyrightText: 2026 Kloud <dgudim@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once

#include <QObject>
#include <QString>
#include <KProcess>
#include <QTimer>

#include <KIOTShared/kiotshared.h>
using KIOTShared::Entities::Sensor;


// Hjelpefunksjon for å parse tidsintervaller
qint64 parseTimeSpanToMs(const QString &text);

class CustomSensor : public QObject
{
    Q_OBJECT
public:
    CustomSensor(const QString &id, const QString &name, const QString &command, qint64 intervalMs, QObject *parent = nullptr);
    ~CustomSensor() override = default;

    Sensor *sensor() const;

private slots:
    void poll();

private:
    Sensor *m_sensor = nullptr;
    QString m_command;
    QTimer *m_timer = nullptr;
    KProcess *m_process = nullptr;
};