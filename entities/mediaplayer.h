// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once

#include "entity.h"
#include <QStringList>
#include <QVariantMap>
#include <QObject>
#include <QMqttMessage>

class MediaPlayerEntity : public Entity
{
    Q_OBJECT
public:
    explicit MediaPlayerEntity(QObject *parent = nullptr);

    void setState(const QVariantMap &info); // Setter alle attributter/state
    QVariantMap state() const;

    void setAvailablePlayers(const QStringList &players);
    QStringList availablePlayers() const;

protected:
    void init() override;
    void publishState();

private slots:
    void onPlayCommand(const QString &payload);
    void onPauseCommand(const QString &payload);
    void onPlayPauseCommand(const QString &payload);
    void onStopCommand(const QString &payload);
    void onNextCommand(const QString &payload);
    void onPreviousCommand(const QString &payload);
    void onSetVolumeCommand(const QString &payload);
    void onPlayMediaCommand(const QString &payload);

public slots:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void setVolume(qreal volume);

signals:
    void stateChanged(QVariantMap newState);
    void playRequested();
    void pauseRequested();
    void stopRequested();
    void nextRequested();
    void previousRequested();
    void volumeChanged(double volume);
    void playMediaRequested(const QString &payload);

private:
    QVariantMap m_state;
    QStringList m_players;
};
