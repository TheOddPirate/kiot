#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>
#include "shortcut.h"
using KIOTShared::Plugins::KIOTPluginInterface;
class ShortcutPlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    ShortcutPlugin(QObject *parent = nullptr);
    ~ShortcutPlugin() override = default;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    bool enabledByDefault() override;
    bool startPlugin() override;
    bool stopPlugin() override;

private:
    Shortcut *m_shortcut = nullptr;
};