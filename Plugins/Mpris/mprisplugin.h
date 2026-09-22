// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>
#include "mprismultiplexer.h"
using KIOTShared::Plugins::KIOTPluginInterface;
class MprisPlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    MprisPlugin(QObject *parent = nullptr);
    ~MprisPlugin() override = default;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    
    bool startPlugin() override;
    bool stopPlugin() override;

private:
    MprisMultiplexer *m_multiplexer = nullptr;
};