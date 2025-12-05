#include "battery.h"
#include "core.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMqttClient>
Battery::Battery(QObject *parent)
    : Entity(parent)
{
}

void Battery::init()
{
    setHaType("sensor");

    // Standard state topic
    setHaConfig({
        {"state_topic", baseTopic()},
        {"unit_of_measurement", "%"},
        {"device_class", "battery"},
        {"json_attributes_topic", baseTopic() + "/attributes"}
    });

    sendRegistration();
    publishState();
    publishAttributes();
}

void Battery::setState(const QString &state)
{
    m_state = state;
    publishState();
}

void Battery::setAttributes(const QVariantMap &attrs)
{
    m_attributes = attrs;
    publishAttributes();
}

void Battery::publishState()
{
    if (HaControl::mqttClient()->state() != QMqttClient::Connected)
        return;

    HaControl::mqttClient()->publish(baseTopic(), m_state.toUtf8(), 0, true);
}

void Battery::publishAttributes()
{
    if (HaControl::mqttClient()->state() != QMqttClient::Connected)
        return;

    QJsonObject obj;
    for (auto it = m_attributes.constBegin(); it != m_attributes.constEnd(); ++it)
        obj[it.key()] = QJsonValue::fromVariant(it.value());

    QJsonDocument doc(obj);
    HaControl::mqttClient()->publish(baseTopic() + "/attributes", doc.toJson(QJsonDocument::Compact), 0, true);
}

#include "battery.moc"