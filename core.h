// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QVariantMap>
#include <QMqttSubscription>
#include <KSharedConfig>

class QMqttClient;
class ConnectedNode;

struct IntegrationFactory {
    QString name;
    std::function<void()> factory;
    std::function<void()> shutdown;
    bool onByDefault = true;  // ny flagg for default enabled
};

class HaControl : public QObject {
    Q_OBJECT
public:
    HaControl();
    ~HaControl();

    static QMqttClient *mqttClient() { return s_self->m_client; }

    static bool registerIntegrationFactory(const QString &name, std::function<void()> plugin, std::function<void()> shutdown, bool onByDefault = true);
    
private:
    void doConnect();
    void loadIntegrations(KSharedConfigPtr config);
    static QList<IntegrationFactory> s_integrations;
    static HaControl *s_self;
    QMqttClient *m_client;
    ConnectedNode *m_connectedNode;
};
//Macro for registration of integrations
#define REGISTER_INTEGRATION(nameStr, func, shutdown, onByDefault) \
static bool dummy##func  = HaControl::registerIntegrationFactory( \
    nameStr, [](){ func(); }, [](){ shutdown(); }, onByDefault);


