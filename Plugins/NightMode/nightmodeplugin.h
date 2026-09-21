#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>
#include "nightmode.h"
using KIOTShared::Plugins::KIOTPluginInterface;
class NightModePlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    NightModePlugin(QObject *parent = nullptr);
    ~NightModePlugin() override = default;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    
    bool startPlugin() override;
    bool stopPlugin() override;

private:
    NightMode *m_nightMode = nullptr;
};