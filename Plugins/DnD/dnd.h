#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>

using KIOTShared::Plugins::KIOTPluginInterface;
using KIOTShared::Entities::BinarySensor;
using KIOTShared::DBusProperty;
class DnDPlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    DnDPlugin(QObject *parent = nullptr);
    ~DnDPlugin() override = default;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    
    bool startPlugin() override;
    bool stopPlugin() override;

private:
    BinarySensor *m_dndSensor; // Binary sensor for Do Not Disturb state
    DBusProperty *m_dndProperty; // DBus
};