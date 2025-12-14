// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

//This is a notify entity implementation 
//https://www.home-assistant.io/integrations/notify.mqtt/

#include "notify.h"
#include "core.h"
#include <QMqttClient>
Notify::Notify(QObject *parent)
    : Entity(parent)
{
}

void Notify::init()
{
    setHaType("notify");

    setDiscoveryConfig("state_topic", baseTopic() + "/state");
    setDiscoveryConfig("command_topic", baseTopic() + "/notifications");
    sendRegistration();
    auto subscription = HaControl::mqttClient()->subscribe(baseTopic() + "/notifications");
    connect(subscription, &QMqttSubscription::messageReceived, this, [this](const QMqttMessage &message) {
        qDebug() << "Notify message received" << QString::fromUtf8(message.payload());
        emit notificationReceived(QString::fromUtf8(message.payload()));
    });


}
