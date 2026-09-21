#pragma once

#include "abstractplayercontainer.h"

class OrgFreedesktopDBusPropertiesInterface;
class OrgMprisMediaPlayer2PlayerInterface;
class OrgMprisMediaPlayer2Interface;
class QDBusPendingCallWatcher;
inline constexpr QLatin1String MPRIS2_PATH{"/org/mpris/MediaPlayer2"};

class PlayerContainer : public AbstractPlayerContainer
{
    Q_OBJECT
public:
    explicit PlayerContainer(const QString &bus, QObject *parent = nullptr);
    ~PlayerContainer() override;

    QString busName() const;
    QString dbusname() const;

    void Play();
    void Pause();
    void Stop();
    void Next();
    void Previous();
    void setVolume(double value);
    void OpenUri(const QString &Uri);
    void seek(qlonglong pos);
    void setPosition(qlonglong value);

Q_SIGNALS:
    void stateChanged();
    void initialFetchFinished(PlayerContainer *container);
    void initialFetchFailed(PlayerContainer *container);

private Q_SLOTS:
    void onSeeked(qlonglong position);
    void onGetPropsFinished(QDBusPendingCallWatcher *watcher);
    void onPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);

private:
    QString getDesktopFileIcon(const QString &filePath);
    void updateFromMap(const QVariantMap &map);
    void initBindings();
    void refresh();
    void updatePosition();

    int m_fetchesPending = 0;
    QString m_busName;
    OrgFreedesktopDBusPropertiesInterface *m_propsIface = nullptr;
    OrgMprisMediaPlayer2PlayerInterface *m_playerIface = nullptr;
    OrgMprisMediaPlayer2Interface *m_rootIface = nullptr;

    QPropertyNotifier m_rateNotifier;
    QPropertyNotifier m_playbackStatusNotifier;
};