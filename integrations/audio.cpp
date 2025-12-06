// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "core.h"
#include "entities/entities.h"

#include <PulseAudioQt/Context>
#include <PulseAudioQt/Server>
#include <PulseAudioQt/Sink>
#include <PulseAudioQt/VolumeObject>
#include <QDebug>

class Audio : public QObject
{
    Q_OBJECT

public:
    explicit Audio(QObject *parent = nullptr);

private slots:
    void updateSinks(PulseAudioQt::Sink *sink);
    void onSinkSelected(QString newOption);
    void onVolumeChanged();
    void setVolume(int v);

private:
    int paToPercent(qint64 v) const;
    qint64 percentToPa(int percent) const;

    Number *m_sinkVolume = nullptr;
    Select *m_sinSelector = nullptr;
    PulseAudioQt::Sink *m_sink = nullptr;
    PulseAudioQt::Context *m_ctx = nullptr;
};

// Constructor
Audio::Audio(QObject *parent)
    : QObject(parent)
{
    m_sinkVolume = new Number(this);
    m_sinkVolume->setId("volume");
    m_sinkVolume->setName("System Volume");
    m_sinkVolume->setHaIcon("mdi:knob");
    m_sinkVolume->setRange(0, 100, 1, "%");

    connect(m_sinkVolume, &Number::valueChangeRequested,
            this, &Audio::setVolume);

    m_ctx = PulseAudioQt::Context::instance();
    if (!m_ctx || !m_ctx->isValid()) {
        qWarning() << "Audio: PulseAudio context not valid";
        return;
    }

    auto *server = m_ctx->server();
    if (!server) {
        qWarning() << "Audio: No PulseAudio server";
        return;
    }

    // Opprett Select tidlig og koble signalet
    m_sinSelector = new Select(this);
    m_sinSelector->setId("volume_output_selector");
    m_sinSelector->setHaIcon("mdi:volume-source");
    m_sinSelector->setName("Output Selector");
    connect(m_sinSelector, &Select::optionSelected,
            this, &Audio::onSinkSelected);

    // Lytt på default sink endringer
    connect(server, &PulseAudioQt::Server::defaultSinkChanged,
            this, &Audio::updateSinks);

    // Hvis default sink allerede finnes, oppdater
    if (auto *initial = server->defaultSink())
        updateSinks(initial);
}

void Audio::updateSinks(PulseAudioQt::Sink *sink)
{
    if (!sink || !sink->isDefault()) return;

    // Fyll options basert på tilgjengelige sinks
    QStringList options;
    for (auto s : m_ctx->sinks())
        options.append(s->description());

    m_sinSelector->setOptions(options);

    // Sett initial state
    m_sinSelector->setState(sink->description());

    // Lagre aktiv sink
    if (m_sink)
        disconnect(m_sink, nullptr, this, nullptr);

    m_sink = sink;

    // Lytt på volumendringer
    connect(m_sink, &PulseAudioQt::VolumeObject::volumeChanged,
            this, &Audio::onVolumeChanged);

    onVolumeChanged();
}

void Audio::onSinkSelected(QString newOption)
{
    qDebug() << "Sink selected:" << newOption;

    if (!m_ctx) return;

    for (PulseAudioQt::Sink *sink : m_ctx->sinks()) {
        if (sink->description() == newOption) {
            qDebug() << "Setting sink to" << sink->description();
            sink->setDefault(true);
            break;
        }
    }
}

void Audio::onVolumeChanged()
{
    if (!m_sink) return;

    int percent = paToPercent(m_sink->volume());
    if (percent == m_sinkVolume->getValue()) return;

    m_sinkVolume->setValue(percent);
    qDebug() << "Audio: Updated volume from system:" << percent << "%";
}

void Audio::setVolume(int v)
{
    if (!m_sink) return;
    if (v == m_sinkVolume->getValue()) return;

    qint64 paVol = percentToPa(v);
    m_sink->setVolume(paVol);
    qDebug() << "Audio: Set volume to" << v << "%";
}

int Audio::paToPercent(qint64 v) const
{
    double p = (double)v / PulseAudioQt::normalVolume() * 100.0;
    return qRound(p);
}

qint64 Audio::percentToPa(int percent) const
{
    return qRound(PulseAudioQt::normalVolume() * (percent / 100.0));
}

// Setup integrasjon
void setupAudio()
{
    new Audio(qApp);
}

REGISTER_INTEGRATION("Audio", setupAudio, true)
#include "audio.moc"
