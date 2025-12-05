#include "binarysensor.h"
#include "core.h"
#include <QMqttClient>
BinarySensor::BinarySensor(QObject *parent)
    : Entity(parent)
{
}

void BinarySensor::init()
{
    setHaType("binary_sensor");
    setHaConfig({
        {"state_topic", baseTopic()},
        {"payload_on", "true"},
        {"payload_off", "false"}
    });
    sendRegistration();
    publish();
}

void BinarySensor::publish()
{
    qDebug() << name() << "publishing state" << m_state;
    if (HaControl::mqttClient()->state() == QMqttClient::Connected) {
        HaControl::mqttClient()->publish(baseTopic(), m_state ? "true" : "false", 0, true);
    }
}

void BinarySensor::setState(bool state)
{
    if (m_state == state) {
        return;
    }
    m_state = state;
    publish();
}

bool BinarySensor::state() const
{
    return m_state;
}
#include "binarysensor.moc"