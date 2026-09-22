// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include "dockerswitch.h"
#include <KIOTShared/kiotshared.h>

using KIOTShared::Plugins::KIOTPluginInterface;
using KIOTShared::Entities::BinarySensor;

/**
 * @file dockerplugin.h
 * @brief Dynamic plugin wrapper for the Docker integration.
 */

/**
 * @class DockerPlugin
 * @brief Main entry point for the Docker integration plugin.
 *
 * @details Implements the KIOTPluginInterface to allow runtime loading, unloading,
 *          compatibility checks, and lifecycle management of the Docker monitoring system.
 */
class DockerPlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    /**
     * @brief Constructs a DockerPlugin instance.
     * @param parent Optional parent QObject.
     */
    explicit DockerPlugin(QObject *parent = nullptr);

    /**
     * @brief Destroys the DockerPlugin instance, ensuring all resources are stopped and cleaned up.
     */
    ~DockerPlugin() override;

    /**
     * @brief Gets the name of the plugin.
     * @return QString Plugin name.
     */
    QString name() const override;

    /**
     * @brief Gets a description of the plugin's functionality.
     * @return QString Plugin description.
     */
    QString description() const override;

    /**
     * @brief Gets the version number of the plugin.
     * @return QVersionNumber Version object.
     */
    QVersionNumber version() const override;

    /**
     * @brief Gets the associated URL or project link for the plugin.
     * @return QUrl Plugin URL.
     */
    QUrl url() const override;

    /**
     * @brief Checks if the host system is compatible and meets requirements (e.g. user groups, socket access).
     * @return bool True if compatible, false otherwise.
     */
    bool checkCompatibility() override;

    /**
     * @brief Starts the plugin, initializing the Docker switch manager and event loops.
     * @return bool True if successfully started.
     */
    bool startPlugin() override;

    /**
     * @brief Stops the plugin and frees all allocated monitoring resources.
     * @return bool True if successfully stopped.
     */
    bool stopPlugin() override;

private:
    /**
     * @brief Verifies whether the current user belongs to the local 'docker' system group.
     * @return bool True if member or running in a Flatpak context, false otherwise.
     */
    bool isUserInDockerGroup() const;

    /**
     * @brief Verifies whether the Docker UNIX socket is reachable and active.
     * @return bool True if connection test succeeds.
     */
    bool isDockerAvailable() const;

    DockerSwitch *m_dockerSwitch = nullptr; ///< Pointer to the active Docker container switch manager.
};