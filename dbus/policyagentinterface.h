/**
 * @file policyagentinterface.h
 * @brief Qt DBus interface for KDE Solid PowerManagement PolicyAgent
 * 
 * This file contains the stripped down version of a generated Qt DBus interface 
 * for interacting with the KDE Solid PowerManagement PolicyAgent service.
 * It provides methods for managing power management inhibitions (preventing sleep/screen lock).
 */

#ifndef POLICYAGENTINTERFACE_H
#define POLICYAGENTINTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QString>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>



/**
 * @struct PolicyAgentInhibition
 * @brief Represents a power management inhibition in the KDE Solid system
 * 
 * This structure encapsulates information about a single power management
 * inhibition. It's used by the PolicyAgentInterface to provide structured
 * access to inhibition data through properties.
 * 
 * @see PolicyAgentInterface::activeInhibitions()
 * @see PolicyAgentInterface::requestedInhibitions()
 */
struct PolicyAgentInhibition {
    /**
     * @brief Bitmask of inhibition types
     * 
     * A combination of the following flags:
     * - 1 (0x01): PreventSleep - Inhibits system sleep/suspend
     * - 2 (0x02): PreventScreenLocking - Inhibits screen locking
     * - 4 (0x04): PreventAutoDimming - Inhibits screen dimming
     * 
     * Multiple types can be combined using bitwise OR:
     * @code
     * uint types = 1 | 2; // Prevent both sleep and screen locking
     * @endcode
     */
    uint types;
    
    /**
     * @brief Name of the application that requested the inhibition
     * 
     * This is typically the application's display name or identifier
     * as provided when calling AddInhibition().
     */
    QString appName;
    
    /**
     * @brief Human-readable reason for the inhibition
     * 
     * Provides context about why the inhibition was requested.
     * Examples: "Playing video", "Downloading large file", 
     * "Manual block from Home Assistant"
     */
    QString reason;
};

/**
 * @class PolicyAgentInterface
 * @brief Qt DBus interface wrapper for org.kde.Solid.PowerManagement.PolicyAgent
 * 
 * This class provides a type-safe interface to the KDE Solid PowerManagement
 * PolicyAgent DBus service. It allows applications to:
 * - Add and release power management inhibitions
 * - Query current inhibition status
 * - Monitor inhibition changes via signals
 * 
 * The interface corresponds to the DBus service at:
 * - Service: org.kde.Solid.PowerManagement
 * - Path: /org/kde/Solid/PowerManagement/PolicyAgent
 * - Interface: org.kde.Solid.PowerManagement.PolicyAgent
 */
class PolicyAgentInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    /**
     * @brief Returns the DBus interface name
     * @return Constant string "org.kde.Solid.PowerManagement.PolicyAgent"
     */
    static inline const char *staticInterfaceName()
    { return "org.kde.Solid.PowerManagement.PolicyAgent"; }

    /**
     * @brief Constructs a PolicyAgentInterface instance
     * @param service DBus service name (e.g., "org.kde.Solid.PowerManagement")
     * @param path DBus object path (e.g., "/org/kde/Solid/PowerManagement/PolicyAgent")
     * @param connection DBus connection (session or system bus)
     * @param parent Parent QObject for memory management
     */
    explicit PolicyAgentInterface(const QString &service,
                                  const QString &path,
                                  const QDBusConnection &connection,
                                  QObject *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~PolicyAgentInterface();

    /**
     * @property ActiveInhibitions
     * @brief List of currently active power management inhibitions
     * 
     * This property contains all inhibitions that are currently preventing
     * power management actions (sleep, screen lock, etc.).
     */
    Q_PROPERTY(QList<PolicyAgentInhibition> ActiveInhibitions READ activeInhibitions)
    
    /**
     * @brief Getter for ActiveInhibitions property
     * @return List of currently active PolicyAgentInhibition objects
     */
    inline QList<PolicyAgentInhibition> activeInhibitions() const
    { return qvariant_cast<QList<PolicyAgentInhibition>>(property("ActiveInhibitions")); }

    /**
     * @property RequestedInhibitions
     * @brief List of requested but not yet active inhibitions
     * 
     * This property contains inhibitions that have been requested but
     * may not yet be active due to policy rules or conflicts.
     */
    Q_PROPERTY(QList<PolicyAgentInhibition> RequestedInhibitions READ requestedInhibitions)
    
    /**
     * @brief Getter for RequestedInhibitions property
     * @return List of requested PolicyAgentInhibition objects
     */
    inline QList<PolicyAgentInhibition> requestedInhibitions() const
    { return qvariant_cast<QList<PolicyAgentInhibition>>(property("RequestedInhibitions")); }

public Q_SLOTS:
    /**
     * @brief Add a new power management inhibition
     * @param types Bitmask of inhibition types to apply
     *              - 1: PreventSleep
     *              - 2: PreventScreenLocking
     *              - 4: PreventAutoDimming
     * @param app_name Name of the application requesting the inhibition
     * @param reason Human-readable reason for the inhibition
     * @return DBus pending reply containing a cookie (uint) that identifies this inhibition
     * 
     * The cookie should be saved and used to release the inhibition later.
     */
    QDBusPendingReply<uint> AddInhibition(uint types, const QString &app_name, const QString &reason)
    {
        QList<QVariant> args{types, app_name, reason};
        return asyncCallWithArgumentList(QStringLiteral("AddInhibition"), args);
    }

    /**
     * @brief Release a previously added inhibition
     * @param cookie The inhibition cookie returned by AddInhibition()
     * @return DBus pending reply (void)
     * 
     * Releases the inhibition identified by the given cookie.
     * If the cookie is invalid or the inhibition doesn't exist,
     * this call will fail silently on the DBus side.
     */
    QDBusPendingReply<> ReleaseInhibition(uint cookie)
    {
        QList<QVariant> args{cookie};
        return asyncCallWithArgumentList(QStringLiteral("ReleaseInhibition"), args);
    }

    /**
     * @brief Check if any inhibitions of specified types are active
     * @param types Bitmask of inhibition types to check
     * @return DBus pending reply containing true if any matching inhibitions are active
     * 
     * This method checks whether there are any active inhibitions
     * that include any of the specified inhibition types.
     */
    QDBusPendingReply<bool> HasInhibition(uint types)
    {
        QList<QVariant> args{types};
        return asyncCallWithArgumentList(QStringLiteral("HasInhibition"), args);
    }

    /**
     * @brief List all current inhibitions
     * @return DBus pending reply containing list of inhibitions as QStringList pairs
     * 
     * Each inhibition is represented as a QStringList with two elements:
     * - [0]: Application name
     * - [1]: Inhibition reason
     * 
     * Note: This returns a different format than the ActiveInhibitions property.
     * The property returns structured PolicyAgentInhibition objects, while
     * this method returns raw string lists for compatibility.
     */
    QDBusPendingReply<QList<QStringList>> ListInhibitions()
    {
        QList<QVariant> args;
        return asyncCallWithArgumentList(QStringLiteral("ListInhibitions"), args);
    }

Q_SIGNALS:
    /**
     * @brief Signal emitted when inhibitions change
     * @param added List of newly added inhibitions (as QStringList pairs)
     * @param removed List of removed inhibition cookies (as strings)
     * 
     * This signal is emitted whenever the set of active inhibitions changes.
     * Applications should connect to this signal to update their UI or
     * internal state when power management inhibitions are added or removed.
     * 
     * Each QStringList in 'added' contains:
     * - [0]: Application name
     * - [1]: Inhibition reason
     * 
     * The 'removed' list contains string representations of inhibition cookies.
     */
    void InhibitionsChanged(const QList<QStringList> &added, const QStringList &removed);
};

#endif // POLICYAGENTINTERFACE_H