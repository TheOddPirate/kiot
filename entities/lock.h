// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file lock.h
 * @brief Lock entity for Home Assistant integration
 * 
 * @details
 * This header defines the Lock class which implements a lock entity
 * for Home Assistant. Locks represent security devices that can be
 * locked/unlocked, with bidirectional state synchronization between
 * KIOT and Home Assistant.
 * 
 * The lock entity is commonly used for screen lock state monitoring
 * and control in desktop integration scenarios.
 */

#pragma once
#include "entity.h"

/**
 * @class Lock
 * @brief Lock entity for security device control
 * 
 * @details
 * The Lock class represents a lock entity in Home Assistant, used for
 * controlling lockable devices or features. It supports bidirectional
 * state synchronization: lock state changes from KIOT are reported to
 * Home Assistant, and lock/unlock commands from Home Assistant trigger
 * state change requests in KIOT.
 * 
 * Primary use case:
 * - Screen lock state monitoring and control
 * - Could be extended for other lockable features
 * 
 * @note Inherits from Entity to leverage MQTT discovery and topic management.
 */
class Lock : public Entity
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a Lock entity
     * @param parent Parent QObject for memory management (optional)
     */
    Lock(QObject *parent = nullptr);
    
    /**
     * @brief Sets the lock state
     * @param state Boolean state (true = locked, false = unlocked)
     * 
     * @details
     * Sets the current state of the lock and automatically publishes
     * it to Home Assistant. The state is converted to "locked" (true)
     * or "unlocked" (false) strings for MQTT transmission.
     */
    void setState(bool state);
    
    /**
     * @brief Gets the current lock state
     * @return bool Current state (true = locked, false = unlocked)
     */
    bool state() { return m_state; };
    
Q_SIGNALS:
    /**
     * @brief Signal emitted when lock state change is requested from Home Assistant
     * @param state Requested state (true = lock, false = unlock)
     * 
     * @details
     * Emitted when Home Assistant sends a command to change the lock state.
     * Integrations should connect to this signal to implement the actual
     * lock/unlock logic for the controlled device or feature.
     */
    void stateChangeRequested(bool state);
    
protected:
    /**
     * @brief Initializes the lock entity
     * 
     * @details
     * Overrides Entity::init() to set up lock-specific configuration,
     * including command topic subscription for receiving lock/unlock
     * commands from Home Assistant.
     */
    void init() override;
    
private:
    /** @private Current lock state */
    bool m_state = false;
};