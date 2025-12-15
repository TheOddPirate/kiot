// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "core.h"
#include "entities/entities.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QRegularExpression>
#include <QProcess>
#include <QTimer>
#include <QFileInfo>
#include <KConfigGroup>
#include <KProcess>

class SystemDWatcher : public QObject
{
    Q_OBJECT
public:
    explicit SystemDWatcher(QObject *parent = nullptr);
    ~SystemDWatcher() = default;

    bool ensureConfig();
    void delayedInit();

private slots:
    void onUnitPropertiesChanged(const QString &interface,
                                 const QVariantMap &changedProps,
                                 const QStringList &invalidatedProps,
                                 const QDBusMessage &msg);
    void performInit();

private:
    KSharedConfig::Ptr cfg;
    QHash<QString, Switch*> m_serviceSwitches;
    QDBusInterface *m_systemdUser = nullptr;
    QString sanitizeServiceId(const QString &svc);
    QStringList listUserServices() const;

    QString pathToUnitName(const QString &path) const;
    bool m_initialized = false;
};

namespace {
    static const QRegularExpression invalidCharRegex("[^a-zA-Z0-9]");
}

SystemDWatcher::SystemDWatcher(QObject *parent)
    : QObject(parent)
{
    cfg = KSharedConfig::openConfig("kiotrc");
    
    m_systemdUser = new QDBusInterface(
        "org.freedesktop.systemd1",
        "/org/freedesktop/systemd1",
        "org.freedesktop.systemd1.Manager",
        QDBusConnection::sessionBus(),
        this
    );

    if (!m_systemdUser->isValid()) {
        qWarning() << "SystemDWatcher: Failed to connect to systemd user D-Bus";
        return;
    }

    // Use single-shot timer to delay initialization
    QTimer::singleShot(1000, this, &SystemDWatcher::delayedInit);
}

// Ensure SystemD integration is enabled and create config entries
bool SystemDWatcher::ensureConfig()
{
    KConfigGroup intgrp(cfg, "Integrations");
    if (!intgrp.readEntry("SystemD", false)) {
        qWarning() << "Aborting: SystemD integration disabled, should not be running";
        return false;
    }
    
    // Don't list services here - do it in delayed initialization
    KConfigGroup grp(cfg, "systemd");
    if (!grp.exists()) {
        // Create empty config group, will be populated later
        grp.writeEntry("initialized", false);
        cfg->sync();
    }
    return true;
}

void SystemDWatcher::delayedInit()
{
    if (!ensureConfig()) {
        qWarning() << "SystemD: Failed to ensure config, aborting";
        return;
    }
    
    // Use another timer to ensure we're fully initialized
    QTimer::singleShot(500, this, &SystemDWatcher::performInit);
}

