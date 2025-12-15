// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file button.h
 * @brief Button entity for Home Assistant integration
 * 
 * @details
 * This header defines the Button class which implements a button entity
 * for Home Assistant. Buttons represent momentary actions that can be
 * triggered from Home Assistant to execute specific functions in KIOT.
 * 
 * The button provides a simple trigger mechanism without state persistence,
 * ideal for one-time actions and script execution.
 */

#pragma once
#include "entity.h"

/**
 * @class Button
 * @brief Button entity for momentary actions
 * 
 * @details
 * The Button class represents a button entity in Home Assistant, used for
 * triggering momentary actions. Unlike switches, buttons don't maintain
 * state - they simply emit a signal when pressed from Home Assistant.
 * 
 * Common use cases:
 * - Script execution triggers
 * - System power controls (suspend, restart, shutdown)
 * - Custom action triggers
 * - One-time automation triggers
 * 
 * @note Inherits from Entity to leverage MQTT discovery and topic management.
 */
class Button : public Entity
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a Button entity
     * @param parent Parent QObject for memory management (optional)
     */
    Button(QObject *parent = nullptr);
    
Q_SIGNALS:
    /**
     * @brief Signal emitted when button is triggered from Home Assistant
     * 
     * @details
     * Emitted when the button is pressed in Home Assistant. Integrations
     * should connect to this signal to execute the associated action.
     * 
     * Example usage:
     * @code
     * connect(button, &Button::triggered, []() {
     *     // Execute action when button is pressed
     *     system("suspend");
     * });
     * @endcode
     */
    void triggered();
    
protected:
    /**
     * @brief Initializes the button entity
     * 
     * @details
     * Overrides Entity::init() to set up button-specific configuration,
     * including command topic subscription for receiving trigger commands
     * from Home Assistant.
     */
    void init() override;
};