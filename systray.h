#pragma once

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>

#include <QMqttClient>

class SystemTray : public QObject
{
    Q_OBJECT

public:
    explicit SystemTray(QObject *parent = nullptr);

private slots:
    void onMqttStateChanged(QMqttClient::ClientState state);
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onOpenSettings();
    void onReconnect();
    void onQuit();

private:
    void createIcons();
    void setupMenu();
    void updateIcon(QMqttClient::ClientState state);
    void openSettings();


private:
    QSystemTrayIcon *m_trayIcon = nullptr;
    QMenu *m_menu = nullptr;
    QAction *m_statusAction = nullptr;

    QIcon m_connectedIcon;
    QIcon m_disconnectedIcon;
    QIcon m_connectingIcon;
};
