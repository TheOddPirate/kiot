#include "core.h"
#include "entities/entities.h"

#include <QObject>
#include <QString>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusVariant>
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
        refresh();
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

private:
    void refresh()
    {
        QDBusInterface iface(m_busName, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", QDBusConnection::sessionBus());
        QDBusPendingCall call = iface.asyncCall("GetAll", "org.mpris.MediaPlayer2.Player");
        auto watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher]() {
            QDBusPendingReply<QVariantMap> reply = *watcher;
            watcher->deleteLater();
            if(reply.isValid()) {
                m_state = reply.value();
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
        m_playerEntity->setId("kiot_mpris_player");
        m_playerEntity->setName("Kiot Active MPRIS Player");
        connect(m_playerEntity, &MediaPlayerEntity::playRequested, this, [this]() {
            if(!m_activePlayer) return;
            m_activePlayer->Play();
        });
        connect(m_playerEntity, &MediaPlayerEntity::pauseRequested, this, [this]() {
            if(!m_activePlayer) return;
            m_activePlayer->Pause();
        });
        connect(m_playerEntity, &MediaPlayerEntity::nextRequested, this, [this]() {
            if(!m_activePlayer) return;
            m_activePlayer->Next();
        });
        connect(m_playerEntity, &MediaPlayerEntity::previousRequested, this, [this]() {
            if(!m_activePlayer) return;
            m_activePlayer->Previous();
        });
        connect(m_playerEntity, &MediaPlayerEntity::volumeChanged, this, [this](double volume) {
            if(!m_activePlayer) return;
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
            if(reply.isValid()) {
                for(const QString &svc : reply.value()) {
                    if(svc.startsWith("org.mpris.MediaPlayer2.")) {
                        auto *container = new PlayerContainer(svc, this);
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
        const QString status = container->state().value("PlaybackStatus").toString();
        qDebug() << "Player status:" << status;
        if (status == "Playing") {
            if (m_activePlayer != container) {
                m_activePlayer = container;
                qDebug() << "Active player:" << m_activePlayer;
            }
            updateMediaPlayerEntity(container);
        }
    }

void updateMediaPlayerEntity(PlayerContainer *container)
{
    const auto &cState = container->state();
    QVariantMap state;
    qDebug() << cState;
    state["state"]  = cState.value("PlaybackStatus", "Stopped").toString();
    state["volume"] = cState.value("Volume", 1.0).toDouble();

    state["title"]  = QString();
    state["artist"] = QString();
    state["album"]  = QString();
    state["duration"] = cState.value("Duration", 0);// Convert from microseconds to seconds
    state["position"] = cState.value("Position", 0); // Convert from microseconds to seconds
    
    if (cState.contains("Metadata")) {
        QVariant metadataVar = cState.value("Metadata");
        if (metadataVar.canConvert<QDBusArgument>()) {
            QDBusArgument arg = metadataVar.value<QDBusArgument>();
            QVariantMap metadata;
            arg >> metadata;

            state["title"]  = metadata.value("xesam:title").toString();
            QVariant artistVal = metadata.value("xesam:artist");
            if (artistVal.canConvert<QStringList>()) {
                state["artist"] = artistVal.toStringList().join(", ");
            }
            QVariant albumVal = metadata.value("xesam:album");
            if (albumVal.canConvert<QStringList>()) {
                state["album"] = albumVal.toStringList().join(", ");
            }
        }
    }

    qDebug() << "Setting media player state:" << state;
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
