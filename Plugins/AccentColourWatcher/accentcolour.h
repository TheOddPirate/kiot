#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>

using KIOTShared::Plugins::KIOTPluginInterface;

class AccentColourWatcher;
class AccentColourWatcherPlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    AccentColourWatcherPlugin(QObject *parent = nullptr);
    ~AccentColourWatcherPlugin() override = default;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    
    bool startPlugin() override;
    bool stopPlugin() override;
private:
    AccentColourWatcher *m_watcher;
};