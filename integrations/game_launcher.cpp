// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later
// This is a early prototype, no flatpak suport atm
#include "core.h"
#include "entities/entities.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QObject>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QVariantMap>
#include <QDirIterator>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QByteArray>
#include <QApplication>
#include <QProcess>
#include <algorithm>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(gl)
Q_LOGGING_CATEGORY(gl, "integration.GameLauncher")

namespace {
    static const QRegularExpression invalidCharRegex("[^a-zA-Z0-9]");
}

/**
 * @class GameLauncher
 * @brief Steam game launcher integration for Kiot
 * 
 * @details
 * This integration discovers installed Steam games and creates
 * button entities for launching them directly from Home Assistant.
 * It parses Steam's libraryfolders.vdf and appmanifest files to
 * find all installed games and creates launch commands for them.
 */
class GameLauncher : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a GameLauncher instance
     * @param parent Parent QObject (optional)
     * 
     * @details
     * Initializes the game launcher by checking if Steam is installed,
     * finding the library configuration, and creating button entities
     * for all discovered games.
     */
    explicit GameLauncher(QObject *parent = nullptr)
        : QObject(parent)
    {
        if (!isSteamInstalled()) {
            qCWarning(gl) << "Steam is not installed or not found. GameLauncher integration disabled.";
            return;
        }

        QString libraryConfig = findLibraryConfig();
        if (libraryConfig.isEmpty()) {
            qCWarning(gl) << "Could not find Steam library configuration. GameLauncher integration disabled.";
            return;
        }

        qCDebug(gl) << "Found Steam library config:" << libraryConfig;

        QMap<QString, QString> games = getGamesDirect(libraryConfig);
        if (games.isEmpty()) {
            qCWarning(gl) << "No games found. GameLauncher integration disabled.";
            return;
        }
        ensureConfig(games);

        qCDebug(gl) << "Found" << games.size() << "games";

        createGameEntities(games);
    }

private slots:
    /**
     * @brief Slot called when a game button is pressed
     * @param gameId The Steam App ID of the game to launch
     * 
     * @details
     * Launches the specified game using Steam's URI scheme.
     * The game is launched in the background without bringing
     * Steam client to the foreground.
     */
    void onGameButtonPressed(const QString &gameId)
    {
        qCDebug(gl) << "Launching game with App ID:" << gameId;
        
        QString launchCommand = QString("steam://rungameid/%1").arg(gameId);
        
        QProcess *process = new QProcess(this);
        process->start("xdg-open", QStringList() << launchCommand);
        
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this, gameId, process](int exitCode, QProcess::ExitStatus exitStatus) {
                    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                        qCDebug(gl) << "Successfully launched game:" << gameId;
                    } else {
                        qCWarning(gl) << "Failed to launch game" << gameId 
                                     << ": exit code" << exitCode;
                    }
                    process->deleteLater();
                });
    }

