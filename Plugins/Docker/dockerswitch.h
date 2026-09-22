// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QStringList>
#include <QVariantMap>

class DockerEventListener;

namespace KIOTShared::Entities {
class Switch;
}

/**
 * @file dockerswitch.h
 * @brief Manages Docker container switches and Home Assistant synchronization.
 */

/**
 * @class DockerSwitch
 * @brief Controls and monitors Docker containers via the UNIX socket API.
 *
 * @details Discovers available containers, synchronizes them with the KIOT configuration file,
 *          creates Home Assistant switch entities, and handles live state updates and event triggers.
 */
class DockerSwitch : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a DockerSwitch manager and initializes monitored containers.
     * @param parent Optional parent QObject.
     */
    explicit DockerSwitch(QObject *parent = nullptr);

    /**
     * @brief Destroys the DockerSwitch manager and stops active event listeners.
     */
    ~DockerSwitch() override;

private:
    /**
     * @struct ContainerInfo
     * @brief Internal mapping between a container name and its Home Assistant switch entity.
     */
    struct ContainerInfo {
        QString name; ///< Name of the Docker container.
        KIOTShared::Entities::Switch *sw; ///< Pointer to the corresponding Home Assistant switch.
    };

    QList<ContainerInfo> m_containers; ///< List of all actively managed containers.
    DockerEventListener *m_listener = nullptr; ///< Background thread listening for container events.

    /**
     * @brief Sets up Home Assistant switches for containers enabled in the configuration.
     */
    void initializeSwitches();

    /**
     * @brief Creates and registers a switch entity for a specific container.
     * @param name Name of the Docker container.
     */
    void createContainerSwitch(const QString &name);

    /**
     * @brief Starts the background Docker event listener thread.
     */
    void startEventListener();

    /**
     * @brief Gracefully stops and cleans up the Docker event listener thread.
     */
    void stopEventListener();

    /**
     * @brief Synchronizes configuration defaults with currently discovered system containers.
     * @return bool True if the configuration was successfully loaded or updated.
     */
    bool ensureConfigDefaults();

    /**
     * @brief Performs an HTTP request/response cycle over the local Docker UNIX socket.
     * @param request The raw HTTP request bytes to send.
     * @param response Reference to store the full raw HTTP response bytes.
     * @return bool True if the socket communication succeeded.
     */
    bool callDockerSocket(const QByteArray &request, QByteArray &response);

    /**
     * @brief Retrieves a list of all Docker containers available on the host system.
     * @param retries Number of retry attempts if communication fails.
     * @return QStringList List of container names.
     */
    QStringList listAllContainers(int retries = 3);

    /**
     * @brief Checks if a specific container is currently running.
     * @param name Name of the container to check.
     * @return bool True if running, false otherwise.
     */
    bool isRunning(const QString &name);

    /**
     * @brief Extracts the HTTP body from a raw Docker socket response.
     * @param response Full raw HTTP response bytes.
     * @return QByteArray Extracted body content.
     */
    QByteArray extractHttpBody(const QByteArray &response);

    /**
     * @brief Sends a command to start or stop a specific Docker container.
     * @param name Name of the container.
     * @param start True to start the container, false to stop it.
     */
    void toggleContainer(const QString &name, bool start);

    /**
     * @brief Updates a Home Assistant switch's state and attributes with live container data.
     * @param name Container name.
     * @param sw Pointer to the Switch entity to update.
     */
    void updateSwitch(const QString &name, KIOTShared::Entities::Switch *sw);

private slots:
    /**
     * @brief Handles incoming container state change events from the background listener.
     * @param name Name of the container affected by the event.
     * @param attrs Map of event attributes.
     */
    void handleEvent(const QString &name, const QVariantMap &attrs);
};