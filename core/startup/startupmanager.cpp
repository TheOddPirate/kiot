#include "startupmanager.h"
#include "systemdmanager.h" 
#include "backgroundmanager.h"
#include "desktopmanager.h"
#include <QTranslator>
#include <QObject>
#include <QCoreApplication>

#include <KIOTShared/kiotshared.h>
using KIOTShared::PlatformHelper;


DEFINE_LOGGER(sum, Core.Startup.StartupManager)

StartupManager::StartupManager(QObject *parent)
    : QObject(parent),
      m_systemdManager(new SystemdManager(this)),
      m_desktopManager(new DesktopManager(this)),
      m_backgroundManager(new BackgroundManager(this)) {}

StartupManager::~StartupManager() = default;

bool StartupManager::isAutostartEnabled() {
    if (PlatformHelper::isFlatpak() && m_backgroundManager->isAvailable()) {
        return m_backgroundManager->isAutostartEnabled();
    } else if (m_systemdManager->isAvailable()) {
        return m_systemdManager->isAutostartEnabled();
    } else if (m_desktopManager->isAvailable()) {
        return m_desktopManager->isAutostartEnabled();
    }
    
    return false;
}

bool StartupManager::setAutostart(bool enabled) {
    
    if (PlatformHelper::isFlatpak() && m_backgroundManager->isAvailable()) {
        qCDebug(sum) << tr("Running in Flatpak, delegating autostart configuration to BackgroundManager");
        if (m_backgroundManager->setupAutostart(enabled)) {
            return true;
        }
        qCWarning(sum) << tr("BackgroundManager autostart failed, attempting fallback");
    }

    if (!PlatformHelper::isFlatpak() && m_systemdManager->isAvailable()) {
        qCDebug(sum) << tr("Delegating autostart configuration to SystemdManager");
        if (m_systemdManager->setupAutostart(enabled)) {
            return true;
        }
        qCWarning(sum) << tr("Systemd autostart failed, attempting fallback to .desktop");
    }

    if (m_desktopManager->isAvailable()) {
        qCDebug(sum) << tr("Using DesktopManager for autostart");
        bool success = m_desktopManager->setupAutostart(enabled);
        if (success) {
            return true;
        }
    }

    if (enabled) {
        qCCritical(sum) << tr("CRITICAL: Autostart could not be enabled in this environment!");
        qCCritical(sum) << tr("Step-by-step for manual configuration:");
        qCCritical(sum) << tr("1. Open your desktop session's autostart settings.");
        qCCritical(sum) << tr("2. Add a new application entry manually pointing to: ") << QCoreApplication::applicationFilePath();
    }

    return false;
}