private:
    /**
     * @brief Sanitizes a game name for use as a config key
     * @param gameName The original game name
     * @return Sanitized string safe for config keys
     */
    QString sanitizeGameName(const QString &gameName)
    {

        QString id = gameName.toLower();
        id.replace(invalidCharRegex, QStringLiteral("_"));
        if (!id.isEmpty() && id[0].isDigit()) {
            id.prepend("game_");
        }
        return id;
    }

    /**
     * @brief Ensures configuration has entries for all discovered games
     * @param games Map of App ID to game name
     * 
     * @details
     * Updates the steam config group with sanitized game names as keys
     * and true/false values indicating whether each game should be exposed.
     * New games default to false (not exposed).
     */
    void ensureConfig(const QMap<QString, QString> &games)
    {
        auto cfg = KSharedConfig::openConfig();
        KConfigGroup grp = cfg->group("steam");
    
        bool configChanged = false;
        
        // For each discovered game
        for (auto it = games.constBegin(); it != games.constEnd(); ++it) {
            const QString &gameName = it.value();
            QString configKey = sanitizeGameName(gameName);
        
            // Check if this game already has a config entry
            if (!grp.hasKey(configKey)) {
                // New game - add to config with default expose=false
                grp.writeEntry(configKey, false);
                configChanged = true;
                qCDebug(gl) << "Added new steam game to config:" << configKey << "= false";
            }
        }
    
        // Get all current config keys
        const QStringList currentKeys = grp.keyList();
    
        // Remove games from config that are no longer installed
        for (const QString &configKey : currentKeys) {
            bool gameStillExists = false;
        
            // Check if this config key corresponds to any current game
            for (auto it = games.constBegin(); it != games.constEnd(); ++it) {
                const QString &gameName = it.value();
                if (sanitizeGameName(gameName) == configKey) {
                    gameStillExists = true;
                    break;
                }
            }
        
            if (!gameStillExists) {
                grp.deleteEntry(configKey);
                configChanged = true;
                qCDebug(gl) << "Removed unavailable game from config:" << configKey;
            }
        }
    
        if (configChanged) {
            cfg->sync();
            qCDebug(gl) << "Configuration updated with current games";
        }
    }

    /**
     * @brief Checks if Steam is installed on the system
     * @return True if Steam is found, false otherwise
     * 
     * @details
     * Checks for Steam installation by looking for:
     * 1. Steam executable in PATH
     * 2. Steam desktop file
     * 3. Steam installation directory
     */
    bool isSteamInstalled()
    {
        // Check if steam command is in PATH
        QProcess process;
        process.start("which", QStringList() << "steam");
        process.waitForFinished();
        if (process.exitCode() == 0) {
            return true;
        }

        // Check for Steam desktop file
        QStringList desktopPaths = {
            QDir::homePath() + "/.local/share/applications/steam.desktop",
            "/usr/share/applications/steam.desktop",
            "/var/lib/flatpak/exports/share/applications/com.valvesoftware.Steam.desktop",
        };

        for (const QString &desktopPath : desktopPaths) {
            if (QFile::exists(desktopPath)) {
                return true;
            }
        }

        // Check for Steam installation directory
        QString steamHome = QDir::homePath() + "/.local/share/Steam";
        if (QDir(steamHome).exists()) {
            return true;
        }

        return false;
    }

    /**
     * @brief Finds Steam library configuration file
     * @return Path to libraryfolders.vdf if found, empty string otherwise
     * 
     * @details
     * Searches in standard Steam locations first, then falls back
     * to recursive search if not found in standard locations.
     */
    QString findLibraryConfig()
    {
        // Standard Steam configuration paths (prioritized)
        QStringList standardPaths = {
            QDir::homePath() + "/.local/share/Steam/config/libraryfolders.vdf",
            QDir::homePath() + "/.steam/steam/config/libraryfolders.vdf",
            QDir::homePath() + "/.var/app/com.valvesoftware.Steam/data/Steam/config/libraryfolders.vdf",
            "/home/steam/.local/share/Steam/config/libraryfolders.vdf"
        };

        // Check standard paths first
        for (const QString &path : standardPaths) {
            if (QFile::exists(path)) {
                qCDebug(gl) << "Found libraryfolders.vdf in standard location:" << path;
                return path;
            }
        }

        // Fallback: recursive search from Steam home directory
        QString steamHome = QDir::homePath() + "/.local/share/Steam";
        if (QDir(steamHome).exists()) {
            QString foundPath = recursiveFind(QDir(steamHome), 0, 3);
            if (!foundPath.isEmpty()) {
                qCDebug(gl) << "Found libraryfolders.vdf via recursive search:" << foundPath;
                return foundPath;
            }
        }

        // Last resort: recursive search from home directory (limited depth)
        qCDebug(gl) << "Falling back to limited recursive search from home directory";
        return recursiveFind(QDir(QDir::homePath()), 0, 3);
    }

    /**
     * @brief Simple direct parser for Steam libraryfolders.vdf
     * @param steamConfigPath Path to libraryfolders.vdf
     * @return Map of App ID to game name
     * 
     * @details
     * Directly parses the VDF file without complex nesting logic.
     * Extracts library paths and app IDs, then reads appmanifest
     * files to get game names.
     */
    QMap<QString, QString> getGamesDirect(const QString &steamConfigPath)
    {
        QMap<QString, QString> games;
        
        QFile file(steamConfigPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCWarning(gl) << "Failed to open Steam config:" << steamConfigPath;
            return games;
        }
        
        QTextStream in(&file);
        QString currentLibraryPath;
        bool inAppsSection = false;
        int braceDepth = 0;
        int appsBraceDepth = 0;
        
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.trimmed().isEmpty()) continue;
            
            // Track brace depth
            if (line.contains('{')) {
                braceDepth++;
                if (inAppsSection && appsBraceDepth == 0) {
                    appsBraceDepth = braceDepth;
                }
            }
            
            if (line.contains('}')) {
                if (inAppsSection && braceDepth == appsBraceDepth) {
                    inAppsSection = false;
                    appsBraceDepth = 0;
                }
                braceDepth--;
            }
            
            // Look for library path (format: "path"		"/path/to/library")
            if (line.contains("\"path\"\t\t\"")) {
                // Extract the path value
                int startPos = line.indexOf("\"path\"");
                startPos = line.indexOf('\"', startPos + 6); // Skip "path"
                if (startPos != -1) {
                    int endPos = line.indexOf('\"', startPos + 1);
                    if (endPos != -1) {
                        currentLibraryPath = line.mid(startPos + 1, endPos - startPos - 1);
                        qCDebug(gl) << "Found library path:" << currentLibraryPath;
                    }
                }
            }
            
            // Check for apps section
            if (line.contains("\"apps\"")) {
                inAppsSection = true;
                continue;
            }
            
            // Parse app entries when in apps section
            if (inAppsSection && !currentLibraryPath.isEmpty()) {
                // Format: "228980"		"142521834"
                // We're looking for lines that start with a quote and contain two quotes
                line = line.trimmed();
                if (line.startsWith('\"') && line.count('\"') >= 2) {
                    // Extract app ID (first quoted string)
                    int firstQuote = line.indexOf('\"');
                    int secondQuote = line.indexOf('\"', firstQuote + 1);
                    if (firstQuote != -1 && secondQuote != -1) {
                        QString appId = line.mid(firstQuote + 1, secondQuote - firstQuote - 1);
                        
                        // Only process if it looks like a numeric app ID
                        bool isNumeric = false;
                        appId.toInt(&isNumeric);
                        
                        if (isNumeric && !games.contains(appId)) {
                            // Read appmanifest file to get game name
                            QString acfPath = QDir(currentLibraryPath).filePath(
                                QString("steamapps/appmanifest_%1.acf").arg(appId));
                            
                            QFile acfFile(acfPath);
                            if (acfFile.exists() && acfFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                                QTextStream acfIn(&acfFile);
                                QString gameName;
                                
                                while (!acfIn.atEnd()) {
                                    QString acfLine = acfIn.readLine();
                                    // Look for name field (format: "name"		"Game Name")
                                    if (acfLine.contains("\"name\"\t\t\"")) {
                                        int nameStart = acfLine.indexOf('\"', acfLine.indexOf("\"name\"") + 6);
                                        int nameEnd = acfLine.indexOf('\"', nameStart + 1);
                                        if (nameStart != -1 && nameEnd != -1) {
                                            gameName = acfLine.mid(nameStart + 1, nameEnd - nameStart - 1);
                                            break;
                                        }
                                    }
                                }
                                
                                acfFile.close();
                                
                                if (!gameName.isEmpty()) {
                                    games[appId] = gameName;
                                    qCDebug(gl) << "Found game:" << gameName << "(App ID:" << appId << ")";
                                } else {
                                    qCDebug(gl) << "Could not find name for App ID" << appId;
                                }
                            } else {
                                qCDebug(gl) << "Could not open appmanifest for App ID" << appId << "at" << acfPath;
                            }
                        }
                    }
                }
            }
        }

        file.close();
        qCDebug(gl) << "Total games found:" << games.size();
        return games;
    }

    /**
     * @brief Creates button entities for all discovered games
     * @param games Map of App ID to game name
     * 
     * @details
     * Creates a button entity for each game that can be pressed
     * from Home Assistant to launch the game.
     */
    void createGameEntities(const QMap<QString, QString> &games)
    {
        const auto cfg = KSharedConfig::openConfig();
        KConfigGroup grp = cfg->group("steam");
        for (auto it = games.constBegin(); it != games.constEnd(); ++it) {
            const QString &appId = it.key();
            const QString &gameName = it.value();
            if (!grp.readEntry(sanitizeGameName(gameName), false)) continue;
            // Create a button entity for this game
            Button *gameButton = new Button(this);
            gameButton->setId(QString("game_%1").arg(appId));
            gameButton->setName(gameName);
            gameButton->setDiscoveryConfig("icon","mdi:steam");
            
            // Connect the button press to launch the game
            connect(gameButton, &Button::triggered,
                    this, [this, appId]() { onGameButtonPressed(appId); });
            
            m_gameButtons[appId] = gameButton;
            
            qCDebug(gl) << "Created button for game:" << gameName << "(ID:" << appId << ")";
        }
    }

    /**
     * @brief Recursively searches for a file
     * @param dir Directory to search in
     * @param depth Current recursion depth
     * @param maxDepth Maximum recursion depth
     * @return Path to found file, or empty string
     */
    QString recursiveFind(const QDir &dir, int depth, int maxDepth)
    {
        if (depth > maxDepth) return QString();

        QFileInfoList children = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        for (const QFileInfo &fi : children) {
            if (fi.isFile() && fi.fileName() == "libraryfolders.vdf") {
                QString filePath = fi.absoluteFilePath();
                // Simple validation - check if file contains "libraryfolders"
                QFile file(filePath);
                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&file);
                    QString firstLine = in.readLine().trimmed();
                    file.close();
                    if (firstLine.contains("libraryfolders")) {
                        return filePath;
                    }
                }
            } else if (fi.isDir()) {
                // Skip certain directories that are unlikely to contain Steam config
                QString dirName = fi.fileName();
                if (dirName.startsWith(".") || 
                    dirName == "proc" || 
                    dirName == "sys" || 
                    dirName == "dev" ||
                    dirName.contains("wine") ||
                    dirName.contains("proton") ||
                    dirName.contains("dosdevices")) {
                    continue;
                }
                
                QString result = recursiveFind(QDir(fi.absoluteFilePath()), depth + 1, maxDepth);
                if (!result.isEmpty()) return result;
            }
        }
        return QString();
    }

private:
    QMap<QString, Button*> m_gameButtons; /**< @brief Map of App ID to button entities */
};

/**
 * @brief Sets up the GameLauncher integration
 * 
 * @details
 * Factory function called by the integration framework to create and
 * initialize a GameLauncher instance.
 */
void setupGameLauncher()
{
    new GameLauncher(qApp);
}

REGISTER_INTEGRATION("GameLauncher", setupGameLauncher, true)

#include "game_launcher.moc"
