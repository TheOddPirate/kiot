#include "mprisplugin.h"
#include <QCoreApplication>

using KIOTShared::Entities::BinarySensor;
using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(plugin_logger,PLUGIN_NAME)

TemplatePlugin::TemplatePlugin(QObject *parent)
    : QObject(parent)
{
}

QString TemplatePlugin::name() const
{
    return QString(PLUGIN_NAME).replace("\"", "");
}

QString TemplatePlugin::description() const
{
    return QString(PLUGIN_DESCRIPTION).replace("\"", "") + QString(" ") + QString(PLUGIN_DOMAIN).replace("\"", "");
}
QUrl TemplatePlugin::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}
QVersionNumber TemplatePlugin::version() const
{
    QString version = QString(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool TemplatePlugin::checkCompatibility()
{
    return true;
}

bool TemplatePlugin::startPlugin()
{
    if(m_multiplexer)
        stopPlugin();
    m_multiplexer = new MprisMultiplexer(this);

    qCInfo(plugin_logger) << name() << " plugin started successfully";
    return true;
}

bool TemplatePlugin::stopPlugin()
{
    if(m_multiplexer)
    {
        m_multiplexer->deleteLater();
        m_multiplexer = nullptr;
    }
    qCInfo(plugin_logger) << name() << " plugin stopped";
    return true;
}

#include "mprisplugin.moc"