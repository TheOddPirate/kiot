// SPDX-FileCopyrightText: David Edmundson <davidedmundson@kde.org>
// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QStringList>
#include <QMap>
#include <kscreen/types.h>

namespace KIOTShared::Entities {
class Switch;
class Select;
class Number;
}

/**
 * @class ScreenController
 * @brief Display controller integration for KIOT managing KScreen outputs, resolution, and brightness.
 */
class ScreenController : public QObject
{
    Q_OBJECT

public:
    explicit ScreenController(QObject *parent = nullptr);
    ~ScreenController() override = default;

    QStringList screenNames() const;

Q_SIGNALS:
    void screenNamesChanged();
    void lastErrorChanged();

private Q_SLOTS:
    void onOptionSelected(const QString &option);
    void onResolutionSelected(const QString &option);
    void onScreenNamesChanged();
    void onPriorityChanged();

private:
    void setup();
    void loadConfigFromSystem();
    void updateState();
    void saveConfig();
    void refreshScreenNames();

    KIOTShared::Entities::Select *m_select;
    KIOTShared::Entities::Select *m_selectResolution;
    KIOTShared::Entities::Switch *m_switchEnabled;
    KIOTShared::Entities::Switch *m_switchPrimary;
    KIOTShared::Entities::Number *m_brightness;
    KIOTShared::Entities::Number *m_zoomScale;
    
    KScreen::OutputPtr m_activeoutput;
    KScreen::ConfigPtr m_config;
    QList<int> m_screenIds;
    QStringList m_screenNames;
    QMap<QString, QString> m_resolutionMap;
};