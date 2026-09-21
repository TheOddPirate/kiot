// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include "gamebase.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
class FaugusScanner : public GameBase
{
    Q_OBJECT

public:
    explicit FaugusScanner(QObject *parent = nullptr);
    
    QString launcherName() const override { return "Faugus"; }
    bool isLauncherInstalled() const override;
    QMap<QString, GameData> scanGames() override;

private:
    QString m_configPath;

};