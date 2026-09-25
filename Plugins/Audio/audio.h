// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once

#include <QObject>
#include <QFileSystemWatcher>
#include <QFile>
#include <QTimer>
#include <QDir>

#include <PulseAudioQt/Context>
#include <PulseAudioQt/SinkInput>
#include <PulseAudioQt/Server>
#include <PulseAudioQt/Sink>
#include <PulseAudioQt/Source>
#include <PulseAudioQt/VolumeObject>
#include <KIOTShared/kiotshared.h>

using KIOTShared::Entities::Select;
using KIOTShared::Entities::Number;
class Audio : public QObject
{
    Q_OBJECT

public:
    explicit Audio(QObject *parent = nullptr);

private Q_SLOTS:
    void updateSinks();
    void updateSources();
    void updateSinkInputs();
    void onSinkSelected(const QString &newOption);
    void onSinkInputSelected(const QString &newOption);
    void onSourceSelected(const QString &newOption);
    void onSourceVolumeChanged();
    void onSinkVolumeChanged();
    void onSinkInputVolumeChanged();
    void setSinkVolume(int v);
    void setSourceVolume(int v);
    void setSinkInputVolume(int v);
    

private:
    bool checkIfRaiseMaxVolumeEnabled();
    int paToPercent(qint64 v) const;
    qint64 percentToPa(int percent) const;

    QFileSystemWatcher *watcher = nullptr;
    Number *m_sinkVolume = nullptr;
    Number *m_sinkInputVolume = nullptr;
    Number *m_sourceVolume = nullptr;
    Select *m_sinkSelector = nullptr;
    Select *m_sourceSelector = nullptr;
    Select *m_sinkInputSelector = nullptr;

    PulseAudioQt::SinkInput *m_sinkInput = nullptr;
    PulseAudioQt::Sink *m_sink = nullptr;
    PulseAudioQt::Source *m_source = nullptr;
    PulseAudioQt::Context *m_ctx = nullptr;
};
