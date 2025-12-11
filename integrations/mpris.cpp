// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "core.h"
#include "entities/entities.h"

#include <QObject>
#include <QFile>

#include <QString>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusVariant>
#include <QDBusArgument>
#include <QDebug>
#include <QVariantMap>

class PlayerContainer : public QObject
{
    Q_OBJECT
public:
    explicit PlayerContainer(const QString &bus, QObject *parent = nullptr)
        : QObject(parent)
        , m_busName(bus)
    {
        // initial snapshot + start listening for changes
        refresh();
        // subscribe to PropertiesChanged signals for this service
        QDBusConnection::sessionBus().connect(
            m_busName,
            "/org/mpris/MediaPlayer2",
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            this,
            SLOT(onPropertiesChanged(QString,QVariantMap,QStringList))
        );
    }

    ~PlayerContainer() override
    {
        // unsubscribe signal for cleanliness (optional)
        QDBusConnection::sessionBus().disconnect(
            m_busName,
            "/org/mpris/MediaPlayer2",
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            this,
            SLOT(onPropertiesChanged(QString,QVariantMap,QStringList))
        );
    }

    void Play() { callMethod("Play"); }
    void Pause() { callMethod("Pause"); }
    void Stop() { callMethod("Stop"); }
    void Next() { callMethod("Next"); }
    void Previous() { callMethod("Previous"); }
    void setVolume(double v) { setProperty("Volume", v); }

    QVariantMap state() const { return m_state; }

signals:
    void stateChanged();

private slots:
    void onPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties)
    {
        Q_UNUSED(interfaceName)
        Q_UNUSED(invalidatedProperties)

        // Merge changedProperties into m_state
        bool changed = false;
        for (auto it = changedProperties.cbegin(); it != changedProperties.cend(); ++it) {
            // For Metadata, it's a QDBusArgument; store raw QVariant so caller can decode
            if (it.key() == "Metadata") {
                m_state["Metadata"] = it.value();
                changed = true;
            } else {
                if (!m_state.contains(it.key()) || m_state[it.key()] != it.value()) {
                    m_state[it.key()] = it.value();
                    changed = true;
                }
            }
        }
        if (changed) {
            emit stateChanged();
        }
    }

