#pragma once
#include <QObject>

class PlayerContainer;
namespace KIOTShared::Entities {
class MediaPlayer;
}

class MprisMultiplexer : public QObject
{
    Q_OBJECT
public:
    explicit MprisMultiplexer(QObject *parent = nullptr);
    ~MprisMultiplexer() override;

private Q_SLOTS:
    void onNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);

private:
    void setupMediaPlayer();
    void discoverPlayers();
    void addPlayer(const QString &busName);
    void removePlayer(const QString &busName);
    void handleActivePlayer(PlayerContainer *container);
    QString downloadArtAsBase64(const QString &url);
    void updateMediaPlayerEntity(PlayerContainer *container);

    QList<PlayerContainer *> m_containers;
    PlayerContainer *m_activePlayer = nullptr;
    KIOTShared::Entities::MediaPlayer *m_playerEntity = nullptr;
};