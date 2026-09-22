// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once

#include <QObject>
#include "lockedstate.h"
#include <KIOTShared/kiotshared.h>

using KIOTShared::Plugins::KIOTPluginInterface;
class LockedStatePlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    LockedStatePlugin(QObject *parent = nullptr);
    ~LockedStatePlugin() override ;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    
    bool startPlugin() override;
    bool stopPlugin() override;

private:
    LockedState *m_lockedState = nullptr;
};