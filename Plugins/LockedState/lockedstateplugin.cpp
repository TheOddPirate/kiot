#include "lockedstateplugin.h"

using KIOTShared::PlatformHelper;

DEFINE_PLUGIN_LOGGER(plugin_logger,PLUGIN_NAME)

LockedStatePlugin::LockedStatePlugin(QObject *parent)
    : QObject(parent)
{
}
LockedStatePlugin::~LockedStatePlugin()
{
}
QString LockedStatePlugin::name() const
{
    return QString(PLUGIN_NAME).replace("\"", "");
}

QString LockedStatePlugin::description() const
{
    return QString(PLUGIN_DESCRIPTION).replace("\"", "") + QString(" ") + QString(PLUGIN_DOMAIN).replace("\"", "");
}
QUrl LockedStatePlugin::url() const
{
    return QUrl(QString(PLUGIN_DOMAIN).replace("\"", ""));
}
QVersionNumber LockedStatePlugin::version() const
{
    QString version = QString(PLUGIN_VERSION).replace("\"", "");
    return QVersionNumber::fromString(version);
}

bool LockedStatePlugin::checkCompatibility()
{
    return true;
}

bool LockedStatePlugin::startPlugin()
{
    if(!m_lockedState)
        stopPlugin();
    m_lockedState = new LockedState(this);
    //m_lockedState = new LockedState(this);
    qCInfo(plugin_logger) << name() << " plugin started successfully";
    return true;
}

bool LockedStatePlugin::stopPlugin()
{
    if(m_lockedState)
    {
        m_lockedState->deleteLater();
        m_lockedState = nullptr;
    }

    qCInfo(plugin_logger) << name() << " plugin stopped";
    return true;
}

#include "lockedstateplugin.moc"