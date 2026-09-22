#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>
#include "gamepad.h"
using KIOTShared::Plugins::KIOTPluginInterface;
class GamepadPlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    GamepadPlugin(QObject *parent = nullptr);
    ~GamepadPlugin() override = default;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    bool enabledByDefault() override;
    bool startPlugin() override;
    bool stopPlugin() override;

private:
    Gamepad *m_gamepad = nullptr;
};