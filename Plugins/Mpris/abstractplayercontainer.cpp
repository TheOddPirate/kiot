
#include "abstractplayercontainer.h"


AbstractPlayerContainer::AbstractPlayerContainer(QObject *parent)
    : QObject(parent)
{
}

AbstractPlayerContainer::~AbstractPlayerContainer() = default;

bool AbstractPlayerContainer::canControl() const
{
    return m_canControl.value();
}

bool AbstractPlayerContainer::canGoNext() const
{
    return m_effectiveCanGoNext.value();
}

bool AbstractPlayerContainer::canGoPrevious() const
{
    return m_effectiveCanGoPrevious.value();
}

bool AbstractPlayerContainer::canPause() const
{
    return m_effectiveCanPause.value();
}

bool AbstractPlayerContainer::canPlay() const
{
    return m_effectiveCanPlay.value();
}

bool AbstractPlayerContainer::canStop() const
{
    return m_effectiveCanStop.value();
}

bool AbstractPlayerContainer::canSeek() const
{
    return m_effectiveCanSeek.value();
}

LoopStatus::Status AbstractPlayerContainer::loopStatus() const
{
    return m_loopStatus.value();
}

double AbstractPlayerContainer::maximumRate() const
{
    return m_maximumRate.value();
}

double AbstractPlayerContainer::minimumRate() const
{
    return m_minimumRate.value();
}

PlaybackStatus::Status AbstractPlayerContainer::playbackStatus() const
{
    return m_playbackStatus.value();
}

qlonglong AbstractPlayerContainer::position() const
{
    return m_position.value();
}

double AbstractPlayerContainer::rate() const
{
    return m_rate.value();
}

ShuffleStatus::Status AbstractPlayerContainer::shuffle() const
{
    return m_shuffle.value();
}

double AbstractPlayerContainer::volume() const
{
    return m_volume.value();
}

QString AbstractPlayerContainer::track() const
{
    return m_track.value();
}

QString AbstractPlayerContainer::artist() const
{
    return m_artist.value();
}

QString AbstractPlayerContainer::artUrl() const
{
    return m_artUrl.value();
}

QString AbstractPlayerContainer::album() const
{
    return m_album.value();
}

double AbstractPlayerContainer::length() const
{
    return m_length;
}

unsigned AbstractPlayerContainer::instancePid() const
{
    return m_instancePid;
}

unsigned AbstractPlayerContainer::kdePid() const
{
    return m_kdePid.value();
}

bool AbstractPlayerContainer::canQuit() const
{
    return m_canQuit.value();
}

bool AbstractPlayerContainer::canRaise() const
{
    return m_canRaise.value();
}

bool AbstractPlayerContainer::canSetFullscreen() const
{
    return m_canSetFullscreen.value();
}

QString AbstractPlayerContainer::desktopEntry() const
{
    return m_desktopEntry;
}

bool AbstractPlayerContainer::fullscreen() const
{
    return m_fullscreen.value();
}
bool AbstractPlayerContainer::hasTrackList() const
{
    return m_hasTrackList;
}

QString AbstractPlayerContainer::identity() const
{
    return m_identity;
}

QStringList AbstractPlayerContainer::supportedMimeTypes() const
{
    return m_supportedMimeTypes;
}

QStringList AbstractPlayerContainer::supportedUriSchemes() const
{
    return m_supportedUriSchemes;
}

QString AbstractPlayerContainer::iconName() const
{
    return m_iconName;
}
