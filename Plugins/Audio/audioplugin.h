// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVersionNumber>
#include <KIOTShared/kiotshared.h>
#include "audio.h"

/**
 * @file audioplugin.h
 * @brief Template definition for new KIOT runtime plugins.
 */

/**
 * @class AudioPlugin
 * @brief A boilerplate template class for developing new KIOT plugins.
 *
 * @details Implements the KIOTPluginInterface and handles standard lifecycle methods,
 *          metadata parsing from CMake/JSON, and resource cleanup. Copy and adapt this
 *          structure when creating new integrations.
 */


using KIOTShared::Plugins::KIOTPluginInterface;
class AudioPlugin : public QObject, public KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    AudioPlugin(QObject *parent = nullptr);
    ~AudioPlugin() override = default;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    
    bool startPlugin() override;
    bool stopPlugin() override;

private:
    Audio *m_audio = nullptr;
};