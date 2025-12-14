// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file notify.cpp
 * @brief Implementation of the MQTT Notify entity for Home Assistant
 * 
 * @details
 * This file implements the Notify class which provides notification
 * receiving functionality for the KIOT project. It allows receiving
 * messages from Home Assistant via MQTT that can trigger various
 * actions on the computer.
 * 
 * Based on Home Assistant's MQTT notify integration documentation:
 * https://www.home-assistant.io/integrations/notify.mqtt/
 * 
 * The notify entity can be used in Home Assistant automations to send
 * messages to the computer, which can then be processed for:
 * - Text-to-speech announcements (e.g., using Qt's QTextToSpeech)
 * - Desktop notifications via the system notification framework
 * - Custom automation triggers for local scripts or applications
 * - Audio alerts or system sounds
 */

#include "notify.h"
#include "core.h"
#include <QMqttClient>

/**
 * @brief Constructs a Notify entity
 * @param parent Parent QObject (optional)
 * 
 * @details
 * Initializes the Notify entity. The actual MQTT setup is performed
 * in the init() method when the MQTT client connects.
 */
Notify::Notify(QObject *parent)
    : Entity(parent)
{
}

/**
 * @brief Initializes the notify entity with MQTT configuration
 * 
 * @details
 * Sets up the notify entity for Home Assistant MQTT discovery:
 * - Configures the entity type as "notify"
 * - Sets the state topic for entity status
 * - Configures command topic for receiving notification messages
 * - Subscribes to the command topic to receive notifications
 * - Connects received messages to the notificationReceived signal
 * 
 * The entity follows Home Assistant's MQTT notify integration pattern,
 * where messages sent to the command topic are received and processed
 * locally. This enables Home Assistant to send messages to the computer
 * for various purposes like alerts, reminders, or automation triggers.
 * 
 * @note The state topic is used for entity status while the command
 *       topic receives actual notification messages from Home Assistant.
 */
void Notify::init()
{
    setHaType("notify");

    setDiscoveryConfig("state_topic", baseTopic());
    setDiscoveryConfig("command_topic", baseTopic() + "/notifications");
    sendRegistration();
    
    auto subscription = HaControl::mqttClient()->subscribe(baseTopic() + "/notifications");
    connect(subscription, &QMqttSubscription::messageReceived, this, [this](const QMqttMessage &message) {
        qDebug() << "Notify message received" << QString::fromUtf8(message.payload());
        emit notificationReceived(QString::fromUtf8(message.payload()));
    });
}
