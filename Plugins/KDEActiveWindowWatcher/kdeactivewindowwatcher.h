// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

class QDBusInterface;

namespace KIOTShared::Entities {
class Sensor;
}

class KDEActiveWindowWatcher : public QObject
{
    Q_OBJECT

public:
    explicit KDEActiveWindowWatcher(QObject *parent = nullptr);
    ~KDEActiveWindowWatcher() override;

public Q_SLOTS:
    Q_SCRIPTABLE void UpdateAttributes(const QVariantMap &attributes);

private:
    void readScriptResource();
    void tryRegisterDBus();
    void tryInitKWin();
    bool registerKWinScript();
    void cleanup();

    static constexpr int MAX_RETRIES = 5;

    KIOTShared::Entities::Sensor *m_sensor = nullptr;
    QDBusInterface *m_kwinIface = nullptr;
    QString m_scriptPath;
    int m_dbusRetries = 0;
    int m_kwinRetries = 0;
};