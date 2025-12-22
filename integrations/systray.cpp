// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file systray.cpp
 * @brief System Tray integration for Kiot
 * 
 * This integration provides a system tray icon with connection status
 * and quick access to Kiot settings.
 */

#include "core.h"
#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QProcess>
#include <QTimer>
#include <QFile>
#include <QLoggingCategory>
#include <QApplication>
#include <QPainter>

#include <KConfigGroup>
#include <KSharedConfig>
#include <QMqttClient>
Q_DECLARE_LOGGING_CATEGORY(st)
Q_LOGGING_CATEGORY(st, "integration.SystemTray")

/**
 * @class SystemTray
 * @brief System tray integration for Kiot
 * 
 * @details
 * Provides a system tray icon that shows MQTT connection status
 * (green = connected, red = disconnected) and a context menu with
 * quick access to Kiot settings and actions.
 */
class SystemTray : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a SystemTray instance
     * @param parent Parent QObject (optional)
     * 
     * @details
     * Initializes the system tray icon with connection status monitoring
     * and context menu with useful actions.
     */
    explicit SystemTray(QObject *parent = nullptr)
        : QObject(parent)
        , m_trayIcon(new QSystemTrayIcon(this))
        , m_menu(new QMenu())
    {
        // Create icons for different states
        createIcons();
        
        // Set up the tray icon
        updateIcon(false); // Start with disconnected state
        m_trayIcon->setToolTip("Kiot - Disconnected");
        
        // Create context menu
        setupMenu();
        
        // Connect signals
        connect(m_trayIcon, &QSystemTrayIcon::activated, this, &SystemTray::onTrayActivated);
        
        // Monitor MQTT connection state
        auto mqttClient = HaControl::mqttClient();
        if (mqttClient) {
            connect(mqttClient, &QMqttClient::stateChanged, this, &SystemTray::onMqttStateChanged);
            // Initial state
            onMqttStateChanged(mqttClient->state());
        }
        
        // Show the tray icon
        m_trayIcon->show();
        qCDebug(st) << "System tray icon initialized";
    }

private slots:
    /**
     * @brief Slot called when MQTT connection state changes
     * @param state New MQTT connection state
     * 
     * @details
     * Updates the tray icon and tooltip based on connection status.
     */
    void onMqttStateChanged(QMqttClient::ClientState state)
    {
        bool connected = (state == QMqttClient::Connected);
        updateIcon(connected);
        
        QString statusText = connected ? "Connected" : "Disconnected";
        m_trayIcon->setToolTip("Kiot - " + statusText);
        
        qCDebug(st) << "MQTT state changed:" << state << "(connected:" << connected << ")";
    }
    
    /**
     * @brief Slot called when tray icon is activated
     * @param reason Activation reason
     * 
     * @details
     * Handles different activation reasons (click, double-click, context menu).
     */
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason)
    {
        switch (reason) {
            case QSystemTrayIcon::DoubleClick:
                // Double-click opens settings
                openSettings();
                break;
            case QSystemTrayIcon::Trigger:
                // Single click could show a notification or status
                showStatusNotification();
                break;
            default:
                break;
        }
    }
    
    /**
     * @brief Slot called when "Open Settings" is clicked
     * 
     * @details
     * Opens Kiot's settings/configuration dialog.
     */
    void onOpenSettings()
    {
        openSettings();
    }
    
    /**
     * @brief Slot called when "Reconnect" is clicked
     * 
     * @details
     * Attempts to reconnect to MQTT broker.
     */
    void onReconnect()
    {
        qCDebug(st) << "Manual reconnect requested";
        auto mqttClient = HaControl::mqttClient();
        if (mqttClient) {
            if (mqttClient->state() == QMqttClient::Disconnected) {
                mqttClient->connectToHost();
            }
        }
    }

    /**
     * @brief Slot called when "Quit" is clicked
     * 
     * @details
     * Gracefully shuts down Kiot.
     */
    void onQuit()
    {
        qCDebug(st) << "Quit requested from system tray";
        QApplication::quit();
    }

