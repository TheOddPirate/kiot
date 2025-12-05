#include "button.h"
#include "core.h"
#include <QMqttSubscription>
#include <QMqttClient>
#include <QObject>
Button::Button(QObject *parent)
    : Entity(parent)
{
}

void Button::init()
{
    setHaType("button");
    setHaConfig({
        {"command_topic", baseTopic()}
    });
    sendRegistration();

    auto subscription = HaControl::mqttClient()->subscribe(baseTopic());
    if (subscription) {
        connect(subscription, &QMqttSubscription::messageReceived, this, [this](const QMqttMessage &){
            Q_EMIT triggered();
        });
    }
}

#include "button.moc"