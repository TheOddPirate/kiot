// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

// Todo test longer to know that the coredump on handlePropertiesChanged is fixed
#include "core.h"
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include <QMap>
#include <QStringList>
#include <QTimer>

class BatteryWatcher : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.davidedmundson.kiot.Battery")

public:
    explicit BatteryWatcher(QObject *parent = nullptr);

private slots:
    void handlePropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated, const QString &devicePath);

private:
    void setupUPower();
    void registerSensorForDevice(const QString &devicePath, const QVariantMap &props);
    QVariantMap getBatteryProperties(const QString &devicePath);

    QMap<QString, Sensor *> m_pathToSensor;
};

BatteryWatcher::BatteryWatcher(QObject *parent)
    : QObject(parent)
{
    setupUPower();
}

void BatteryWatcher::setupUPower()
{
    QDBusInterface upower("org.freedesktop.UPower", "/org/freedesktop/UPower", "org.freedesktop.UPower", QDBusConnection::systemBus());

    if (!upower.isValid()) {
        qWarning() << "UPower interface not available";
        return;
    }

    QDBusReply<QList<QDBusObjectPath>> reply = upower.call("EnumerateDevices");
    if (!reply.isValid()) {
        qWarning() << "Failed to enumerate devices:" << reply.error().message();
        return;
    }

    for (const QDBusObjectPath &path : reply.value()) {
        QVariantMap props = getBatteryProperties(path.path());
        if (!props.isEmpty()) {
            registerSensorForDevice(path.path(), props);

            // Connect PropertiesChanged for this battery
            QDBusConnection::systemBus().connect("org.freedesktop.UPower",
                                                 path.path(),
                                                 "org.freedesktop.DBus.Properties",
                                                 "PropertiesChanged",
                                                 this,
                                                 SLOT(handlePropertiesChanged(QString, QVariantMap, QStringList, QString)));
        }
    }
}

QVariantMap BatteryWatcher::getBatteryProperties(const QString &devicePath)
{
    QDBusInterface propIface("org.freedesktop.UPower", devicePath, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());

    if (!propIface.isValid())
        return {};

    QVariantMap props;
    QList<QString> neededProps = {"Model", "NativePath", "Percentage", "Serial", "Vendor", "IsRechargeable", "State", "TimeToEmpty", "TimeToFull"};

    for (const QString &p : neededProps) {
        QDBusReply<QVariant> reply = propIface.call("Get", "org.freedesktop.UPower.Device", p);
        if (reply.isValid())
            props[p] = reply.value();
    }

    if (props.value("Percentage").isNull())
        return {}; // skip non-battery devices

    return props;
}

void BatteryWatcher::registerSensorForDevice(const QString &devicePath, const QVariantMap &props)
{
    QString name = props["Vendor"].toString() + " " + props["Model"].toString();
    if (name.isEmpty())
        name = devicePath.split("/").last();

    Sensor *sensor = new Sensor(this);
    sensor->setId(name.toLower().replace(" ", "_"));
    sensor->setName(name);
    sensor->setDiscoveryConfig("icon", "mdi:battery");
    sensor->setState(QString::number(props["Percentage"].toInt()));
    sensor->setAttributes(props);

    m_pathToSensor[devicePath] = sensor;
}

void BatteryWatcher::handlePropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated, const QString &devicePath)
{
    Q_UNUSED(interface)
    Q_UNUSED(invalidated)

    auto it = m_pathToSensor.find(devicePath);
    if (it == m_pathToSensor.end())
        return;

    Sensor *sensor = it.value();
    if (changed.contains("Percentage")) {
        sensor->setState(QString::number(changed["Percentage"].toInt()));
    }

    // Oppdater attributes
    QVariantMap current = sensor->getAttributes();
    for (auto itc = changed.constBegin(); itc != changed.constEnd(); ++itc) {
        current[itc.key()] = itc.value();
    }
    sensor->setAttributes(current);
}

void setupBattery()
{
    new BatteryWatcher(qApp);
}

REGISTER_INTEGRATION("Battery", setupBattery, true)
#include "battery.moc"
