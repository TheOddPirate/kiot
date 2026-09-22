// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include "gamebase.h"
#include "../helpers/VdfParser.h"

class SteamScanner : public GameBase
{
    Q_OBJECT

public:
    explicit SteamScanner(QObject *parent = nullptr);
    
    QString launcherName() const override { return "Steam"; }
    bool isLauncherInstalled() const override;
    QMap<QString, GameData> scanGames() override;

private:
    QMap<QString, QString> getSteamGames();
    QString findSteamLibraryConfig();
    QString recursiveFind(const QDir &dir, int depth, int maxDepth);
};