// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "shortcut.h"

#include <KIOTShared/kiotshared.h>

using KIOTShared::Entities::Event;
using KIOTShared::Entities::Select;
using KIOTShared::PlatformHelper;

#include <KConfigGroup>
#include <KGlobalAccel>
#include <KSharedConfig>

#include <QAction>
#include <QTimer>
#include <QApplication>
#include <QStringList>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QString>
#include <QCollator>
#include <algorithm>
#include <QLocale>

DEFINE_LOGGER(shortcut, Integrations.Shortcut)

Shortcut::Shortcut(QObject *parent)
    : QObject(parent)
    , m_shortcutSelect(nullptr)
{
    registerShortcuts();
    exposeShortcuts();
}

void Shortcut::onOptionSelected(const QString &newOption)
{
    if (newOption == "Default" || !m_shortcutSelect) {
        return;
    }
    if (!m_shortcuts.contains(newOption))
        return;
    auto sc = m_shortcuts[newOption];
    qCDebug(shortcut) << "Executing shortcut" << sc.shortcutName << "from component" << sc.componentName;
    
    // Execute shortcut via DBus
    QDBusInterface kglobalaccel("org.kde.kglobalaccel", 
                                 QString(sc.componentName), 
                                 "org.kde.kglobalaccel.Component",
                                 QDBusConnection::sessionBus());
    
    if (kglobalaccel.isValid()) {
        QDBusReply<void> reply = kglobalaccel.call("invokeShortcut", sc.shortcutName);
        if (reply.isValid()) {
            qCDebug(shortcut) << "Successfully executed shortcut:" << sc.shortcutName << "from component" << sc.componentName;
        } else {
            qCDebug(shortcut) << "Failed to execute shortcut:" << sc.shortcutName << "from component" << sc.componentName;
        }
    } else {
        qCWarning(shortcut) << "Could not connect to org.kde.kglobalaccel";
    }

    // Return to default
    if (m_shortcutSelect) {
        QTimer::singleShot(100, this, [this]() {
            m_shortcutSelect->setState("Default");
        });
    }
}

QList<QString> Shortcut::sortAlphabetically(const QList<QString> &input)
{
    QList<QString> sorted = input;

    QCollator collator(QLocale::system());
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setNumericMode(true);

    std::sort(sorted.begin(), sorted.end(),
              [&collator](const QString &a, const QString &b) {
                  return collator.compare(a, b) < 0;
              });

    return sorted;
}

void Shortcut::exposeShortcuts()
{
    m_shortcutSelect = new Select(this);
    m_shortcutSelect->setId("shortcuts");
    m_shortcutSelect->setName("Shortcuts");
    
    QStringList shortcutIds;

    // Try to get shortcuts from all available components
    QStringList components = getGlobalAccelComponents();
    for (QString component : components) {
        QDBusInterface componentInterface("org.kde.kglobalaccel", 
                                       QStringLiteral("%1").arg(component), 
                                   "org.kde.kglobalaccel.Component",
                                  QDBusConnection::sessionBus());
        if (componentInterface.isValid()) {
            QDBusReply<QStringList> componentReply = componentInterface.call("shortcutNames");
            if (componentReply.isValid()) {
                QStringList componentShortcuts = componentReply.value();
                for (const QString &shortcut : componentShortcuts) {
                    ShortcutDbus dd;
                    dd.componentName = component.contains("/component/") ? component : "/component/" + component;
                    dd.shortcutName = shortcut;
                    dd.componentId = component.replace("/component/","") + " - " + shortcut;
                    m_shortcuts[dd.componentId] = dd;
                    shortcutIds.append(QString("%1").arg(dd.componentId));
                }
                qCDebug(shortcut) << "Found" << componentShortcuts.size() << "shortcuts in component" << component;
            }
        }
    }
    shortcutIds = sortAlphabetically(shortcutIds);
    shortcutIds.prepend("Default");
    m_shortcutSelect->setOptions(shortcutIds);
    m_shortcutSelect->setState("Default");
    
    connect(m_shortcutSelect, &Select::optionSelected, this, &Shortcut::onOptionSelected);
    
    qCInfo(shortcut) << "Exposed" << shortcutIds.size() << "shortcuts in select entity";
}

QStringList Shortcut::getGlobalAccelComponents() const
{
    QStringList components;
    
    QDBusInterface kglobalaccel("org.kde.kglobalaccel", 
                                 "/kglobalaccel", 
                                 "org.kde.KGlobalAccel",
                                 QDBusConnection::sessionBus());
    
    if (kglobalaccel.isValid()) {
        QDBusReply<QList<QDBusObjectPath>> reply = kglobalaccel.call("allComponents");
        if (reply.isValid()) {
            for (const QDBusObjectPath& path : reply.value()) 
            {
                if(path.path().isEmpty()) continue;
                components.append(path.path());
            }
        }
    }
       
    if (components.isEmpty()) {
         components = {"kwin", "krunner", "plasmashell", "org.kde.kglobalaccel"};
    }
    
    return components;
}

void Shortcut::registerShortcuts()
{
    auto shortcutConfigToplevel = KSharedConfig::openConfig(PlatformHelper::configFilePath(), KConfig::SimpleConfig)->group("Shortcuts");
    const QStringList shortcutIds = shortcutConfigToplevel.groupList();
    for (const QString &shortcutId : shortcutIds) {
        auto shortcutConfig = shortcutConfigToplevel.group(shortcutId);
        const QString name = shortcutConfig.readEntry("Name", shortcutId);
        QAction *action = new QAction(name, qApp);
        action->setObjectName(shortcutId);

        auto event = new Event(qApp);
        event->setId(shortcutId);
        event->setName(name);

        KGlobalAccel::self()->setShortcut(action, {});
        QObject::connect(action, &QAction::triggered, event, &Event::trigger);
    }
    
    if (shortcutIds.length() >= 1) {
        qCInfo(shortcut) << "Registered" << shortcutIds.length() << "custom shortcuts:" << shortcutIds.join(", ");
    }
}

