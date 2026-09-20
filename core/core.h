// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include <KIOTShared/kiotshared.h>
#include <KSharedConfig>
#include <QCoreApplication>
#include <QMqttSubscription>
#include <QObject>
#include <QVariantMap>

// Trekk inn PlatformHelper i globalt scope (eller bruk KIOTShared::PlatformHelper i koden)
using KIOTShared::PlatformHelper;

class QMqttClient;
class ConnectedNode;
class MainWindow;
struct IntegrationFactory {
    QString name;
    std::function<void()> factory;
    bool onByDefault = true; // ny flag for default enabled
};

class HaControl : public QObject
{
    Q_OBJECT
public:
    HaControl();
    ~HaControl();


    static bool registerIntegrationFactory(const QString &name, std::function<void()> plugin, bool onByDefault = true);

private:
    void validateStartup(bool autostart);
    bool validateConfig();
    void loadIntegrations(KSharedConfigPtr config);
    static QList<IntegrationFactory> s_integrations;
    static HaControl *s_self;

    ConnectedNode *m_connectedNode = nullptr;
    MainWindow *m_mainWindow = nullptr;
};

// clang-format off

// Macro for integrations
#define REGISTER_INTEGRATION(nameStr, func, onByDefault) \
static bool dummy##func = HaControl::registerIntegrationFactory(nameStr, [](){ func(); }, onByDefault);

// clang-format on