// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file switch.h
 * @brief Switch entity for Home Assistant integration
 * 
 * @details
 * This header defines the Switch class which implements a switch entity
 * for Home Assistant. Switches represent toggleable devices that can be
 * turned on/off from Home Assistant, with bidirectional state synchronization.
 * 
 * The switch supports command topics for remote control and maintains
 * consistent state between KIOT and Home Assistant.
 */

#pragma once
#include "entity.h"

/**
 * @class Switch
 * @brief Switch entity for toggleable devices
 * 
 * @details
 * The Switch class represents a switch entity in Home Assistant, used for
 * controlling toggleable devices or features. It supports bidirectional
 * state synchronization: state changes from KIOT are reported to Home
 * Assistant, and commands from Home Assistant trigger state change requests
 * in KIOT.
 * 
 * Common use cases:
 * - Bluetooth adapter control (on/off)
 * - Docker container start/stop
 * - SystemD service management
 * - Custom toggleable features
 * 
 * @note Inherits from Entity to leverage MQTT discovery and topic management.
 */
class Switch : public Entity
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a Switch entity
     * @param parent Parent QObject for memory management (optional)
     */
    Switch(QObject *parent = nullptr);
    
    /**
     * @brief Sets the switch state
     * @param state Boolean state (true = on, false = off)
     * 
     * @details
     * Sets the current state of the switch and automatically publishes
     * it to Home Assistant. The state is converted to "on" (true) or
     * "off" (false) strings for MQTT transmission.
     */
    void setState(bool state);
    
    /**
     * @brief Gets the current switch state
     * @return bool Current state (true = on, false = off)
     */
    bool state() { return m_state; };
    
Q_SIGNALS:
    /**
     * @brief Signal emitted when state change is requested from Home Assistant
     * @param state Requested state (true = turn on, false = turn off)
     * 
     * @details
     * Emitted when Home Assistant sends a command to change the switch state.
     * Integrations should connect to this signal to implement the actual
     * toggle logic for the controlled device or feature.
     */
    void stateChangeRequested(bool state);
    
protected:
    /**
     * @brief Initializes the switch entity
     * 
     * @details
     * Overrides Entity::init() to set up switch-specific configuration,
     * including command topic subscription for receiving state change
     * requests from Home Assistant.
     */
    void init() override;
    
private:
    /** @private Current switch state */
    bool m_state = false;
};