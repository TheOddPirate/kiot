#include "switch.h"
#include "core.h"
#include <QMqttSubscription>
#include <QMqttClient>
#include <QObject>

Switch::Switch(QObject *parent)
    : Entity(parent)
{
    setHaType("switch");
}

void Switch::init()
{
    setHaConfig({
        {"state_topic", baseTopic()},
        {"command_topic", baseTopic() + "/set"},
        {"payload_on", "true"},
        {"payload_off", "false"}
    });

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
                qWarning() << "unknown state request" << message.payload();
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

#include "switch.moc"