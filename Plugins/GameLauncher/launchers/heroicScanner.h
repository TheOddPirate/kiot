// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once
#include "gamebase.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
class HeroicScanner : public GameBase
{
    Q_OBJECT

public:
    explicit HeroicScanner(QObject *parent = nullptr);
    
    QString launcherName() const override { return "Heroic"; }
    bool isLauncherInstalled() const override;
    QMap<QString, GameData> scanGames() override;

private:
    QString m_configPath;
    QMap<QString, GameData> getHeroicGames();
    QString readGameCOnfig(const QString &appName);
    QMap<QString, GameData> getEpicGames(const QString &filePath);
    QMap<QString, GameData> getGogGames(const QString &filePath);
    QMap<QString, GameData> getPrimeGames(const QString &filePath);
    QMap<QString, GameData> getSideloadGames(const QString &filePath);
};