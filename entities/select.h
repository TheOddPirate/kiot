// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file select.h
 * @brief Select entity for Home Assistant integration
 * 
 * @details
 * This header defines the Select class which implements a select entity
 * for Home Assistant. Select entities provide a dropdown-style interface
 * for choosing from predefined options, with bidirectional state
 * synchronization between KIOT and Home Assistant.
 * 
 * The select entity is commonly used for mode selection, theme switching,
 * or any scenario requiring choice from a fixed set of options.
 */

#pragma once
#include "entity.h"

/**
 * @class Select
 * @brief Select entity for option selection
 * 
 * @details
 * The Select class represents a select entity in Home Assistant, used for
 * choosing from a predefined list of options. It supports bidirectional
 * state synchronization: option selections from KIOT are reported to Home
 * Assistant, and selections from Home Assistant trigger option changes
 * in KIOT.
 * 
 * Common use cases:
 * - Audio output device selection
 * - Desktop theme or mode selection
 * - Preset configuration choices
 * - Mode switching (e.g., work/entertainment/gaming modes)
 * 
 * @note Inherits from Entity to leverage MQTT discovery and topic management.
 */
class Select : public Entity
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a Select entity
     * @param parent Parent QObject for memory management (optional)
     */
    explicit Select(QObject *parent = nullptr);

    /**
     * @brief Sets the available options for selection
     * @param opts List of available option strings
     * 
     * @details
     * Defines the list of options available for selection in the dropdown.
     * This should be called before initialization to ensure Home Assistant
     * displays the correct options.
     */
    void setOptions(const QStringList &opts);
    
    /**
     * @brief Sets the currently selected option
     * @param state Selected option string
     * 
     * @details
     * Sets the currently selected option and automatically publishes
     * it to Home Assistant. The option must be one of the options
     * defined via setOptions().
     */
    void setState(const QString &state);
    
    /**
     * @brief Gets the currently selected option
     * @return QString Currently selected option
     */
    QString state() const;
    
    /**
     * @brief Gets the list of available options
     * @return QStringList List of available options
     */
    QStringList options() const;

protected:
    /**
     * @brief Initializes the select entity
     * 
     * @details
     * Overrides Entity::init() to set up select-specific configuration,
     * including command topic subscription for receiving option selection
     * commands from Home Assistant.
     */
    void init() override;

signals:
    /**
     * @brief Signal emitted when option is selected from Home Assistant
     * @param newOption Newly selected option string
     * 
     * @details
     * Emitted when Home Assistant sends a command to select a different
     * option. Integrations should connect to this signal to implement
     * the actual behavior change for the selected option.
     */
    void optionSelected(QString newOption);

private:
    /**
     * @brief Publishes the current selection to Home Assistant
     * 
     * @details
     * Internal method that publishes the currently selected option
     * to the entity's MQTT state topic.
     */
    void publishState();

    /** @private Currently selected option */
    QString m_state;
    
    /** @private List of available options */
    QStringList m_options;
};