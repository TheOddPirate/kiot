


#pragma once

#include <QObject>
#include <QVariantMap>
#include <BluezQt/Device>
#include <BluezQt/Battery>
#include <KIOTShared/kiotshared.h>

using KIOTShared::Entities::Switch;
using KIOTShared::PlatformHelper;


class BluetoothDeviceSwitch : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothDeviceSwitch(const BluezQt::DevicePtr &device, QObject *parent = nullptr);
    ~BluetoothDeviceSwitch() override;

    void unregisterSwitch();

private:
    void update();

    BluezQt::DevicePtr m_device;
    Switch *m_switch = nullptr;
};