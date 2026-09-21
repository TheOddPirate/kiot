#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>
#include "mprismultiplexer.h"
using KIOTShared::Plugins::KIOTPluginInterface;
class TemplatePlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    TemplatePlugin(QObject *parent = nullptr);
    ~TemplatePlugin() override = default;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    
    bool startPlugin() override;
    bool stopPlugin() override;

private:
    MprisMultiplexer *m_multiplexer = nullptr;
};