void SystemDWatcher::performInit()
{
    if (m_initialized) {
        return;
    }
    
    KConfigGroup grp(cfg, "systemd");
    
    // Get services list
    QStringList services = listUserServices();
    
    // Update config with current services if needed
    bool configUpdated = false;
    for (const QString &svc : services) {
        if (!grp.hasKey(svc)) {
            grp.writeEntry(svc, false); // default: disabled
            configUpdated = true;
        }
    }
    if (configUpdated) {
        cfg->sync();
    }
    
    // Initialize switches for enabled services
    for (const QString &svc : services) {
        if (!grp.hasKey(svc) || !grp.readEntry(svc, false))
            continue; // skip disabled

        auto *sw = new Switch(this);
        sw->setId("systemd_" + sanitizeServiceId(svc));
        sw->setName(svc);

        // Query initial state from D-Bus
        QDBusReply<QDBusObjectPath> unitPathReply = m_systemdUser->call("GetUnit", svc);
        if (unitPathReply.isValid()) {
            QDBusObjectPath unitPath = unitPathReply.value();

            QDBusInterface unitIface(
                "org.freedesktop.systemd1",
                unitPath.path(),
                "org.freedesktop.DBus.Properties",
                QDBusConnection::sessionBus()
            );

            QDBusReply<QVariant> stateReply = unitIface.call("Get", "org.freedesktop.systemd1.Unit", "ActiveState");
            if (stateReply.isValid()) {
                sw->setState(stateReply.value().toString() == "active");
            }

            // Listen for live property changes
            QDBusConnection::sessionBus().connect(
                "org.freedesktop.systemd1",
                unitPath.path(),
                "org.freedesktop.DBus.Properties",
                "PropertiesChanged",
                this,
                SLOT(onUnitPropertiesChanged(QString,QVariantMap,QStringList,QDBusMessage))
            );
        }

        // Connect switch to D-Bus for toggling service (works in flatpak)
        connect(sw, &Switch::stateChangeRequested, this, [this, svc](bool state) {
            if (!m_systemdUser || !m_systemdUser->isValid()) {
                qWarning() << "SystemD: D-Bus interface not available for toggling service";
                return;
            }
            
            QString method = state ? "StartUnit" : "StopUnit";
            QString mode = "replace"; // replace existing job if any
            
            QDBusReply<QDBusObjectPath> reply = m_systemdUser->call(method, svc, mode);
            if (!reply.isValid()) {
                qWarning() << "SystemD: Failed to" << (state ? "start" : "stop") 
                           << "service" << svc << ":" << reply.error().message();
            } else {
                qDebug() << "Toggled service" << svc << "to" << (state ? "start" : "stop");
            }
        });

        m_serviceSwitches[svc] = sw;
    }
    
    m_initialized = true;
    qDebug() << "SystemD: Initialized" << m_serviceSwitches.size() << "service switches";
}

QString SystemDWatcher::sanitizeServiceId(const QString &svc)
{
    QString id = svc;
    id.replace(invalidCharRegex, QStringLiteral("_"));
    return id;
}

// List all user services (*.service) - Use ListUnitFiles which actually works
QStringList SystemDWatcher::listUserServices() const
{
    QStringList services;

    QDBusMessage reply = m_systemdUser->call("ListUnitFiles");
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "SystemD: ListUnitFiles failed:" << reply.errorMessage();
        return services;
    }

    const QDBusArgument arg = reply.arguments().first().value<QDBusArgument>();

    arg.beginArray();
    while (!arg.atEnd()) {
        arg.beginStructure();

        QString path;
        QString state;
        arg >> path >> state;

        arg.endStructure();

        const QString unit = QFileInfo(path).fileName();
        if (unit.endsWith(".service")) {
            services.append(unit);
        }
    }
    arg.endArray();


    return services;
}



// Convert D-Bus path to proper unit name
QString SystemDWatcher::pathToUnitName(const QString &path) const
{
    QString name = path.section('/', -1);
    name.replace("_2e", ".");
    name.replace("_2d", "-");
    return name;
}

// Slot for handling live updates from systemd units
void SystemDWatcher::onUnitPropertiesChanged(const QString &interface,
                                             const QVariantMap &changedProps,
                                             const QStringList &invalidatedProps,
                                             const QDBusMessage &msg)
{
    Q_UNUSED(invalidatedProps);
    if (interface != "org.freedesktop.systemd1.Unit")
        return;

    QString unitName = pathToUnitName(msg.path());
    if (!m_serviceSwitches.contains(unitName))
        return;

    if (changedProps.contains("ActiveState")) {
        QString state = changedProps["ActiveState"].toString();
        if (m_serviceSwitches[unitName]->state() != (state == "active")) {
            m_serviceSwitches[unitName]->setState(state == "active");
            qDebug() << "Updated state for" << unitName << "to" << state;
        }
    }
}

// Setup function
void setupSystemDWatcher()
{
    new SystemDWatcher(qApp);
}

REGISTER_INTEGRATION("SystemD", setupSystemDWatcher, true)

#include "systemd.moc"