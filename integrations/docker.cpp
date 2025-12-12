// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "core.h"
#include "entities/entities.h"
#include <KConfigGroup>
#include <QThread>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include <QDebug>

class DockerEventListener : public QThread
{
    Q_OBJECT
public:
    explicit DockerEventListener(QObject *parent = nullptr)
        : QThread(parent) {}
    
    void stop() { m_stop = true; }
    
signals:
    void containerEvent(const QString &name, const QVariantMap &attrs);

protected:
    void run() override {
        QLocalSocket socket;
        socket.connectToServer("/var/run/docker.sock", QIODevice::ReadWrite);
        if (!socket.waitForConnected(1000)) return;

        QByteArray req = "GET /events HTTP/1.1\r\nHost: localhost\r\n\r\n";
        socket.write(req);
        socket.flush();

        while (!m_stop && socket.state() == QLocalSocket::ConnectedState) {
            if (!socket.waitForReadyRead(1000)) continue;

            QByteArray line = socket.readLine().trimmed();
            if (line.isEmpty()) continue;

            QJsonParseError err;
            auto doc = QJsonDocument::fromJson(line, &err);
            if (err.error != QJsonParseError::NoError) continue;
            if (!doc.isObject()) continue;

            auto obj = doc.object();
            if (obj.value("Type").toString() != "container") continue;

            QString name;
            auto actor = obj.value("Actor").toObject();
            auto attrsObj = actor.value("Attributes").toObject();
            if (attrsObj.contains("name")) name = attrsObj.value("name").toString();
            if (name.isEmpty()) continue;

            QVariantMap attrs;
            attrs["status"] = obj.value("status").toString();
            attrs["id"] = obj.value("id").toString();
            attrs["image"] = attrsObj.value("image").toString();

            emit containerEvent(name, attrs);
        }

        socket.disconnectFromServer();
    }

private:
    std::atomic<bool> m_stop{false};
    
};


class DockerSwitch : public QObject
{
    Q_OBJECT
public:
    explicit DockerSwitch(QObject *parent = nullptr)
        : QObject(parent) 
    {
        if (!ensureConfigDefaults()) {
            qWarning() << "Docker disabled due to missing socket";
            return;
        }

        auto cfg = KSharedConfig::openConfig();
        KConfigGroup grp = cfg->group("docker");

        // Create switches for enabled containers
        for (const auto &key : grp.keyList()) {
            if (key == "polltimer") continue;
            if (!grp.readEntry(key, false)) continue;

            qDebug() << "[docker] Enabling control for container" << key;

            auto *sw = new Switch(this);
            sw->setId("docker_" + key);
            sw->setName(key);
            sw->setHaIcon("mdi:docker");

            // Initial state + attributes
            updateSwitch(key, sw);

            connect(sw, &Switch::stateChangeRequested, this, [this, key](bool state){
                toggleContainer(key, state);
            });

            m_containers.append({key, sw});
        }

        // Start event listener
        m_listener = new DockerEventListener(this);
        connect(m_listener, &DockerEventListener::containerEvent,
                this, &DockerSwitch::handleEvent, Qt::QueuedConnection);
        m_listener->start();
    }
    ~DockerSwitch()
{
    if (m_listener) {
        m_listener->stop();
        // TODO implement so we can abort connection for faster shutdown
        //m_listener->socket()->abort();
        m_listener->quit();
        m_listener->wait();  // Vent til run() er ferdig
    }
}
private:
    struct ContainerInfo { QString name; Switch *sw; };
    QList<ContainerInfo> m_containers;
    DockerEventListener *m_listener = nullptr;

    bool ensureConfigDefaults() {
        auto cfg = KSharedConfig::openConfig();
        KConfigGroup grp = cfg->group("docker");

        if (!grp.exists()) {
            if (!grp.hasKey("polltimer"))
                grp.writeEntry("polltimer", 30);
            for (const auto &name : listAllContainers())
                if (!grp.hasKey(name)) grp.writeEntry(name, false);
            cfg->sync();
        }
        return true;
    }

