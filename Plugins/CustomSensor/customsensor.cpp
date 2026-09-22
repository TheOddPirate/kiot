// SPDX-FileCopyrightText: 2026 Kloud <dgudim@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later
#include "customsensor.h"

#include <QTimer>
#include <QHash>
#include <QStringView>
#include <KProcess>
#include <KSandbox>
#include <KIOTShared/kiotshared.h>
using KIOTShared::PlatformHelper;
DEFINE_PLUGIN_LOGGER(customSensors,CustomSensors)

constexpr qint64 MinimumIntervalMs = 1000;

static const QHash<QString, double> unitToMs = {
    {QStringLiteral("s"), 1000.0},
    {QStringLiteral("sec"), 1000.0},
    {QStringLiteral("second"), 1000.0},
    {QStringLiteral("seconds"), 1000.0},
    {QStringLiteral("m"), 60000.0},
    {QStringLiteral("min"), 60000.0},
    {QStringLiteral("minute"), 60000.0},
    {QStringLiteral("minutes"), 60000.0},
    {QStringLiteral("h"), 3600000.0},
    {QStringLiteral("hr"), 3600000.0},
    {QStringLiteral("hour"), 3600000.0},
    {QStringLiteral("hours"), 3600000.0},
    {QStringLiteral("d"), 86400000.0},
    {QStringLiteral("day"), 86400000.0},
    {QStringLiteral("days"), 86400000.0},
};

qint64 parseTimeSpanToMs(const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        return -1;
    }

    bool parsedAsBareNumber = false;
    const double bareSeconds = trimmed.toDouble(&parsedAsBareNumber);
    if (parsedAsBareNumber) {
        return (qint64)(bareSeconds * 1000.0);
    }

    double totalMs = 0.0;
    for (const QString &token : trimmed.simplified().split(u' ', Qt::SkipEmptyParts)) {
        const auto unitStart = std::find_if(token.cbegin(), token.cend(), [](QChar c) {
            return !c.isDigit() && c != u'.';
        });
        const auto splitAt = unitStart - token.cbegin();
        if (splitAt == 0 || splitAt == token.size()) {
            return -1;
        }

        bool numberParsedSuccessfully = false;
        const double value = QStringView(token).first(splitAt).toDouble(&numberParsedSuccessfully);
        const auto unitMsMultiplier = unitToMs.constFind(token.sliced(splitAt));
        if (!numberParsedSuccessfully || unitMsMultiplier == unitToMs.constEnd()) {
            return -1;
        }
        totalMs += value * unitMsMultiplier.value();
    }

    return (qint64)totalMs;
}

CustomSensor::CustomSensor(const QString &id, const QString &name, const QString &command, qint64 intervalMs, QObject *parent)
    : QObject(parent)
    , m_command(command)
{
    m_sensor = new Sensor(this);
    m_sensor->setId(id);
    m_sensor->setName(name);

    m_timer = new QTimer(this);
    m_timer->setInterval(std::max(intervalMs, MinimumIntervalMs));
    connect(m_timer, &QTimer::timeout, this, &CustomSensor::poll);
    m_timer->start();

    poll();
}

Sensor *CustomSensor::sensor() const
{
    return m_sensor;
}

void CustomSensor::poll()
{
    if (m_process) {
        return;
    }

    m_process = new KProcess(this);
    m_process->setOutputChannelMode(KProcess::OnlyStdoutChannel);
    m_process->setShellCommand(m_command);

    if (KSandbox::isFlatpak()) {
        KSandbox::ProcessContext ctx = KSandbox::makeHostContext(*m_process);
        m_process->setProgram(ctx.program);
        m_process->setArguments(ctx.arguments);
    }

    connect(m_process, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus != QProcess::NormalExit || exitCode != 0) {
            qCWarning(customSensors) << "Command failed with exit code" << exitCode;
        } else {
            const QString output = QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();
            m_sensor->setState(output);
        }
        m_process->deleteLater();
        m_process = nullptr;
    });

    m_process->start();
}