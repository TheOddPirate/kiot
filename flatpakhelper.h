// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file flatpakhelper.h
 * @brief Helper utilities for Flatpak environment detection and adaptation
 * 
 * @details
 * This header provides utilities to detect if the application is running
 * inside a Flatpak sandbox and adapt system commands accordingly.
 * 
 * When running in Flatpak, many system commands need to be prefixed with
 * 'flatpak-spawn --host' to execute them on the host system.
 * 
 * Usage example:
 * @code
 * if (FlatpakHelper::isFlatpak()) {
 *     QString command = FlatpakHelper::adaptCommand("systemctl --user status");
 *     // command will be "flatpak-spawn --host systemctl --user status"
 * }
 * @endcode
 */

#pragma once

#include <QString>
#include <QProcess>
#include <QVariant>

/**
 * @namespace FlatpakHelper
 * @brief Helper utilities for Flatpak environment
 */
namespace FlatpakHelper {
    
    /**
     * @brief Check if the application is running inside a Flatpak sandbox
     * @return bool True if running in Flatpak, false otherwise
     * 
     * @details
     * Detects Flatpak environment by checking:
     * 1. The 'container' environment variable (set by Flatpak)
     * 2. Existence of '/app' directory (Flatpak mount point)
     * 3. Existence of '/.flatpak-info' file (Flatpak metadata)
     */
    bool isFlatpak();
    
    /**
     * @brief Adapt a system command for Flatpak environment
     * @param command The original system command
     * @return QString Command adapted for Flatpak (prefixed if needed)
     * 
     * @details
     * If running in Flatpak, prefixes the command with 'flatpak-spawn --host'
     * to execute it on the host system. Otherwise returns the command unchanged.
     */
    QString adaptCommand(const QString &command);
    
    /**
     * @brief Execute a system command with Flatpak adaptation
     * @param command The command to execute
     * @param arguments Command arguments
     * @param timeout Timeout in milliseconds (default: 30000)
     * @return QPair<int, QString> Exit code and standard output
     * 
     * @details
     * Executes a system command with proper Flatpak adaptation.
     * Automatically handles 'flatpak-spawn --host' prefix when needed.
     */
    QPair<int, QString> executeCommand(const QString &command, 
                                       const QStringList &arguments = QStringList(),
                                       int timeout = 30000);
    
    /**
     * @brief Check if a command is available in the current environment
     * @param command Command to check
     * @return bool True if command is available
     * 
     * @details
     * Checks if a command exists and is executable, taking Flatpak
     * environment into account.
     */
    bool commandAvailable(const QString &command);
    
    /**
     * @brief Get the path to a system binary, adapting for Flatpak
     * @param binaryName Name of the binary (e.g., "systemctl")
     * @return QString Full path to binary or empty if not found
     * 
     * @details
     * In Flatpak, returns 'flatpak-spawn --host /usr/bin/{binary}'
     * Outside Flatpak, returns standard which/whereis result
     */
    QString getSystemBinary(const QString &binaryName);
    
    /**
     * @brief Run a D-Bus command with Flatpak adaptation
     * @param service D-Bus service name
     * @param path D-Bus object path
     * @param interface D-Bus interface
     * @param method D-Bus method to call
     * @param args Method arguments
     * @return QVariant Method return value or invalid QVariant on error
     * 
     * @details
     * Provides a unified way to call D-Bus methods that works both
     * inside and outside Flatpak.
     */
    QVariant dbusCall(const QString &service,
                      const QString &path,
                      const QString &interface,
                      const QString &method,
                      const QList<QVariant> &args = QList<QVariant>());
    
    /**
     * @brief Check if we have D-Bus access to a specific service
     * @param service D-Bus service name
     * @return bool True if we can access the service
     */
    bool hasDBusAccess(const QString &service);
    
    /**
     * @brief Get Flatpak app ID if running in Flatpak
     * @return QString Flatpak application ID or empty string
     */
    QString getFlatpakAppId();
    
    /**
     * @brief Get Flatpak runtime information
     * @return QString Runtime name and version or empty string
     */
    QString getFlatpakRuntime();
    
    /**
     * @brief Check if we have specific Flatpak permission
     * @param permission Permission to check (e.g., "--filesystem=home")
     * @return bool True if permission is granted
     */
    bool hasFlatpakPermission(const QString &permission);
}