    bool callDockerSocket(const QByteArray &req, QByteArray &response) {
        QLocalSocket socket;
        socket.connectToServer("/var/run/docker.sock", QIODevice::ReadWrite);
        if (!socket.waitForConnected(1000)) return false;

        socket.write(req);
        socket.flush();
        if (!socket.waitForReadyRead(5000)) return false;

        response = socket.readAll();
        socket.disconnectFromServer();
        return true;
    }

    QStringList listAllContainers() {
        QStringList names;
        QByteArray resp;
        if (!callDockerSocket("GET /containers/json?all=1 HTTP/1.0\r\n\r\n", resp)) return names;
        int headerEnd = resp.indexOf("\r\n\r\n");
        if (headerEnd == -1) return names;
        auto body = resp.mid(headerEnd + 4);
        auto doc = QJsonDocument::fromJson(body);
        if (!doc.isArray()) return names;

        for (const auto &val : doc.array()) {
            if (!val.isObject()) continue;
            auto arr = val.toObject()["Names"].toArray();
            if (arr.isEmpty()) continue;
            QString name = arr.first().toString();
            if (name.startsWith("/")) name.remove(0,1);
            if (!name.isEmpty()) names.append(name);
        }
        return names;
    }

    bool isRunning(const QString &name) {
        QByteArray resp;
        if (!callDockerSocket("GET /containers/json?all=0 HTTP/1.0\r\n\r\n", resp)) return false;
        int headerEnd = resp.indexOf("\r\n\r\n");
        if (headerEnd == -1) return false;
        auto body = resp.mid(headerEnd + 4);
        auto doc = QJsonDocument::fromJson(body);
        if (!doc.isArray()) return false;

        for (const auto &val : doc.array()) {
            if (!val.isObject()) continue;
            QString cName = val.toObject()["Names"].toArray().first().toString().remove(0,1);
            if (cName == name) return true;
        }
        return false;
    }

    void toggleContainer(const QString &name, bool start) {
        QByteArray req = QString("%1 /containers/%2/%3 HTTP/1.0\r\n\r\n")
                        .arg(start?"POST":"POST", name, start?"start":"stop").toUtf8();
        QByteArray resp;
        callDockerSocket(req, resp);
        for (auto &ci : m_containers)
            if (ci.name == name) updateSwitch(name, ci.sw);
    }

    void updateSwitch(const QString &name, Switch *sw) {
        bool running = isRunning(name);
        sw->setState(running);

        QByteArray resp;
        if (!callDockerSocket(QString("GET /containers/%1/json HTTP/1.0\r\n\r\n").arg(name).toUtf8(), resp)) return;
        int headerEnd = resp.indexOf("\r\n\r\n");
        if (headerEnd == -1) return;
        auto body = resp.mid(headerEnd + 4);
        auto doc = QJsonDocument::fromJson(body);
        if (!doc.isObject()) return;
        auto obj = doc.object();

        QVariantMap attrs;
        attrs["image"] = obj["Config"].toObject()["Image"].toString();
        attrs["status"] = obj["State"].toObject()["Status"].toString();
        attrs["running"] = obj["State"].toObject()["Running"].toBool();
        attrs["created"] = obj["Created"].toString();
        attrs["ports"] = obj["NetworkSettings"].toObject()["Ports"].toVariant();
        sw->setAttributes(attrs);
    }

private slots:
    void handleEvent(const QString &name, const QVariantMap & /*attrs*/) {
        for (auto &ci : m_containers) {
            if (ci.name == name) updateSwitch(name, ci.sw);
        }
    }
};

void setupDockerSwitch() {
    new DockerSwitch(qApp);
}

REGISTER_INTEGRATION("Docker",setupDockerSwitch,false)
#include "docker.moc"
