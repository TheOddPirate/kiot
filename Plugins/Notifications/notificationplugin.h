// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>
using KIOTShared::Entities::Notify;
using KIOTShared::Plugins::KIOTPluginInterface;
class NotificationPlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    NotificationPlugin(QObject *parent = nullptr);
    ~NotificationPlugin() override = default;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    bool enabledByDefault() override;
    bool startPlugin() override;
    bool stopPlugin() override;
private slots:
    void notificationCallback(QByteArray message);
    
private:
    Notify *m_notify = nullptr;
};