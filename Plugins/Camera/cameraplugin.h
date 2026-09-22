// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

// SPDX-FileCopyrightText: 1998 Sven Radej <sven@lisa.exp.univie.ac.at>
//      SPDX-FileCopyrightText: 2006 Dirk Mueller <mueller@kde.org>
//          SPDX-FileCopyrightText: 2007 Flavio Castelli <flavio.castelli@gmail.com>

#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>
#include "camerawatcher.h"

using KIOTShared::Plugins::KIOTPluginInterface;
class CameraPlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    CameraPlugin(QObject *parent = nullptr);
    ~CameraPlugin() override = default;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    
    bool startPlugin() override;
    bool stopPlugin() override;

private:
    CameraWatcher *m_cameraWatcher = nullptr;
};