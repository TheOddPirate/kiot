// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "ui_qt/mainwindow.h"
#include "core.h"
#include "core/startup/startupmanager.h"
#include <KIOTShared/kiotshared.h>

#include <KConfigGroup>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMqttClient>
#include <QTimer>
#include <QLoggingCategory>
#include <QApplication>

using KIOTShared::Entities::Entity;
using KIOTShared::Transport::TransportManager;
using KIOTShared::PlatformHelper;
using KIOTShared::Config::ConfigManager;
DEFINE_LOGGER(core, Core.HaControl)

HaControl *HaControl::s_self = nullptr;


class ConnectedNode : public Entity
{
    Q_OBJECT
public:
    ConnectedNode(QObject *parent);
    ~ConnectedNode();
    void init() override;
};

void HaControl::validateStartup(bool autostart)
{
    auto startupManager = new StartupManager(this);
    bool currentlyEnabled = startupManager->isAutostartEnabled();

    // Hvis ønsket tilstand matcher det som allerede er satt, trenger vi ikke gjøre noe
    if (currentlyEnabled == autostart) {
        qCDebug(core) << "Autostart is already in desired state:" << autostart;
        return;
    }

    QString actionStr = autostart ? "Enabling" : "Disabling";
    qCInfo(core) << actionStr << " autostartup";

    if (startupManager->setAutostart(autostart)) {
        if (autostart) {
            qCInfo(core) << "Autostart successfully enabled.";
            
            // Hvis vi bruker systemd (og ikke er i Flatpak), kan vi avslutte 
            // slik at systemd tar over kjøringen i bakgrunnen som planlagt.
            if (!PlatformHelper::isFlatpak()) {
                qCInfo(core) << "Running natively with systemd, closing instance to let systemd manage lifecycle.";
                QApplication::exit(0);
            }
        } else {
            qCInfo(core) << "Autostart successfully disabled.";
        }
    } else {
        qCWarning(core) << "Failed to" << (autostart ? "enable" : "disable") << "autostartup";
    }
}

bool HaControl::validateConfig()
{
    auto config = KSharedConfig::openConfig(PlatformHelper::configFilePath(), KConfig::SimpleConfig );
    if (!config->hasGroup("general")) {
        qCWarning(core) << "Config not found, creating default config and launching UI";
        KConfigGroup group(config, "general");
        group.writeEntry("host", "localhost");
        group.writeEntry("port", 1883);
        group.writeEntry("user", "mqtt");
        group.writeEntry("password", "mqtt-password-here");
        group.writeEntry("autostart", false);
        group.writeEntry("useSSL",false);
        group.writeEntry("discoveryprefix","homeassistant");
        config->sync();
        return false;
    }else{
        KConfigGroup group(config, "general");
        if(group.readEntry("password") == "mqtt-password-here"){
            MainWindow::sendNotification(QString(PROJECT_NAME),"Please configure your MQTT settings");
            m_mainWindow->show();
        }

    }
    return true;
}
HaControl::HaControl()
{
    s_self = this;
    m_mainWindow = MainWindow::instance();
    ConfigManager *m_conf = new ConfigManager(this);
    qCDebug(core) << m_conf->filePath();
    if(!validateConfig()) {
        QProcess::startDetached(QStringLiteral(PROJECT_NAME));
        QApplication::quit();
        return;
    }
    
    auto config = KSharedConfig::openConfig(PlatformHelper::configFilePath(), KConfig::SimpleConfig);
    auto group = config->group("general");
    auto autostart = group.readEntry("autostart", false);
    validateStartup(autostart);

    // 1. Start opp TransportManager som tar over hele MQTT-ansvaret
    auto transportManager = new TransportManager(this);

    // 2. Koble UI-oppdatering til TransportManager sine tilstandsendringer
    if (m_mainWindow) {
        connect(transportManager, &TransportManager::connectionStateChanged, 
                m_mainWindow, &MainWindow::updateIcon);
    }

    // 3. Håndter hvis konfigurasjonen mangler via signal
    connect(transportManager, &TransportManager::mqttConfigMissing, this, [this]() {
        if (m_mainWindow) {
            MainWindow::sendNotification(QString(PROJECT_NAME), "Please configure your MQTT settings");
            m_mainWindow->show();
        }
    });

    // 4. Opprett internt sensor-node (ConnectedNode henter nå klienten via HaControl::mqttClient())
    m_connectedNode = new ConnectedNode(this);

    // 5. Last inn integrasjoner
    loadIntegrations(config);

    transportManager->doConnect();
}

HaControl::~HaControl()
{
    if (m_connectedNode) {
        delete m_connectedNode;
        m_connectedNode = nullptr;
    }

    // Stopp og rydd opp i alle lastede plugins
    for (const auto &plugin : std::as_const(m_loadedPlugins)) {
        if (plugin.interface) {
            plugin.interface->stopPlugin();
        }
        if (plugin.loader) {
            plugin.loader->unload();
        }
    }
    m_loadedPlugins.clear();
}


