// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once
#include <QObject>
#include <KIOTShared/kiotshared.h>
using KIOTShared::Entities::BinarySensor;
using KIOTShared::Entities::Switch;

using KIOTShared::PlatformHelper;

#include "dbusproperties.h"
#include "kwinnightlight.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>



class NightMode : public QObject
{
    Q_OBJECT
public:
    NightMode(QObject *parent);
    void updateAttributes();
private:
    BinarySensor *m_sensor;
    Switch *m_switch;
    std::optional<uint32_t> m_inhibitCookie;

    OrgKdeKWinNightLightInterface *m_nightLightIface;
    OrgFreedesktopDBusPropertiesInterface *m_propsIface;
};
