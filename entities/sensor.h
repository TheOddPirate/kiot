// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file sensor.h
 * @brief Sensor entity for Home Assistant integration
 * 
 * @details
 * This header defines the Sensor class which implements a generic sensor
 * entity for Home Assistant. Sensors represent measurable values such as
 * temperature, humidity, battery level, or any string/numeric data.
 * 
 * The sensor supports unit of measurement configuration and provides
 * flexible state representation for various data types.
 */

#pragma once
#include "entity.h"

/**
 * @class Sensor
 * @brief Generic sensor entity for measurable values
 * 
 * @details
 * The Sensor class represents a generic sensor entity in Home Assistant,
 * used for monitoring and reporting various types of data. It supports
 * string or numeric state values with optional unit of measurement
 * configuration.
 * 
 * Common use cases:
 * - Battery percentage with "%" unit
 * - Temperature readings with "°C" or "°F" units
 * - Active window titles as string states
 * - System metrics (CPU usage, memory usage)
 * - Custom string-based states
 * 
 * @note Inherits from Entity to leverage MQTT discovery and topic management.
 */
class Sensor : public Entity
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a Sensor entity
     * @param parent Parent QObject for memory management (optional)
     */
    Sensor(QObject *parent = nullptr);

    /**
     * @brief Sets the sensor state
     * @param state String representation of the sensor value
     * 
     * @details
     * Sets the current state of the sensor and automatically publishes
     * it to Home Assistant. The state can be any string representation,
     * including numeric values (e.g., "75", "25.5", "normal").
     * 
     * @note For numeric sensors, configure unit_of_measurement using
     *       setDiscoveryConfig() for proper Home Assistant display.
     */
    void setState(const QString &state);
    
    /**
     * @brief Gets the current sensor state
     * @return QString Current state as string
     */
    QString state() { return m_state; };
    
protected:
    /**
     * @brief Initializes the sensor entity
     * 
     * @details
     * Overrides Entity::init() to set up sensor-specific configuration.
     * Called automatically when the MQTT client connects.
     */
    void init() override;

private:
    /** @private Current sensor state as string */
    QString m_state;
    
    /**
     * @brief Publishes the current state to Home Assistant
     * 
     * @details
     * Internal method that publishes the current sensor state
     * to the entity's MQTT state topic.
     */
    void publishState();
};