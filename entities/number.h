// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file number.h
 * @brief Number entity for Home Assistant integration
 * 
 * @details
 * This header defines the Number class which implements a number entity
 * for Home Assistant. Number entities provide a numeric input interface
 * with configurable range, step size, and unit of measurement, with
 * bidirectional value synchronization between KIOT and Home Assistant.
 * 
 * The number entity is commonly used for volume control, brightness
 * adjustment, or any scenario requiring numeric input within defined bounds.
 */

#pragma once
#include "entity.h"

/**
 * @class Number
 * @brief Number entity for numeric input and control
 * 
 * @details
 * The Number class represents a number entity in Home Assistant, used for
 * numeric value input and control within defined bounds. It supports
 * bidirectional value synchronization: value changes from KIOT are reported
 * to Home Assistant, and value changes from Home Assistant trigger value
 * change requests in KIOT.
 * 
 * Common use cases:
 * - Volume control (0-100%)
 * - Brightness adjustment (0-100%)
 * - Temperature setpoints
 * - Numeric parameter configuration
 * 
 * @note Inherits from Entity to leverage MQTT discovery and topic management.
 */
class Number : public Entity
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a Number entity
     * @param parent Parent QObject for memory management (optional)
     */
    Number(QObject *parent = nullptr);
    
    /**
     * @brief Sets the current numeric value
     * @param value Numeric value to set
     * 
     * @details
     * Sets the current value and automatically publishes it to Home Assistant.
     * The value is constrained to the defined range [min, max].
     */
    void setValue(int value);
    
    /**
     * @brief Gets the current numeric value
     * @return int Current numeric value
     */
    int value();
    
    /**
     * @brief Configures the number entity range and properties
     * @param min Minimum allowed value
     * @param max Maximum allowed value
     * @param step Step size for increment/decrement (default: 1)
     * @param unit Unit of measurement (default: "%")
     * 
     * @details
     * Configures the number entity's range, step size, and unit of measurement.
     * This should be called before initialization to ensure Home Assistant
     * displays the correct input constraints.
     * 
     * Example for volume control:
     * @code
     * number->setRange(0, 100, 5, "%");
     * @endcode
     */
    void setRange(int min, int max, int step = 1, const QString &unit = "%");

protected:
    /**
     * @brief Initializes the number entity
     * 
     * @details
     * Overrides Entity::init() to set up number-specific configuration,
     * including command topic subscription for receiving value change
     * commands from Home Assistant.
     */
    void init() override;
    
Q_SIGNALS:
    /**
     * @brief Signal emitted when value change is requested from Home Assistant
     * @param value Requested numeric value
     * 
     * @details
     * Emitted when Home Assistant sends a command to change the numeric value.
     * Integrations should connect to this signal to implement the actual
     * behavior change for the new value.
     */
    void valueChangeRequested(int value);

private:
    /** @private Current numeric value */
    int m_value = 0;
    
    /** @private Minimum allowed value */
    int m_min = 0;
    
    /** @private Maximum allowed value */
    int m_max = 100;
    
    /** @private Step size for increment/decrement */
    int m_step = 1;
    
    /** @private Unit of measurement */
    QString m_unit = "%";
};