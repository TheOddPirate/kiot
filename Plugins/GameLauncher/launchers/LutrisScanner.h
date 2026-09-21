// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef LUTRISSCANNER_H
#define LUTRISSCANNER_H

#include "gamebase.h"
#include <QList>


class LutrisScanner : public GameBase{
public:
    explicit LutrisScanner(QObject *parent = nullptr);

    QString launcherName() const override { return "Lutris"; }
    bool isLauncherInstalled() const override;
    QMap<QString, GameData> scanGames() override;

private:
    struct YamlInfo {
        QString gameId;
        QString gameName;
        QString prefixPath;
        QString runner;
        QString exepath;
        QString args;
    };
    QMap<QString, YamlInfo> getLutrisGames();
    YamlInfo parseYamlFile(const QString &yamlPath);
};

#endif // LUTRISSCANNER_H