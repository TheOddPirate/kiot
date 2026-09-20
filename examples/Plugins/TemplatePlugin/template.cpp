#include "template.h"
#include <KIdleTime>
#include <QCoreApplication>

using KIOTShared::Entities::BinarySensor;
using KIOTShared::PlatformHelper;

DEFINE_LOGGER(plugin_logger,PLUGIN_NAME)

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
    return QUrl::fromString(QString(PLUGIN_DOMAIN).replace("\"", ""));
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
    auto sensor = new BinarySensor(this);
    sensor->setId("active");
    sensor->setName("Active");
    sensor->setDiscoveryConfig("device_class", "presence");

    // Idle-logikk fra din originale kode
    auto kidletime = KIdleTime::instance();
    auto id = kidletime->addIdleTimeout(60 * 1000);
    
    QObject::connect(kidletime, &KIdleTime::resumingFromIdle, this, [sensor]() {
        sensor->setState(true);
    });
    
    QObject::connect(kidletime, &KIdleTime::timeoutReached, this, [id, kidletime, sensor](int _id) {
        if (_id != id) {
            return;
        }
        sensor->setState(false);
        kidletime->catchNextResumeEvent();
    });
    
    sensor->setState(true);
    qCInfo(plugin_logger) << name() << " plugin started successfully";
    return true;
}

bool TemplatePlugin::stopPlugin()
{
    qCInfo(plugin_logger) << name() << " plugin stopped";
    return true;
}

#include "template.moc"