private:
    /**
     * @brief Creates icons for different connection states
     */
    void createIcons()
    {
        // Create green icon for connected state
        QPixmap connectedPixmap(32, 32);
        connectedPixmap.fill(Qt::transparent);
        QPainter painter1(&connectedPixmap);
        painter1.setRenderHint(QPainter::Antialiasing);
        painter1.setBrush(QBrush(QColor(76, 175, 80))); // Green
        painter1.setPen(Qt::NoPen);
        painter1.drawEllipse(4, 4, 24, 24);
        painter1.end(); // Explicit end
        m_connectedIcon = QIcon(connectedPixmap);
    
        // Create red icon for disconnected state
        QPixmap disconnectedPixmap(32, 32);
        disconnectedPixmap.fill(Qt::transparent);
        QPainter painter2(&disconnectedPixmap);
        painter2.setRenderHint(QPainter::Antialiasing);
        painter2.setBrush(QBrush(QColor(244, 67, 54))); // Red
        painter2.setPen(Qt::NoPen);
        painter2.drawEllipse(4, 4, 24, 24);
        painter2.end(); // Explicit end
        m_disconnectedIcon = QIcon(disconnectedPixmap);
    
        // Create yellow icon for connecting state
        QPixmap connectingPixmap(32, 32);
        connectingPixmap.fill(Qt::transparent);
        QPainter painter3(&connectingPixmap);
        painter3.setRenderHint(QPainter::Antialiasing);
        painter3.setBrush(QBrush(QColor(255, 193, 7))); // Yellow/Amber
        painter3.setPen(Qt::NoPen);
        painter3.drawEllipse(4, 4, 24, 24);
        painter3.end(); // Explicit end
        m_connectingIcon = QIcon(connectingPixmap);
    }
    /**
     * @brief Sets up the context menu
     */
    void setupMenu()
    {
        // Status item (non-clickable)
        m_statusAction = m_menu->addAction("Status: Disconnected");
        m_statusAction->setEnabled(false);
        
        m_menu->addSeparator();
        
        // Open Settings
        QAction *settingsAction = m_menu->addAction(QIcon::fromTheme("configure"), "Open Settings");
        connect(settingsAction, &QAction::triggered, this, &SystemTray::onOpenSettings);
        
        // Reconnect
        QAction *reconnectAction = m_menu->addAction(QIcon::fromTheme("view-refresh"), "Reconnect");
       connect(reconnectAction, &QAction::triggered, this, &SystemTray::onReconnect);
        


        // Quit
        QAction *quitAction = m_menu->addAction(QIcon::fromTheme("application-exit"), "Quit");
        connect(quitAction, &QAction::triggered, this, &SystemTray::onQuit);
        
        m_trayIcon->setContextMenu(m_menu);
    }
    
    /**
     * @brief Updates the tray icon based on connection state
     * @param connected True if connected to MQTT broker
     */
    void updateIcon(bool connected)
    {
        if (connected) {
            m_trayIcon->setIcon(m_connectedIcon);
            if (m_statusAction) {
                m_statusAction->setText("Status: Connected");
            }
        } else {
            m_trayIcon->setIcon(m_disconnectedIcon);
            if (m_statusAction) {
                m_statusAction->setText("Status: Disconnected");
            }
        }
    }
    
    /**
     * @brief Opens Kiot settings/configuration
     */
    void openSettings()
    {
        qCDebug(st) << "Opening settings";
        
        // Try to open KCM (KDE Configuration Module) if available
        QStringList kcmPaths = {
            "kcm_kiot",
            "kcm_kiot",
            "kiot-config"
        };
        
        for (const QString &kcm : kcmPaths) {
            if (QProcess::startDetached("kcmshell6", QStringList() << kcm)) {
                qCDebug(st) << "Opened KCM:" << kcm;
                return;
            }
        }
        
        // Fallback: open config file in text editor
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/kiotrc";
        if (QFile::exists(configPath)) {
            QProcess::startDetached("xdg-open", QStringList() << configPath);
            qCDebug(st) << "Opened config file:" << configPath;
        } else {
            qCWarning(st) << "Could not open settings - no KCM or config file found";
        }
    }
    
    /**
     * @brief Shows a status notification
     */
    void showStatusNotification()
    {
        auto mqttClient = HaControl::mqttClient();
        if (!mqttClient) return;
        
        QString title, message;
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information;
        
        switch (mqttClient->state()) {
            case QMqttClient::Connected:
                title = "Kiot - Connected";
                message = QString("Connected to %1:%2")
                          .arg(mqttClient->hostname())
                          .arg(mqttClient->port());
                icon = QSystemTrayIcon::Information;
                break;
                
            case QMqttClient::Connecting:
                title = "Kiot - Connecting";
                message = QString("Connecting to %1:%2...")
                          .arg(mqttClient->hostname())
                          .arg(mqttClient->port());
                icon = QSystemTrayIcon::Warning;
                break;
                
            case QMqttClient::Disconnected:
                title = "Kiot - Disconnected";
                message = "Disconnected from MQTT broker";
                if (mqttClient->error() != QMqttClient::NoError) {
                    message += "\n" + QString("Error: 1%").arg(static_cast<int>(mqttClient->error()));
                }
                icon = QSystemTrayIcon::Critical;
                break;
        }
        
        m_trayIcon->showMessage(title, message, icon, 3000);
    }

private:
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_menu;
    QAction *m_statusAction = nullptr;
    QIcon m_connectedIcon;
    QIcon m_disconnectedIcon;
    QIcon m_connectingIcon;
};

/**
 * @brief Sets up the SystemTray integration
 */
void setupSystemTray()
{
    new SystemTray(qApp);
}

REGISTER_INTEGRATION("SystemTray", setupSystemTray, true)

#include "systray.moc"