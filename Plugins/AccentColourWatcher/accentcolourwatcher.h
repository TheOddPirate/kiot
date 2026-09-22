// SPDX-FileCopyrightText: 2025-2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <KConfigWatcher>

namespace KIOTShared::Entities {
class Sensor;
}

/**
 * @class AccentColourWatcher
 * @brief Core logic for monitoring and exposing KDE system accent colors in KIOT.
 */
class AccentColourWatcher : public QObject
{
    Q_OBJECT

public:
    explicit AccentColourWatcher(QObject *parent = nullptr);
    ~AccentColourWatcher() override = default;

private:
    void updateAccentColor(const KConfigGroup &config);
    QString rgbToHex(const QString &rgb);
    void setRgbAttributes(QVariantMap &attributes, const QString &rgb, const QString &prefix);

    KIOTShared::Entities::Sensor *m_sensor;
    KConfigWatcher::Ptr m_watcher;
};