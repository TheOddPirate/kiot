// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include <KIOTShared/kiotshared.h>
#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include <QFileSystemWatcher>
#include <QProcess>


#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVariantMap>
#include <QTimer>
#include <QApplication>
#include <QByteArray>
#include <QLoggingCategory>
#include <QProcess>
#include <QCollator>
#include <QLocale>

using KIOTShared::PlatformHelper;

struct GameData {
    QString launcher;     // "Steam", "Heroic", "Lutris", osv.
    QString gameId;       // AppID eller intern ID
    QString gameName;     // f.eks. "Skyrim Special Edition"
    QString displayName;
    QString runner;       // f.eks. "Proton GE-Proton9-2", "lutris-f wine"
    
    QString installPath;  // Hvor spillets filer ligger (.exe)
    QString prefixPath;   // Eks: ~/.local/share/steam/steamapps/compatdata/489830/pfx
    QString exePath;      // Hoved-exe fil for spillet
    QString iconPath;    // Sti til ikonet
    QString envVariables;
    QString launchOptions;
};

class GameBase : public QObject
{
    Q_OBJECT

public:
    explicit GameBase(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~GameBase() = default;

    // === Pure Virtuals / Vurderes overstyrt av subklasser ===
    virtual QString launcherName() const = 0;
    virtual bool isLauncherInstalled() const = 0;
    virtual QMap<QString, GameData> scanGames() = 0;

    // Felles funksjon for å hente cachede spill
    QMap<QString, GameData> getGameList() const { return m_games; }


signals:
    void gamesUpdated();

protected:
    QMap<QString, GameData> m_games;
    QFileSystemWatcher m_watcher; // Klonet til hver underklasse for å lytte på relevante konfigurasjonsfiler
};