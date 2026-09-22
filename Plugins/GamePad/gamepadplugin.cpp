#include "gamepadplugin.h"
#include <QCoreApplication>

using KIOTShared::Entities::BinarySensor;
using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(plugin_logger,Gamepad)

GamepadPlugin::GamepadPlugin(QObject *parent)
    : QObject(parent)
{
}

QString GamepadPlugin::name() const
{
    return QString(PLUGIN_NAME).replace("\"", "");
}

QString GamepadPlugin::description() const
{
    return QString(PLUGIN_DESCRIPTION).replace("\"", "") + QString(" ") + QString(PLUGIN_DOMAIN).replace("\"", "");
}
QUrl GamepadPlugin::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}
QVersionNumber GamepadPlugin::version() const
{
    QString version = QString(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool GamepadPlugin::checkCompatibility()
{
    return true;
}

bool GamepadPlugin::enabledByDefault()
{
    return checkCompatibility();
}

bool GamepadPlugin::startPlugin()
{
    if(m_gamepad)
        stopPlugin();

    m_gamepad = new Gamepad(this);
    qCInfo(plugin_logger) << name() << " plugin started successfully";
    return true;
}

bool GamepadPlugin::stopPlugin()
{
    if(m_gamepad)
    {
        m_gamepad->deleteLater();
        m_gamepad = nullptr;
    }
    qCInfo(plugin_logger) << name() << " plugin stopped";
    return true;
}

#include "gamepadplugin.moc"