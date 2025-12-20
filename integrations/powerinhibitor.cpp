// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

/**
 * @file powerinhibitor.cpp
 * @brief Power Inhibitor integration for Kiot
 * 
 * This integration provides a switch entity that can inhibit system sleep
 * and screen locking through the KDE Solid Power Management DBus interface.
 * It allows Home Assistant to control and monitor power management inhibitions.
 */

#include "core.h"
#include "entities/entities.h"
#include "dbus/policyagentinterface.h" // <- DBus proxy

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(pi)
Q_LOGGING_CATEGORY(pi, "integration.PowerInhibitor")

/**
 * @class PowerInhibitor
 * @brief Integration class for power inhibition functionality
 * 
 * @details
 * This class provides a switch entity that can inhibit system sleep and
 * screen locking. It interfaces with the KDE Solid Power Management
 * DBus service to manage inhibitions and monitor their status.
 * 
 * The integration creates a switch entity that can be toggled to enable
 * or disable power inhibitions. It also provides attributes showing
 * currently active inhibitions from all applications.
 */
class PowerInhibitor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a PowerInhibitor instance
     * @param parent Parent QObject (optional)
     * 
     * @details
     * Initializes the switch entity, connects to the DBus power management
     * service, and sets up signal connections for monitoring inhibition changes.
     */
    explicit PowerInhibitor(QObject *parent = nullptr)
        : QObject(parent)
    {
        // Create switch entity
        m_switch = new Switch(this);
        m_switch->setId("inhibit");
        m_switch->setName("Sleep and screen lock inhibitor");
        m_switch->setState(false);
        m_switch->setAttributes(QVariantMap());

        // Connect switch toggle signal to handler
        connect(m_switch, &Switch::stateChangeRequested, this, &PowerInhibitor::onSwitchToggled);

        // Initialize DBus interface to KDE Solid Power Management
        m_policyAgent = new PolicyAgentInterface(
            "org.kde.Solid.PowerManagement",
            "/org/kde/Solid/PowerManagement/PolicyAgent",
            QDBusConnection::sessionBus(),
            this
        );

        // Connect to DBus signal for inhibition changes
        connect(m_policyAgent, &PolicyAgentInterface::InhibitionsChanged,
                this, &PowerInhibitor::itChanged);

        // Perform initial sync with current inhibition state
        updateStateFromDBus();
    }

private slots:
    /**
     * @brief Slot called when inhibitions change on DBus
     * @param added List of newly added inhibitions (unused)
     * @param removed List of removed inhibitions (unused)
     * 
     * @details
     * This slot is triggered by the DBus InhibitionsChanged signal.
     * It updates the switch state and attributes to reflect the current
     * inhibition status.
     */
    void itChanged(const QList<QStringList> &added, const QStringList &removed)
    {
        Q_UNUSED(added)
        Q_UNUSED(removed)
        updateStateFromDBus();
    }

    /**
     * @brief Slot called when the switch is toggled
     * @param enabled True if switch is being enabled, false if being disabled
     * 
     * Handles the switch toggle by either adding a new inhibition
     * (when enabled) or releasing the existing inhibition (when disabled).
     * Prevents duplicate inhibitions and ensures proper cleanup.
     */
    void onSwitchToggled(bool enabled)
    {
        qCDebug(pi) << "Switch toggled to" << enabled;

        if (enabled) 
        {
            // Don't create duplicate inhibition
            if (m_ownCookie != 0)
                return;

            // Add new inhibition through DBus
            uint cookie = m_policyAgent->AddInhibition(
                InhibitTypes,
                "Kiot",
                "Manual block from Home Assistant"
            );
            m_ownCookie = cookie;
        } 
        else 
        {
            // Don't release if no active inhibition
            if (m_ownCookie == 0)
                return;

            // Release the inhibition through DBus
            m_policyAgent->ReleaseInhibition(m_ownCookie);
            m_ownCookie = 0;
        }
    }

private:
    /**
     * @brief Updates the switch state and attributes from DBus
     * 
     * @details
     * Queries the current inhibition status from the DBus service and
     * updates the switch state accordingly. Also collects information
     * about all active inhibitions and sets them as switch attributes.
     */
    void updateStateFromDBus()
    {
        // Check if any inhibitions of our types are active
        bool active = m_policyAgent->HasInhibition(InhibitTypes);
        m_switch->setState(active);

        // Collect information about all active inhibitions
        QVariantList inhibitors;
        QList<QStringList> list = m_policyAgent->ListInhibitions();
        for (const QStringList &entry : list) {
            QVariantMap map;
            map["app"] = entry.value(0);     // Application name
            map["reason"] = entry.value(1);  // Inhibition reason
            inhibitors.append(map);
        }


        QVariantMap attributes;
        attributes["active_inhibitors"] = inhibitors;
        attributes["count"] = inhibitors.count();

        m_switch->setAttributes(attributes);
    }

private:
    Switch *m_switch = nullptr;                     ///< Switch entity for control
    PolicyAgentInterface *m_policyAgent = nullptr;  ///< DBus interface to power management
    uint m_ownCookie = 0;                           ///< Cookie for our own inhibition

    /**
     * @brief Bitmask of inhibition types to apply
     * 
     * Combines:
     * - 1: PreventSleep - Inhibits system sleep/suspend
     * - 2: PreventScreenLocking - Inhibits screen locking
     */
    static constexpr uint InhibitTypes =
        1 | // PreventSleep
        2;  // PreventScreenLocking
};

/**
 * @brief Setup function for the PowerInhibitor integration
 * 
 * @details
 * Creates and initializes the PowerInhibitor instance.
 * This function is called by the integration registration system.
 */
void setupPowerInhibitor()
{
    new PowerInhibitor(qApp);
}

/**
 * @brief Registers the PowerInhibitor integration with Kiot
 * 
 * Parameters:
 * - "PowerInhibitor": Integration name
 * - setupPowerInhibitor: Setup function pointer
 * - true: Auto-start enabled
 */
REGISTER_INTEGRATION("PowerInhibitor", setupPowerInhibitor, true)

#include "powerinhibitor.moc"