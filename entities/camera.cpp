// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file camera.cpp
 * @brief Implementation of the MQTT Camera entity for Home Assistant
 * 
 * @details
 * This file implements the Camera class which provides camera snapshot
 * functionality for the KIOT project. It allows publishing base64-encoded
 * images to Home Assistant via MQTT and supports command-triggered updates.
 * 
 * Based on Home Assistant's MQTT camera integration documentation:
 * https://www.home-assistant.io/integrations/camera.mqtt/
 * 
 * @note This implementation is designed for snapshot images, not live streaming.
 *       It can be triggered via MQTT commands to update the camera image.
 */

#include "camera.h"
#include "core.h"
#include <QMqttClient>
#include <QGuiApplication>

/**
 * @brief Constructs a Camera entity
 * @param parent Parent QObject (optional)
 * 
 * @details
 * Initializes the Camera entity. The actual MQTT setup is performed
 * in the init() method when the MQTT client connects.
 */
Camera::Camera(QObject *parent)
    : Entity(parent)
{
}

/**
 * @brief Initializes the camera entity with MQTT configuration
 * 
 * @details
 * Sets up the camera entity for Home Assistant MQTT discovery:
 * - Configures the entity type as "camera"
 * - Sets the state topic for image publishing
 * - Configures base64 image encoding
 * - Sets up a command topic for triggering image updates
 * - Subscribes to the command topic to receive update requests
 * 
 * The command topic functionality is a custom extension beyond the
 * standard Home Assistant MQTT camera integration, allowing integrations
 * to trigger image updates via MQTT commands.
 * 
 * @note The command topic is not part of the standard Home Assistant
 *       MQTT camera integration but provides flexibility for custom
 *       integrations to request fresh images.
 */
void Camera::init()
{
    setHaType("camera");
    // MQTT topics
    setDiscoveryConfig("topic", baseTopic()); // state/topic for image
    setDiscoveryConfig("image_encoding", "b64");
    // This is not supported by default, but can be used to publish a trigger command to update camera image from HA
    setDiscoveryConfig("command_topic", baseTopic() + "/command");
    sendRegistration();

    // This is not supported by default from Home Assistant's MQTT camera integration, 
    // but lets you publish a command and use it from the signal to trigger a fresh image in a integration
    auto subscription = HaControl::mqttClient()->subscribe(baseTopic() + "/command");
    connect(subscription, &QMqttSubscription::messageReceived, this, [this](const QMqttMessage &message) {
        qDebug() << name() << "Camera command received:" << QString::fromUtf8(message.payload());
        emit commandReceived(QString::fromUtf8(message.payload()));
    });
}

/**
 * @brief Publishes an image to Home Assistant
 * @param imageDataBase64 Image data encoded in base64 format
 * 
 * @details
 * Publishes the provided base64-encoded image to the camera's MQTT state topic.
 * Also updates entity attributes with metadata including:
 * - timestamp: Current UTC time in ISO format
 * - size_bytes: Size of the base64-encoded image data
 * 
 * The method checks if the MQTT client is connected before attempting
 * to publish. If disconnected, the image is not sent.
 * 
 * @note The image data should be in a format compatible with Home Assistant,
 *       typically JPEG or PNG format encoded as base64.
 * @note Attributes are published to a separate attributes topic for
 *       additional metadata about the image.
 */
void Camera::publishImage(const QByteArray &imageDataBase64)
{
    if (HaControl::mqttClient()->state() != QMqttClient::Connected)
        return;

    // Publish the image as base64
    HaControl::mqttClient()->publish(baseTopic(), imageDataBase64, 0, true);

    // Publish attributes
    QVariantMap attrs;
    attrs["timestamp"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    attrs["size_bytes"] = imageDataBase64.size();
    setAttributes(attrs);
}

/*
 * Example implementation for publishing screenshots:
 * 
 * @brief Example method for publishing a screenshot
 * 
 * @details
 * This example shows how to capture a screenshot and publish it
 * using the Camera entity. It captures the primary screen using
 * QScreen::grabWindow(), converts it to JPEG format, encodes it
 * as base64, and publishes it via publishImage().
 * 
 * @code
 * void publishScreenshot()
 * {
 *     QScreen *screen = QGuiApplication::primaryScreen();
 *     if (!screen) return;
 * 
 *     QPixmap pixmap = screen->grabWindow(0);
 *     QByteArray bytes;
 *     QBuffer buffer(&bytes);
 *     buffer.open(QIODevice::WriteOnly);
 *     pixmap.save(&buffer, "JPEG", 70); // compression
 * 
 *     QByteArray base64Image = bytes.toBase64();
 *     publishImage(base64Image);
 * }
 * @endcode
 */