#include "core.h"
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <KConfig>
#include <KSharedConfig>
#include <KConfigGroup>

class DockerSwitch : public QObject
{
    Q_OBJECT
public:
    explicit DockerSwitch(QObject *parent = nullptr);

private:
    struct ContainerInfo {
        QString name;
        Switch *sw;
    };

    QList<ContainerInfo> m_containers;
    QTimer m_timer;

    bool ensureConfigDefaults();
    bool callDockerSocket(const QByteArray &req, QByteArray &response);
    QStringList listAllContainers();
    bool isRunning(const QString &name);
    void toggleContainer(const QString &name, bool start);
    void refreshStatus();
};

DockerSwitch::DockerSwitch(QObject *parent)
    : QObject(parent)
{
    if (!ensureConfigDefaults()){
        qWarning() << "Docker disabled due to missing socket";
        return;
    }

    auto cfg = KSharedConfig::openConfig();
    KConfigGroup grp = cfg->group("docker");

    bool enable_timer = false;
    int pollTimerSec = grp.readEntry("polltimer", 30);
    if (pollTimerSec <= 0) pollTimerSec = 30;

    // Create switches for enabled containers
    for (const auto &key : grp.keyList()) {
        if (key == "polltimer") continue;
        if (!grp.readEntry(key, false)) continue;
        if (!enable_timer) enable_timer = true;

        qDebug() << "[docker] Enabling control of docker image" << key;

        auto *sw = new Switch(this);
        sw->setId("docker_" + key);
        sw->setName(key);
        sw->setDiscoveryConfig("icon", "mdi:application");
        sw->setState(isRunning(key));

        connect(sw, &Switch::stateChangeRequested, this, [this, key](bool state){
            toggleContainer(key, state);
        });

        m_containers.append({key, sw});
    }

    if (enable_timer) {
        connect(&m_timer, &QTimer::timeout, this, &DockerSwitch::refreshStatus);
        m_timer.start(pollTimerSec * 1000);
    }
}

bool DockerSwitch::ensureConfigDefaults()
{
    auto cfg = KSharedConfig::openConfig();
    //Auto disables integration if we failed talking to the docker socket
    KConfigGroup intgrp = cfg->group("Integrations");
    if (intgrp.readEntry("docker", false)) {
        QByteArray resp;
        if (!callDockerSocket("{}", resp) && resp.contains("Cannot connect")) {
            qWarning() << "Disabling Docker integration because socket is missing or inaccessible";
            intgrp.writeEntry("docker", false);
            intgrp.sync();
            return false;
        }
    }
    //Writes the needed parts to the config file if its missing
    KConfigGroup grp = cfg->group("docker");
    if (!grp.exists()) {
        if (!grp.hasKey("polltimer"))
            grp.writeEntry("polltimer", 30);

        for (const auto &name : listAllContainers()) {
            if (!grp.hasKey(name))
                grp.writeEntry(name, false); // default to disabled
        }

        cfg->sync();
    }
    return true;
}

bool DockerSwitch::callDockerSocket(const QByteArray &req, QByteArray &response)
{
    QLocalSocket socket;
    socket.connectToServer("/var/run/docker.sock", QIODevice::ReadWrite);
    if (!socket.waitForConnected(1000)) {
        response = "Cannot connect to Docker socket";
        qWarning() << response;
        return false;
    }

    socket.write(req);
    if (!socket.waitForBytesWritten(5000)) {
        response = "Failed to write to Docker socket";
        qWarning() << response;
        return false;
    }

    if (!socket.waitForReadyRead(10000)) {
        response =  "No response from Docker socket";
        qWarning() << response;
        return false;
    }

    response = socket.readAll();
    socket.disconnectFromServer();
    return true;
}

QStringList DockerSwitch::listAllContainers()
{
    QStringList names;
    QByteArray request = "GET /containers/json?all=1 HTTP/1.0\r\n\r\n";
    QByteArray response;
    if (!callDockerSocket(request, response)) return names;

    int headerEnd = response.indexOf("\r\n\r\n");
    if (headerEnd == -1) return names;

    auto body = response.mid(headerEnd + 4);
    auto doc = QJsonDocument::fromJson(body);
    if (!doc.isArray()) return names;

    for (const auto &val : doc.array()) {
        if (!val.isObject()) continue;
        auto obj = val.toObject();
        if (!obj["Names"].isArray()) continue;
        auto arr = obj["Names"].toArray();
        if (arr.isEmpty()) continue;

        QString name = arr.first().toString();
        if (name.startsWith("/")) name.remove(0, 1);
        if (!name.isEmpty()) names.append(name);
    }

    return names;
}

bool DockerSwitch::isRunning(const QString &name)
{
    QByteArray request = "GET /containers/json?all=0 HTTP/1.0\r\n\r\n";
    QByteArray response;
    if (!callDockerSocket(request, response)) return false;

    int headerEnd = response.indexOf("\r\n\r\n");
    if (headerEnd == -1) return false;

    auto body = response.mid(headerEnd + 4);
    auto doc = QJsonDocument::fromJson(body);
    if (!doc.isArray()) return false;

    for (const auto &val : doc.array()) {
        if (!val.isObject()) continue;
        auto obj = val.toObject();
        QString containerName = obj["Names"].toArray().first().toString().remove(0, 1);
        if (containerName == name) return true;
    }

    return false;
}

void DockerSwitch::toggleContainer(const QString &name, bool start)
{
    QByteArray request = QString("%1 /containers/%2/%3 HTTP/1.0\r\n\r\n")
                         .arg(start ? "POST" : "POST", name, start ? "start" : "stop")
                         .toUtf8();

    QByteArray response;
    if (!callDockerSocket(request, response))
        qWarning() << "Failed to" << (start ? "start" : "stop") << "container" << name;

    for (auto &ci : m_containers) {
        if (ci.name == name)
            ci.sw->setState(isRunning(name));
    }
}

void DockerSwitch::refreshStatus()
{
    for (auto &ci : m_containers)
        ci.sw->setState(isRunning(ci.name));
}

void setupDockerSwitch()
{
    new DockerSwitch(qApp);
}

REGISTER_INTEGRATION("Docker",setupDockerSwitch,false)
#include "docker.moc"
