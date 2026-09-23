// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVersionNumber>
#include <KIOTShared/kiotshared.h>
#include "virtualkeyboarddevice.h"
using KIOTShared::Plugins::KIOTPluginInterface;
using KIOTShared::Entities::Notify;
/**
 * @file macroplugin.h
 * @brief Template definition for new KIOT runtime plugins.
 */

/**
 * @class MacroPlugin
 * @brief A boilerplate macroplugin class for developing new KIOT plugins.
 *
 * @details Implements the KIOTPluginInterface and handles standard lifecycle methods,
 *          metadata parsing from CMake/JSON, and resource cleanup. Copy and adapt this
 *          structure when creating new integrations.
 */
class MacroPlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    /**
     * @brief Constructs a MacroPlugin instance.
     * @param parent Optional parent QObject.
     */
    explicit MacroPlugin(QObject *parent = nullptr);

    /**
     * @brief Destroys the MacroPlugin instance and ensures safe resource cleanup.
     */
    ~MacroPlugin() override;

    /**
     * @brief Gets the name of the plugin from project metadata.
     * @return QString Plugin name.
     */
    QString name() const override;

    /**
     * @brief Gets the description of the plugin.
     * @return QString Plugin description.
     */
    QString description() const override;

    /**
     * @brief Gets the URL or project home for the plugin.
     * @return QUrl Plugin URL.
     */
    QUrl url() const override;

    /**
     * @brief Gets the version number of the plugin.
     * @return QVersionNumber Version object.
     */
    QVersionNumber version() const override;

    /**
     * @brief Checks if the host system satisfies requirements for this plugin.
     * @return bool True if compatible, false otherwise.
     */
    bool checkCompatibility() override;
    
    /**
     * @brief Checks if the plugin should be enabled by default, mostly for writing config first load
     * @return bool True if it should, false otherwise.
     */
    bool enabledByDefault() override;
    /**
     * @brief Starts the plugin functionality, sets up entities and listeners.
     * @return bool True if successfully started.
     */
    bool startPlugin() override;

    /**
     * @brief Stops the plugin functionality and tears down allocated entities.
     * @return bool True if successfully stopped.
     */
    bool stopPlugin() override;

private slots:
    void notificationCallback(QByteArray message);
private:
    bool validateSequence(const QString &sequenceStr, QString &errorMessage);
    void executeSequence(const QString &sequenceStr);
    /**
     * @brief The virtual keyboard device used to execute macros.
     */
    VirtualKeyboardDevice *m_device = nullptr;
    Notify *m_notify = nullptr;
};