void HaControl::loadIntegrations(KSharedConfigPtr config)
{
      auto integrationconfig = config->group("Integrations");

    if (!integrationconfig.exists()) {
        qCWarning(core) << "Integration group not found in config, defaulting to onByDefault values";
    }


    QStringList pluginDirStrings = PlatformHelper::appdataDirPaths();
    qCDebug(core) << "Checking plugin path:" << pluginDirStrings;
    if(QDir(QCoreApplication::applicationDirPath() + "/plugins").exists())
        pluginDirStrings.append(QCoreApplication::applicationDirPath() + "/plugins");
    for (QString pathName : pluginDirStrings)
    {
        if(!pathName.endsWith("/plugins"))
            pathName = pathName + "/plugins";
        qCDebug(core) << "Checking plugin path:" << pathName;
        QDir pluginsDir(pathName);
        if(!pluginsDir.exists())
            continue;
        for (const QString &fileName : pluginsDir.entryList(QDir::Files)) {
auto pluginLoader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName), this);
QObject *pluginInstance = pluginLoader->instance();

if (pluginInstance) {
    auto *kiotPlugin = qobject_cast<KIOTShared::Plugins::KIOTPluginInterface *>(pluginInstance);
    if (kiotPlugin) {
        QString pluginName = kiotPlugin->name();

        if (!integrationconfig.hasKey(pluginName)) {
            integrationconfig.writeEntry(pluginName, true);
            config->sync();
        }
        
        bool enabled = integrationconfig.readEntry(pluginName, true);

        if (enabled) {
            if (kiotPlugin->checkCompatibility()) {
                if (kiotPlugin->startPlugin()) {
                    qCInfo(core) << "Started plugin integration:" << pluginName;
                    // Lagre referansen slik at vi kan rydde opp senere
                    m_loadedPlugins.append({kiotPlugin, pluginLoader});
                } else {
                    qCWarning(core) << "Failed to start plugin:" << pluginName;
                    pluginLoader->unload();
                    pluginLoader->deleteLater();
                }
            } else {
                qCWarning(core) << "Plugin compatibility check failed for:" << pluginName;
                pluginLoader->unload();
                pluginLoader->deleteLater();
            }
        } else {
            qCDebug(core) << "Skipped disabled plugin:" << pluginName;
            pluginLoader->unload();
            pluginLoader->deleteLater();
        }
    } else {
        qCWarning(core) << "File" << fileName << "does not cast to KIOTPluginInterface!";
        pluginLoader->unload();
        pluginLoader->deleteLater();
    }
} else {
    qCWarning(core) << "Failed to load plugin from file" << fileName << ":" << pluginLoader->errorString();
    pluginLoader->deleteLater();
}

    }
}
/* ORiginale 

    // Finn mappen der plugins ligger relative til kjørbare fil eller fastsatt sti
    QDir pluginsDir(QCoreApplication::applicationDirPath() + "/plugins");
    
    // For testing kan du også bruke absolutt sti midlertidig:
    // QDir pluginsDir("/mnt/Development/Clones/kiot/build/bin/plugins");

    qCInfo(core) << "Scanning for plugins in:" << pluginsDir.absolutePath();

    for (const QString &fileName : pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *pluginInstance = pluginLoader.instance();

        if (pluginInstance) {
            auto *kiotPlugin = qobject_cast<KIOTShared::Plugins::KIOTPluginInterface *>(pluginInstance);
            if (kiotPlugin) {
                QString pluginName = kiotPlugin->name();

                if (!integrationconfig.hasKey(pluginName)) {
                    integrationconfig.writeEntry(pluginName, true); // eller onByDefault om du har det definert
                    config->sync();
                }
                
                bool enabled = integrationconfig.readEntry(pluginName, true);

                if (enabled) {
                    if (kiotPlugin->checkCompatibility()) {
                        kiotPlugin->startPlugin();
                        qCInfo(core) << "Started plugin integration:" << pluginName;
                    } else {
                        qCWarning(core) << "Plugin compatibility check failed for:" << pluginName;
                    }
                } else {
                    qCDebug(core) << "Skipped disabled plugin:" << pluginName;
                    pluginLoader.unload();
                }
            } else {
                qCWarning(core) << "File" << fileName << "loaded, but does not cast to KIOTPluginInterface!";
                pluginLoader.unload();
            }
        } else {
            qCWarning(core) << "Failed to load plugin from file" << fileName << ":" << pluginLoader.errorString();
        }
    }
        */
}

ConnectedNode::ConnectedNode(QObject *parent)
    : Entity(parent)
{
    setId("connected");
    setName("Connected");
    setHaType("binary_sensor");
    setDiscoveryConfig("state_topic", baseTopic());
    setDiscoveryConfig("payload_on", "on");
    setDiscoveryConfig("payload_off", "off");
    setDiscoveryConfig("device_class", "power");
    setDiscoveryConfig("device",
                       QVariantMap({{"name", hostname()},
                                    {"identifiers", "linux_ha_bridge_" + hostname()},
                                    {"sw_version", QStringLiteral(PROJECT_VERSION)},
                                    {"manufacturer", QStringLiteral(PROJECT_DEVELOPERS)}, //TODO update to KDE if we manage to make it part of the official portfolio
                                    {"model", QStringLiteral(PROJECT_NAME) },
                                    {"hw_version",QSysInfo::prettyProductName() + " - " + QSysInfo::kernelVersion()}}));

    auto c = TransportManager::mqttClient();
    c->setWillTopic(baseTopic());
    c->setWillMessage("off");
    c->setWillRetain(true);
}

ConnectedNode::~ConnectedNode()
{
    TransportManager::mqttClient()->publish(baseTopic(), "off", 0, true);
}

void ConnectedNode::init()
{
    sendRegistration();
    TransportManager::mqttClient()->publish(baseTopic(), "on", 0, true);
}

#include "core.moc"
