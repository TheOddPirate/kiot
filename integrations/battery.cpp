// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "core.h"
#include "entities/battery.h"
#include <QCoreApplication>
#include <QDebug>
#include <Solid/DeviceNotifier>
#include <Solid/Device>
#include <Solid/Battery>
#include <Solid/DeviceInterface>

class BatteryWatcher : public QObject
{
    Q_OBJECT
public:
    explicit BatteryWatcher(QObject *parent = nullptr);

private slots:
    void deviceAdded(const QString &udi);
    void deviceRemoved(const QString &udi);
    void batteryChargeChanged(int chargePercent, const QString &udi);
    void batteryStateChanged(int state, const QString &udi);

private:
    void setupSolidWatching();
    void registerBattery(const QString &udi);
    void updateBatteryAttributes(const QString &udi);
    QMap<QString, Battery *> m_udiToSensor;
};

BatteryWatcher::BatteryWatcher(QObject *parent)
    : QObject(parent)
{
    setupSolidWatching();
}

void BatteryWatcher::setupSolidWatching()
{
    // Watch for device changes
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded,
            this, &BatteryWatcher::deviceAdded);
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved,
            this, &BatteryWatcher::deviceRemoved);

    // Find existing batteries
    const QList<Solid::Device> batteries = Solid::Device::listFromType(Solid::DeviceInterface::Battery);
    for (const Solid::Device &device : batteries) {
        registerBattery(device.udi());
    }
    
    qDebug() << "BatteryWatcher: Found" << batteries.count() << "battery devices";
}

void BatteryWatcher::deviceAdded(const QString &udi)
{
    Solid::Device device(udi);
    if (device.is<Solid::Battery>()) {
        qDebug() << "Battery added:" << device.displayName();
        registerBattery(udi);
    }
}

void BatteryWatcher::deviceRemoved(const QString &udi)
{
    auto it = m_udiToSensor.find(udi);
    if (it != m_udiToSensor.end()) {
        qDebug() << "Battery removed:" << udi;
        it.value()->deleteLater();
        m_udiToSensor.erase(it);
    }
}

void BatteryWatcher::registerBattery(const QString &udi)
{
    Solid::Device device(udi);
    Solid::Battery *battery = device.as<Solid::Battery>();
    
    if (!battery) {
        qWarning() << "Device is not a battery:" << udi;
        return;
    }
    QString udi_e = udi;
    // Create display name
    QString name = device.displayName();
    if (name.isEmpty()) {
        name = device.vendor() + " " + device.product();
    }
    if (name.trimmed().isEmpty()) {
        name = "Battery " + udi.split("/").last();
    }

    // Create sensor
    Battery *sensor = new Battery(this);
    sensor->setId("battery_" + udi_e.replace("/", "_").replace(":", "_"));
    sensor->setName(name);
    sensor->setDiscoveryConfig("icon", "mdi:battery");
    sensor->setDiscoveryConfig("unit_of_measurement", "%");
    sensor->setDiscoveryConfig("device_class", "battery");
    
    // Set initial state and attributes
    sensor->setState(QString::number(battery->chargePercent()));

    
    // Connect to battery signals
    connect(battery, &Solid::Battery::chargePercentChanged,
            this, [this, udi](int chargePercent) {
                batteryChargeChanged(chargePercent, udi);
            });
            
    connect(battery, &Solid::Battery::chargeStateChanged,
            this, [this, udi](int state) {
                batteryStateChanged(state, udi);
            });

    m_udiToSensor[udi] = sensor;
    updateBatteryAttributes(udi);
    qDebug() << "Registered battery:" << name << "at" << battery->chargePercent() << "%";
}

void BatteryWatcher::batteryChargeChanged(int chargePercent, const QString &udi)
{
    auto it = m_udiToSensor.find(udi);
    if (it != m_udiToSensor.end()) {
        it.value()->setState(QString::number(chargePercent));
        updateBatteryAttributes(udi);
    }
}

void BatteryWatcher::batteryStateChanged(int state, const QString &udi)
{
    Q_UNUSED(state)
    updateBatteryAttributes(udi);
}

void BatteryWatcher::updateBatteryAttributes(const QString &udi)
{
    auto it = m_udiToSensor.find(udi);
    if (it == m_udiToSensor.end()) return;
    Solid::Device device(udi);
    Solid::Battery *battery = device.as<Solid::Battery>();
    if (!battery) return;

    // Map Solid::Battery::ChargeState enum to readable strings
    QString chargeStateString;
    switch (battery->chargeState()) {
    case Solid::Battery::Charging:
        chargeStateString = "Charging";
        break;
    case Solid::Battery::Discharging:
        chargeStateString = "Discharging";
        break;
    case Solid::Battery::FullyCharged:
        chargeStateString = "Fully Charged";
        break;
    default:
        chargeStateString = "Unknown";
        break;
    }

    // Map Solid::Battery::BatteryType enum to readable strings
    QString batteryTypeString;
    switch (battery->type()) {
    case Solid::Battery::PrimaryBattery:
        batteryTypeString = "Primary Battery";
        break;
    case Solid::Battery::UpsBattery:
        batteryTypeString = "UPS Battery";
        break;
    case Solid::Battery::MonitorBattery:
        batteryTypeString = "Monitor Battery";
        break;
    case Solid::Battery::MouseBattery:
        batteryTypeString = "Mouse Battery";
        break;
    case Solid::Battery::KeyboardBattery:
        batteryTypeString = "Keyboard Battery";
        break;
    case Solid::Battery::KeyboardMouseBattery:
        batteryTypeString = "Keyboard/Mouse Battery";
        break;
    default:
        batteryTypeString = "Unknown";
        break;
    }

    QVariantMap attributes = {
        {"charge_state", chargeStateString},
        {"battery_type", batteryTypeString},
        {"technology", battery->technology()},
        {"rechargeable", battery->isRechargeable()},
        {"vendor", device.vendor()},
        {"product", device.product()},
        {"serial", device.as<Solid::Battery>()->serial()},
        {"udi", udi}
    };

    // Add time estimates if available
    if (battery->timeToEmpty() > 0) {
        attributes["time_to_empty_seconds"] = battery->timeToEmpty();
        attributes["time_to_empty_hours"] = QString::number(battery->timeToEmpty() / 3600.0, 'f', 1);
    }
    
    if (battery->timeToFull() > 0) {
        attributes["time_to_full_seconds"] = battery->timeToFull();
        attributes["time_to_full_hours"] = QString::number(battery->timeToFull() / 3600.0, 'f', 1);
    }

    it.value()->setAttributes(attributes);
}

void setupBattery()
{
    new BatteryWatcher(qApp);
}

REGISTER_INTEGRATION("Battery", setupBattery, true)

#include "battery.moc"