// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "flatpakhelper.h"
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QStandardPaths>

namespace FlatpakHelper {
    
    bool isFlatpak()
    {
        // Method 1: Check environment variable (most reliable)
        static bool checked = false;
        static bool flatpak = false;
        
        if (!checked) {
            // Check multiple indicators
            if (!qEnvironmentVariableIsEmpty("container")) {
                flatpak = true;
            }
            // Check for Flatpak specific directories/files
            else if (QFile::exists("/app") || QFile::exists("/.flatpak-info")) {
                flatpak = true;
            }
            // Check for Flatpak runtime in /run/host
            else if (QFile::exists("/run/host")) {
                flatpak = true;
            }
            
            checked = true;
            
            if (flatpak) {
                qDebug() << "FlatpakHelper: Running in Flatpak environment";
            }
        }
        
        return flatpak;
    }
    
    QString adaptCommand(const QString &command)
    {
        if (isFlatpak()) {
            // Check if command already has flatpak-spawn prefix
            if (!command.startsWith("flatpak-spawn")) {
                return "flatpak-spawn --host " + command + " &";
            }
        }
        return command;
    }
    
    QPair<int, QString> executeCommand(const QString &command,
                                       const QStringList &arguments,
                                       int timeout)
    {
        QString fullCommand = command;
        QStringList fullArgs = arguments;
        
        if (isFlatpak() && !command.startsWith("flatpak-spawn")) {
            // For Flatpak, we need to use flatpak-spawn
            fullCommand = "flatpak-spawn";
            fullArgs.prepend("--host");
            fullArgs.prepend(command);
        }
        
        QProcess process;
        process.setProgram(fullCommand);
        process.setArguments(fullArgs);
        
        process.start();
        
        if (!process.waitForFinished(timeout)) {
            qWarning() << "FlatpakHelper: Command timed out:" << command << arguments;
            process.kill();
            return qMakePair(-1, QString("Command timed out"));
        }
        
        int exitCode = process.exitCode();
        QString output = QString::fromUtf8(process.readAllStandardOutput());
        
        if (exitCode != 0) {
            QString error = QString::fromUtf8(process.readAllStandardError());
            qWarning() << "FlatpakHelper: Command failed:" << command << arguments
                       << "Exit code:" << exitCode << "Error:" << error;
        }
        
        return qMakePair(exitCode, output);
    }
    
    bool commandAvailable(const QString &command)
    {
        if (isFlatpak()) {
            // In Flatpak, check if command is available via flatpak-spawn
            auto result = executeCommand("which", {command}, 5000);
            return result.first == 0 && !result.second.trimmed().isEmpty();
        } else {
            // Outside Flatpak, use standard which
            QProcess process;
            process.start("which", {command});
            process.waitForFinished(1000);
            return process.exitCode() == 0;
        }
    }
    
    QString getSystemBinary(const QString &binaryName)
    {
        if (isFlatpak()) {
            // In Flatpak, we need to use flatpak-spawn
            if(binaryName.startsWith("/"))
                return  QString("flatpak-spawn --host %1").arg(binaryName);
            else
                return QString("flatpak-spawn --host /usr/bin/%1").arg(binaryName);
        } else {
            // Outside Flatpak, find the binary
            QProcess process;
            process.start("which", {binaryName});
            process.waitForFinished(1000);
            
            if (process.exitCode() == 0) {
                return QString::fromUtf8(process.readAllStandardOutput()).trimmed();
            }
            
            // Try whereis as fallback
            process.start("whereis", {"-b", binaryName});
            process.waitForFinished(1000);
            
            if (process.exitCode() == 0) {
                QString output = QString::fromUtf8(process.readAllStandardOutput());
                QStringList paths = output.split(" ");
                if (paths.size() > 1) {
                    return paths[1]; // First path after binary name
                }
            }
        }
        
        return QString();
    }
    
    QVariant dbusCall(const QString &service,
                      const QString &path,
                      const QString &interface,
                      const QString &method,
                      const QList<QVariant> &args)
    {
        QDBusConnection connection = QDBusConnection::sessionBus();
        
        if (!connection.isConnected()) {
            qWarning() << "FlatpakHelper: D-Bus connection not available";
            return QVariant();
        }
        
        QDBusInterface dbusInterface(service, path, interface, connection);
        
        if (!dbusInterface.isValid()) {
            qWarning() << "FlatpakHelper: D-Bus interface not valid for"
                       << service << path << interface;
            return QVariant();
        }
        
        QDBusMessage reply = dbusInterface.callWithArgumentList(QDBus::Block, method, args);
        
        if (reply.type() == QDBusMessage::ErrorMessage) {
            qWarning() << "FlatpakHelper: D-Bus call failed:" << reply.errorMessage()
                       << "Service:" << service << "Method:" << method;
            return QVariant();
        }
        
        // Return first argument if any
        if (!reply.arguments().isEmpty()) {
            return reply.arguments().first();
        }
        
        return QVariant();
    }
    
    bool hasDBusAccess(const QString &service)
    {
        // Try to create a simple interface to check access
        QDBusConnection connection = QDBusConnection::sessionBus();
        
        if (!connection.isConnected()) {
            return false;
        }
        
        QDBusInterface dbusInterface(service, "/", "org.freedesktop.DBus.Peer", connection);
        
        // If we can create the interface and it's valid, we likely have access
        return dbusInterface.isValid();
    }
    
    QString getFlatpakAppId()
    {
        if (!isFlatpak()) {
            return QString();
        }
        
        // Read from /.flatpak-info
        QFile flatpakInfo("/.flatpak-info");
        if (flatpakInfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&flatpakInfo);
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.startsWith("name=")) {
                    return line.mid(5); // Remove "name="
                }
            }
        }
        
        return QString();
    }
    
    QString getFlatpakRuntime()
    {
        if (!isFlatpak()) {
            return QString();
        }
        
        // Read from /.flatpak-info
        QFile flatpakInfo("/.flatpak-info");
        if (flatpakInfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&flatpakInfo);
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.startsWith("runtime=")) {
                    return line.mid(8); // Remove "runtime="
                }
            }
        }
        
        return QString();
    }
    
    bool hasFlatpakPermission(const QString &permission)
    {
        if (!isFlatpak()) {
            return true; // Outside Flatpak, assume all permissions
        }
        
        // Check common permissions by testing access
        if (permission.contains("filesystem")) {
            // Extract path from permission string
            QString path = permission.section('=', 1);
            if (path.isEmpty()) {
                return false;
            }
            
            // Check if we can access the path
            return QFile::exists(path);
        }
        
        // For other permissions, we'd need to parse flatpak-info
        // This is a simplified implementation
        QFile flatpakInfo("/.flatpak-info");
        if (flatpakInfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&flatpakInfo);
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.contains(permission)) {
                    return true;
                }
            }
        }
        
        return false;
    }
}