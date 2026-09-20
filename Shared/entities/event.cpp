// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "event.h"
#include "Transport/transportmanager.h"
#include <QMqttClient>


using KIOTShared::Transport::TransportManager;

namespace KIOTShared {
namespace Entities {


Event::Event(QObject *parent)
    : Entity(parent)
{
}

void Event::init()
{
    setHaType("device_automation");
    setDiscoveryConfig("automation_type", "trigger");
    setDiscoveryConfig("topic", baseTopic());
    setDiscoveryConfig("type", QStringLiteral("button_short_press"));
    setDiscoveryConfig("subtype", name());
    sendRegistration();
}

void Event::trigger()
{
    if (TransportManager::mqttClient() ->state() == QMqttClient::Connected) {
        TransportManager::mqttClient() ->publish(baseTopic(), "pressed", 0, false);
        TransportManager::mqttClient() ->publish(baseTopic(), "", 0, true);
    }
}
} // namespace Entities
} // namespace KIOTShared