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
#include "dbus/policyagentinterface.h"

#include <QLoggingCategory>
#include <QCoreApplication>

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
        m_switch = new Switch(this);
        m_switch->setId("inhibit");
        m_switch->setName("Sleep and screen lock inhibitor");
        m_switch->setState(false);
        m_switch->setAttributes(QVariantMap());

        connect(
            m_switch,
            &Switch::stateChangeRequested,
            this,
            &PowerInhibitor::onSwitchToggled
        );

        m_policyAgent = new PolicyAgentInterface(
            "org.kde.Solid.PowerManagement",
            "/org/kde/Solid/PowerManagement/PolicyAgent",
            QDBusConnection::sessionBus(),
            this
        );

        connect(
            m_policyAgent,
            &PolicyAgentInterface::InhibitionsChanged,
            this,
            &PowerInhibitor::onInhibitionsChanged
        );

        // Ensure cleanup on clean shutdown
        connect(
            qApp,
            &QCoreApplication::aboutToQuit,
            this,
            &PowerInhibitor::releaseOwnInhibition
        );

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
    void onInhibitionsChanged(const QList<QStringList>&, const QStringList&)
    {
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

        if (enabled) {
            addInhibition();
        } else {
            releaseOwnInhibition();
        }
    }

private:
    /**
     * @brief Adds a new power inhibition through DBus
     * 
     * @details
     * Requests the DBus power management service to add a new inhibition
     * preventing system sleep and screen locking. The inhibition is registered
     * under the name "Kiot" with a descriptive reason. Prevents duplicate
     * inhibitions by checking if an inhibition is already active.
     * 
     * @note The inhibition cookie is stored in m_ownCookie for later release.
     */
    void addInhibition()
    {
        if (m_ownCookie != 0) {
            return;
        }

        auto reply = m_policyAgent->AddInhibition(
            InhibitTypes,
            "Kiot",
            "Manual block from Home Assistant"
        );

        reply.waitForFinished();

        if (reply.isError()) {
            qCWarning(pi) << "AddInhibition failed:" << reply.error().message();
            return;
        }

        m_ownCookie = reply.value();
        qCDebug(pi) << "Inhibition added with cookie" << m_ownCookie;
    }

    /**
     * @brief Releases the power inhibition created by this instance
     * 
     * @details
     * Releases the inhibition previously created by addInhibition() using
     * the stored cookie. Ensures proper cleanup of DBus resources and
     * resets the internal cookie tracking.
     */
    void releaseOwnInhibition()
    {
        if (m_ownCookie == 0) {
            return;
        }

        auto reply = m_policyAgent->ReleaseInhibition(m_ownCookie);
        reply.waitForFinished();

        if (reply.isError()) {
            qCWarning(pi) << "ReleaseInhibition failed:" << reply.error().message();
            return;
        }

        qCDebug(pi) << "Inhibition released:" << m_ownCookie;
        m_ownCookie = 0;
    }

    /**
     * @brief Updates the switch state and attributes from DBus
     * 
     * @details
     * Queries the DBus power management service for current inhibition status
     * and updates the switch entity accordingly. Sets the switch state based
     * on whether any inhibitions are active, and populates attributes with
     * information about all active inhibitors across the system.
     */
    void updateStateFromDBus()
    {
        // Switch state
        auto hasReply = m_policyAgent->HasInhibition(InhibitTypes);
        hasReply.waitForFinished();

        if (!hasReply.isError()) {
            m_switch->setState(hasReply.value());
        }

        // Attributes
        QVariantList inhibitors;

        auto listReply = m_policyAgent->ListInhibitions();
        listReply.waitForFinished();

        if (!listReply.isError()) {
            const QList<QStringList> list = listReply.value();
            for (const QStringList &entry : list) {
                QVariantMap map;
                map["app"] = entry.value(0);
                map["reason"] = entry.value(1);
                inhibitors.append(map);
            }
        }

        QVariantMap attributes;
        attributes["active_inhibitors"] = inhibitors;
        attributes["count"] = inhibitors.count();

        m_switch->setAttributes(attributes);
    }

private:
    Switch *m_switch = nullptr; /**< @brief Switch entity for controlling power inhibition */
    PolicyAgentInterface *m_policyAgent = nullptr; /**< @brief DBus interface for power management */
    uint m_ownCookie = 0; /**< @brief Cookie identifying our own inhibition for cleanup */

    /**
     * @brief Bitmask of inhibition types to apply
     * 
     * @details
     * Combines PreventSleep (1) and PreventScreenLocking (2) flags to
     * inhibit both system sleep and screen locking when active.
     */
    static constexpr uint InhibitTypes =
        1 | // PreventSleep
        2;  // PreventScreenLocking
};

/**
 * @brief Sets up the PowerInhibitor integration
 * 
 * @details
 * Factory function called by the integration framework to create and
 * initialize a PowerInhibitor instance. The instance is parented to
 * the application object for automatic cleanup.
 */
void setupPowerInhibitor()
{
    new PowerInhibitor(qApp);
}

REGISTER_INTEGRATION("PowerInhibitor", setupPowerInhibitor, true)

#include "powerinhibitor.moc"