private:
    void refresh()
    {
        QDBusInterface iface(m_busName, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", QDBusConnection::sessionBus());
        QDBusPendingCall call = iface.asyncCall("GetAll", "org.mpris.MediaPlayer2.Player");
        auto watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher]() {
            QDBusPendingReply<QVariantMap> reply = *watcher;
            watcher->deleteLater();
            if (reply.isValid()) {
                // store full state snapshot
                QVariantMap replyMap = reply.value();
                // Ensure Metadata stays as QDBusArgument if present (GetAll returns a{sv}, already suitable)
                m_state = replyMap;
                emit stateChanged();
            }
        });
    }

    void callMethod(const QString &method)
    {
        QDBusInterface iface(m_busName, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", QDBusConnection::sessionBus());
        iface.call(method);
    }

    void setProperty(const QString &prop, const QVariant &val)
    {
        QDBusInterface iface(m_busName, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", QDBusConnection::sessionBus());
        iface.call("Set", "org.mpris.MediaPlayer2.Player", prop, QVariant::fromValue(QDBusVariant(val)));
    }

    QString m_busName;
    QVariantMap m_state;
};


class MprisMultiplexer : public QObject
{
    Q_OBJECT
public:
    explicit MprisMultiplexer(QObject *parent = nullptr) : QObject(parent)
    {
        setupMediaPlayer();
        discoverPlayers();
    }

private:
    void setupMediaPlayer()
    {
        m_playerEntity = new MediaPlayerEntity(this);
        m_playerEntity->setId("kiotprisstate");
        m_playerEntity->setName("Kiot Active MPRIS Player");

        // map HA commands to active MPRIS player
        connect(m_playerEntity, &MediaPlayerEntity::playRequested, this, [this]() {
            if (!m_activePlayer) return;
            m_activePlayer->Play();
        });
        connect(m_playerEntity, &MediaPlayerEntity::pauseRequested, this, [this]() {
            if (!m_activePlayer) return;
            m_activePlayer->Pause();
        });
        connect(m_playerEntity, &MediaPlayerEntity::stopRequested, this, [this]() {
            if (!m_activePlayer) return;
            m_activePlayer->Stop();
        });
        connect(m_playerEntity, &MediaPlayerEntity::nextRequested, this, [this]() {
            if (!m_activePlayer) return;
            m_activePlayer->Next();
        });
        connect(m_playerEntity, &MediaPlayerEntity::previousRequested, this, [this]() {
            if (!m_activePlayer) return;
            m_activePlayer->Previous();
        });
        connect(m_playerEntity, &MediaPlayerEntity::volumeChanged, this, [this](double volume) {
            if (!m_activePlayer) return;
            m_activePlayer->setVolume(volume);
        });
    }

    void discoverPlayers()
    {
        QDBusInterface dbusIface("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", QDBusConnection::sessionBus());
        QDBusPendingCall call = dbusIface.asyncCall("ListNames");
        auto watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher]() {
            QDBusPendingReply<QStringList> reply = *watcher;
            watcher->deleteLater();
            if (reply.isValid()) {
                for (const QString &svc : reply.value()) {
                    if (svc.startsWith("org.mpris.MediaPlayer2.")) {
                        auto *container = new PlayerContainer(svc, this);
                        // update whenever container emits stateChanged
                        connect(container, &PlayerContainer::stateChanged, this, [this, container]() {
                            handleActivePlayer(container);
                        });
                        m_containers.append(container);
                    }
                }
            }
        });
    }

    void handleActivePlayer(PlayerContainer *container)
    {
        // Always update entity from this container. If container starts playing, pick it as active.
        const QString status = container->state().value("PlaybackStatus").toString();
        qDebug() << "Player status:" << status << " from " << container;

        if (status == "Playing") {
            if (m_activePlayer != container) {
                m_activePlayer = container;
                qDebug() << "Active player set to:" << m_activePlayer;
            }
            updateMediaPlayerEntity(container);
            return;
        }

        // If this is already the active player, update it to reflect pause/stop/volume changes
        if (m_activePlayer == container) {
            updateMediaPlayerEntity(container);
            // keep m_activePlayer even if paused/stopped so HA shows last known info and can control
            return;
        }

        // If no active player set yet, pick the first non-empty container (fallback)
        if (!m_activePlayer && container->state().contains("PlaybackStatus")) {
            m_activePlayer = container;
            updateMediaPlayerEntity(container);
        }
    }

    void updateMediaPlayerEntity(PlayerContainer *container)
    {
        const auto &cState = container->state();
        QVariantMap state;
        // TODO remove after testing
        //qDebug() << "MPRIS container state raw:" << cState;

        // Playback status and volume
        state["state"]  = cState.value("PlaybackStatus", "Stopped").toString();
        state["volume"] = cState.value("Volume", 1.0).toDouble();

        // position/duration - MPRIS reports microseconds, convert to seconds (use ints for HA)
        qlonglong pos = 0;
        if (cState.contains("Position")) pos = cState.value("Position").toLongLong() / 1000000;
        qlonglong dur = 0;
        if (cState.contains("Metadata")) {
            QVariant metadataVar = cState.value("Metadata");
            if (metadataVar.canConvert<QDBusArgument>()) {
                QDBusArgument arg = metadataVar.value<QDBusArgument>();
                QVariantMap metadata;
                arg >> metadata;
                state["title"] = metadata.value("xesam:title").toString();
                QVariant artistVal = metadata.value("xesam:artist");
                if (artistVal.canConvert<QStringList>()) state["artist"] = artistVal.toStringList().join(", ");
                else state["artist"] = artistVal.toString();
                QVariant albumVal = metadata.value("xesam:album");
                if (albumVal.canConvert<QStringList>()) state["album"] = albumVal.toStringList().join(", ");
                else state["album"] = albumVal.toString();
                QVariant artVal = metadata.value("mpris:artUrl");
                state["art"] = artVal.toString();

                
                if (metadata.contains("mpris:length")) dur = metadata.value("mpris:length").toLongLong() / 1000000;
            }
        }
        // This is probably not needed as "mpris:length" looks to be correct here
        if (cState.contains("Duration") && dur == 0) {
            dur = cState.value("Duration").toLongLong() / 1000000;
        }

        state["position"] = static_cast<qint64>(pos);
        state["duration"] = static_cast<qint64>(dur);

        // Ensure strings are valid (avoid QVariant(Invalid) that breaks publishing)
        if (!state.contains("title")) state["title"] = QString();
        if (!state.contains("artist")) state["artist"] = QString();
        if (!state.contains("album")) state["album"] = QString();
        if(state.contains("art")){
            QString artUrl = state["art"].toString();
            if (artUrl.startsWith("file://")) {
                QString path = artUrl.mid(QString("file://").length());
                QFile f(path);
                if (f.open(QIODevice::ReadOnly)) {
                    QByteArray data = f.readAll();
                    QByteArray encoded = data.toBase64();
                    state["albumart"] = encoded;
                } else {
                    qWarning() << "Failed to read artwork file:" << path;
                }
           } else {
                qWarning() << "Unsupported artwork URL scheme:" << artUrl;
            }
        }
        //TODO remove when finished testing
        //qDebug() << "Setting media player state:" << state;
        m_playerEntity->setState(state);
    }

    QList<PlayerContainer *> m_containers;
    PlayerContainer *m_activePlayer = nullptr;
    MediaPlayerEntity *m_playerEntity;
};

void setupMprisIntegration()
{
    new MprisMultiplexer(qApp);
}

REGISTER_INTEGRATION("MPRISPlayer", setupMprisIntegration, true)

#include "mpris.moc"
