// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

//This is a camera entity implementation based on info from here
//https://www.home-assistant.io/integrations/camera.mqtt/
// Use with the event entity to trigger update on camera image from a automation
// This is not meant to give a live stream, but rather a snapshot of the camera
#include "camera.h"
#include "core.h"
#include <QMqttClient>
#include <QJsonObject>
#include <QJsonDocument>
#include <QBuffer>
#include <QPixmap>
#include <QScreen>
#include <QGuiApplication>

Camera::Camera(QObject *parent)
    : Entity(parent)
{
}

void Camera::init()
{
    setHaType("camera");

    // MQTT topics
    setDiscoveryConfig("topic", baseTopic()); // state/topic for image
    setDiscoveryConfig("image_encoding", "b64");
    //This is not suported by default, but can be used to publish a trigger command to update camera image from ha
    setDiscoveryConfig("command_topic",baseTopic() + "/command");
    sendRegistration();

    //This is not suported by default from Home Assistants MQTT camera integration, 
    // but lets you publish a command and use it from the signal to trigger a fresh image in a integration
    auto subscription = HaControl::mqttClient()->subscribe(baseTopic() + "/command");
    connect(subscription, &QMqttSubscription::messageReceived, this, [this](const QMqttMessage &message) {
        qDebug() << name() << "Camera command received:" << QString::fromUtf8(message.payload());
        emit commandReceived(QString::fromUtf8(message.payload()));
    });
}

void Camera::publishImage(const QByteArray &imageDataBase64)
{
    if (HaControl::mqttClient()->state() != QMqttClient::Connected)
        return;

    // Publiser selve bildet som base64
    HaControl::mqttClient()->publish(baseTopic(), imageDataBase64, 0, true);

    // Valgfritt: publish attributes (f.eks timestamp)
    QVariantMap attrs;
    attrs["timestamp"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    attrs["size_bytes"] = imageDataBase64.size();
    setAttributes(attrs);
}

/*
 * Just example of how to send a screenshot here
 * void publishScreenshot()
 * {
 *     QScreen *screen = QGuiApplication::primaryScreen();
 *     if (!screen) return;

    QPixmap pixmap = screen->grabWindow(0);
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "JPEG", 70); // komprimering

    QByteArray base64Image = bytes.toBase64();
    publishImage(base64Image);
}
*/