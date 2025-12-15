// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file binarysensor.h
 * @brief Binary Sensor entity for Home Assistant integration
 * 
 * @details
 * This header defines the BinarySensor class which implements a binary
 * (on/off) sensor entity for Home Assistant. Binary sensors represent
 * boolean states such as presence detection, door/window status, or
 * device connectivity.
 * 
 * The binary sensor publishes its state as "on" or "off" strings to
 * Home Assistant and supports device class configuration for proper
 * UI representation.
 */

#pragma once
#include "entity.h"

/**
 * @class BinarySensor
 * @brief Binary sensor entity representing on/off states
 * 
 * @details
 * The BinarySensor class represents a binary sensor entity in Home Assistant,
 * used for monitoring boolean states. It automatically publishes state
 * changes to Home Assistant via MQTT and supports device class configuration
 * for appropriate UI icons and behavior.
 * 
 * Common use cases:
 * - User activity detection (active/inactive)
 * - Camera usage monitoring (in use/not in use)
 * - Device connectivity (connected/disconnected)
 * - Door/window status (open/closed)
 * 
 * @note Inherits from Entity to leverage MQTT discovery and topic management.
 */
class BinarySensor : public Entity
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a BinarySensor entity
     * @param parent Parent QObject for memory management (optional)
     */
    BinarySensor(QObject *parent = nullptr);
    
    /**
     * @brief Sets the binary sensor state
     * @param state Boolean state (true = on, false = off)
     * 
     * @details
     * Sets the current state of the binary sensor and automatically
     * publishes it to Home Assistant. The state is converted to
     * "on" (true) or "off" (false) strings for MQTT transmission.
     */
    void setState(bool state);
    
    /**
     * @brief Gets the current binary sensor state
     * @return bool Current state (true = on, false = off)
     */
    bool state() const;
    
protected:
    /**
     * @brief Initializes the binary sensor entity
     * 
     * @details
     * Overrides Entity::init() to set up binary sensor-specific
     * configuration. Called automatically when the MQTT client connects.
     */
    void init() override;
    
private:
    /**
     * @brief Publishes the current state to Home Assistant
     * 
     * @details
     * Internal method that publishes the current binary state
     * to the entity's MQTT state topic.
     */
    void publish();
    
    /** @private Current binary sensor state */
    bool m_state = false;
};