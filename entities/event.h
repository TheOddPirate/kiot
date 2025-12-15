// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file event.h
 * @brief Event entity for Home Assistant integration
 * 
 * @details
 * This header defines the Event class which implements an event entity
 * for Home Assistant. Events represent triggers that can initiate Home
 * Assistant automations when emitted from KIOT.
 * 
 * The event entity is commonly used with global keyboard shortcuts to
 * allow physical key presses to trigger Home Assistant automations.
 */

#pragma once
#include "entity.h"

/**
 * @class Event
 * @brief Event entity for automation triggers
 * 
 * @details
 * The Event class represents an event entity in Home Assistant, used for
 * triggering automations from KIOT. Unlike other entities that represent
 * state, events are one-way triggers that initiate actions in Home Assistant
 * when emitted.
 * 
 * Primary use case:
 * - Global keyboard shortcut triggers
 * - System event notifications
 * - Custom automation triggers from KIOT to Home Assistant
 * 
 * @note Inherits from Entity to leverage MQTT discovery and topic management.
 * @note Events appear as device_triggers in Home Assistant for automation setup.
 */
class Event : public Entity
{
    Q_OBJECT
public:
    /**
     * @brief Constructs an Event entity
     * @param parent Parent QObject for memory management (optional)
     */
    Event(QObject *parent = nullptr);
    
    /**
     * @brief Triggers the event in Home Assistant
     * 
     * @details
     * Publishes an event trigger to Home Assistant, which can then
     * initiate automations configured to respond to this event.
     * 
     * Example usage with keyboard shortcuts:
     * @code
     * // When global shortcut is pressed
     * event->trigger();
     * @endcode
     */
    void trigger();
    
protected:
    /**
     * @brief Initializes the event entity
     * 
     * @details
     * Overrides Entity::init() to set up event-specific configuration
     * for Home Assistant device_trigger discovery.
     */
    void init() override;
};