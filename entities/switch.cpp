// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

// Based on Home Assistant's MQTT switch integration documentation:
// https://www.home-assistant.io/integrations/switch.mqtt/
#include "switch.h"
#include "core.h"
#include <QMqttSubscription>
#include <QMqttClient>
#include <QJsonDocument>
#include <QJsonObject>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(entityswitch)
Q_LOGGING_CATEGORY(entityswitch, "entities.Switch")

Switch::Switch(QObject *parent)
    : Entity(parent)
{
    setHaType("switch");
}

void Switch::init()
{
    setDiscoveryConfig("state_topic", baseTopic());
    setDiscoveryConfig("command_topic", baseTopic() + "/set");
    setDiscoveryConfig("payload_on", "true");
    setDiscoveryConfig("payload_off", "false");

    sendRegistration();
    setState(m_state);

    auto subscription = HaControl::mqttClient()->subscribe(baseTopic() + "/set");
    if (subscription) {
        connect(subscription, &QMqttSubscription::messageReceived, this, [this](const QMqttMessage &message) {
            if (message.payload() == "true") {
                Q_EMIT stateChangeRequested(true);
            } else if (message.payload() == "false") {
                Q_EMIT stateChangeRequested(false);
            } else {
                qCWarning(entityswitch) << "unknown state request" << message.payload();
            }
        });
    }
}
void Switch::setState(bool state)
{
    m_state = state;
    if (HaControl::mqttClient()->state() == QMqttClient::Connected) {
        HaControl::mqttClient()->publish(baseTopic(), state ? "true" : "false", 0, true);
    }
}
