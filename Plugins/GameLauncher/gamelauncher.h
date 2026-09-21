#pragma once

#include <QObject>
#include <KIOTShared/kiotshared.h>
#include "launchers/gamebase.h"

using KIOTShared::Entities::Select;
using KIOTShared::Plugins::KIOTPluginInterface;
class GameLauncherPlugin : public QObject, public KIOTShared::Plugins::KIOTPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KIOTPluginInterface_iid FILE "plugin.json")
    Q_INTERFACES(KIOTShared::Plugins::KIOTPluginInterface)

public:
    GameLauncherPlugin(QObject *parent = nullptr);
    ~GameLauncherPlugin() override;

    QString name() const override;
    QString description() const override;
    QUrl url() const override;
    QVersionNumber version() const override;
    bool checkCompatibility() override;
    
    bool startPlugin() override;
    bool stopPlugin() override;

private slots:
    void onOptionSelected(const QString &option);

private:
    void detectAllGames();
    void createGameEntity();
    void ensureConfig();
    QList<QString> sortAlphabetically(const QList<QString> &input);
    QString sanitizeGameName(const QString &gameName);
    void setToDefault();

    QVector<GameBase*> m_scanners;
    QMap<QString, GameData> m_games;
    Select* m_select;
};