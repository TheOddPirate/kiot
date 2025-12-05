// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "core.h"
#include "entities/entities.h"
#include <KConfigGroup>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QProcess>
#include <QRegularExpression>

class SystemDWatcher : public QObject
{
    Q_OBJECT
public:
    explicit SystemDWatcher(QObject *parent = nullptr);
    ~SystemDWatcher() = default;

    // Creates config group if missing, writes default false for all known services
    // Returns true if group existed or was successfully created
    bool ensureConfig();

    // Initializes switches based on config (only enabled)
    void init();

private slots:
    void onPropertiesChanged(const QString &interface, const QVariantMap &changedProps, const QStringList &invalidatedProps, const QDBusMessage &msg);

private:
    KSharedConfig::Ptr cfg;
    QHash<QString, Switch*> m_serviceSwitches;
    QDBusInterface *m_systemdUser = nullptr;
    QString sanitizeServiceId(const QString &svc);
    QStringList listUserServices() const;
};

SystemDWatcher::SystemDWatcher(QObject *parent)
    : QObject(parent)
{
    cfg = KSharedConfig::openConfig("kiotrc");
    if (!ensureConfig()) {
        qWarning() << "SystemDWatcher: Failed to ensure config";
        return;
    }
    init();

    m_systemdUser = new QDBusInterface("org.freedesktop.systemd1",
                                       "/org/freedesktop/systemd1",
                                       "org.freedesktop.systemd1.Manager",
                                       QDBusConnection::sessionBus(),
                                       this);
    if (!m_systemdUser->isValid()) {
        qWarning() << "SystemDWatcher: Failed to connect to systemd user DBus";
        return;
    }

    // Listen for PropertiesChanged signals from systemd units
    QDBusConnection::sessionBus().connect(
        "org.freedesktop.systemd1",
        "/org/freedesktop/systemd1",
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT(onPropertiesChanged(QString,QVariantMap,QStringList,QDBusMessage)));
}

bool SystemDWatcher::ensureConfig()
{
    KConfigGroup grp(cfg, "systemd");
    if (!grp.exists()) {
        grp.writeEntry("polltimer", 30); // optional default
        for (const QString &svc : listUserServices()) {
            grp.writeEntry(svc, false); // default disabled
        }
        cfg->sync();
    }
    return true;
}

void SystemDWatcher::init()
{
    KConfigGroup grp(cfg, "systemd");
    auto services = listUserServices();
    for (const QString &svc : services) {
        if (!grp.hasKey(svc) || !grp.readEntry(svc, false))
            continue; // skip disabled
        qDebug() << "SystemDWatcher: Adding service" << svc;
        auto *sw = new Switch(this);
        QString id = sanitizeServiceId(svc);
        sw->setId("systemd_" + id);
        sw->setName(svc);
        sw->setState(false); // initial state, will update via DBus
        m_serviceSwitches[svc] = sw;
        connect(sw, &Switch::stateChangeRequested, this, [this, svc](bool state) {
            QString cmd = state ? "start" : "stop";
            QProcess::execute("systemctl --user " + cmd + " " + svc);
            qDebug() << "Toggled service" << svc << "to" << (state ? "enabled" : "disabled");
        });
    }
}
QString SystemDWatcher::sanitizeServiceId(const QString &svc)
{
    QString id = svc;
    id.replace(QRegularExpression("[^a-zA-Z0-9]"), "_");
    return id;
}
QStringList SystemDWatcher::listUserServices() const
{
    // Only list *.service under --user
    QStringList services;
    QProcess p;
    p.start("systemctl", {"--user", "list-unit-files", "--type=service", "--no-pager", "--no-legend"});
    p.waitForFinished(3000);
    QString output = p.readAllStandardOutput();
    for (const QString &line : output.split("\n", Qt::SkipEmptyParts)) {
        QString svc = line.section(' ', 0, 0);
        if (!svc.isEmpty())
            services.append(svc);
    }
    return services;
}

void SystemDWatcher::onPropertiesChanged(const QString &interface, const QVariantMap &changedProps, const QStringList &invalidatedProps, const QDBusMessage &msg)
{   
    Q_UNUSED(invalidatedProps)
    if (interface != "org.freedesktop.systemd1.Unit")
        return;
     QString path = msg.path();
    QString name = path.section('/', -1);
    if (!m_serviceSwitches.contains(name))
        return;

    if (changedProps.contains("ActiveState")) {
        QString state = changedProps["ActiveState"].toString();
        m_serviceSwitches[name]->setState(state == "active");
    }
}

void setupSystemDWatcher()
{
    new SystemDWatcher(qApp);
}

REGISTER_INTEGRATION("SystemD", setupSystemDWatcher, true)
#include "systemd.moc"
