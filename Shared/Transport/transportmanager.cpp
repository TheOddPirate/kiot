#include "transportmanager.h"
#include "platformhelper.h"
#include <KSharedConfig>
#include <QTranslator>
#include <KConfigGroup>
#include <QApplication>
#include <QTranslator>
DEFINE_LOGGER(transportlogger, Shared.TransportManager)

namespace KIOTShared {
namespace Transport {

// Statiske variabler
static bool s_transportManagerInstantiated = false;
TransportManager *TransportManager::s_self = nullptr;

TransportManager::TransportManager(QObject *parent) : QObject(parent), m_client(nullptr)
{
    if (s_transportManagerInstantiated || s_self) {
        qCFatal(transportlogger)<< tr("TransportManager can only be instantiated once");
        return;
    }
    s_self = this;
    
    if(!initiateMqttClient())
    {
        qCWarning(transportlogger)<< tr("Mqtt client not initiated");
        s_self = nullptr;
        deleteLater();
        return;
    }

    s_transportManagerInstantiated = true;
}

TransportManager::~TransportManager()
{
    qCDebug(transportlogger)<< tr("TransportManager destructor called");

    if (this == s_self) {
        qCInfo(transportlogger)<< tr("TransportManager destroyed, releasing lock");
        s_self = nullptr;
        s_transportManagerInstantiated = false;
    }
}

bool TransportManager::initiateMqttClient()
{
    auto config = KSharedConfig::openConfig(PlatformHelper::configFilePath(), KConfig::SimpleConfig);
    if(!config->hasGroup("general"))
    {
        qCFatal(transportlogger)<< tr("No MQTT config found");
        Q_EMIT mqttConfigMissing();
        return false;
    }
    auto group = config->group("general");

    if(group.readEntry("password") == "mqtt-password-here"){
        qCFatal(transportlogger)<< tr("Default MQTT password detected");
        Q_EMIT mqttConfigMissing();
        return false;
    }
    
    m_client = new QMqttClient(this);
    m_client->setHostname(group.readEntry("host"));
    m_client->setPort(group.readEntry("port", 1883));
    m_client->setUsername(group.readEntry("user"));
    m_client->setPassword(group.readEntry("password"));
    m_client->setKeepAlive(3);

    if (m_client->hostname().isEmpty()) {
        qCCritical(transportlogger) << tr("Server is not configured, please check config file at:") << PlatformHelper::configFilePath();
        m_client->deleteLater();
        m_client = nullptr;
        return false;
    }
    
    qCInfo(transportlogger) << tr("MQTT server configured to") << m_client->hostname();

    reconnectTimer = new QTimer(this);
    reconnectTimer->setInterval(5000);
    connect(reconnectTimer, &QTimer::timeout, this, &TransportManager::doConnect);
    connect(m_client, &QMqttClient::stateChanged, this, &TransportManager::handleStateChanged);
    return true;
}

void TransportManager::handleStateChanged(QMqttClient::ClientState state) {
    Q_EMIT connectionStateChanged(state);
    
    switch (state) {
    case QMqttClient::Connected:
        qCInfo(transportlogger)<< tr("connected");
        reconnectTimer->stop();
        break;
    case QMqttClient::Connecting:
        qCInfo(transportlogger)<< tr("connecting");
        break;
    case QMqttClient::Disconnected:
        qCWarning(transportlogger)<< tr("Disconnected from broker");
        qCInfo(transportlogger)<< tr("disconnected");
        if(!QApplication::closingDown())
            reconnectTimer->start();
        break;
    }
}

void TransportManager::doConnect()
{
    if(m_client->state() == QMqttClient::Connecting || m_client->state() == QMqttClient::Connected)
    {
        qCInfo(transportlogger)<< tr("Already connected or connecting");
        return;
    }
    qCInfo(transportlogger)<< tr("Connecting to MQTT broker");
    auto config = KSharedConfig::openConfig(PlatformHelper::configFilePath(), KConfig::SimpleConfig);
    auto group = config->group("general");
    if (group.readEntry("tls", false)) {
        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
        m_client->connectToHostEncrypted(sslConfig);
    } else {
        m_client->connectToHost();
    }
}

void TransportManager::doDisconnect()
{
    qCInfo(transportlogger)<< tr("Disconnecting from MQTT broker");
    if(reconnectTimer->isActive())
        reconnectTimer->stop();
    if(m_client->state() == QMqttClient::Disconnected)
    {
        qCInfo(transportlogger)<< tr("Already disconnected");
        return;
    }
    m_client->disconnectFromHost();
}

} // namespace Transport
} // namespace KIOTShared