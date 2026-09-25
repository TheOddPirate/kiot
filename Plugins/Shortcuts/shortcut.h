// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QMap>
#include <QStringList>

namespace KIOTShared::Entities {
class Select;
}

class Shortcut : public QObject
{
    Q_OBJECT

public:
    explicit Shortcut(QObject *parent = nullptr);

private Q_SLOTS:
    void onOptionSelected(const QString &newOption);

private:
    struct ShortcutDbus {
        QString componentId;
        QString componentName;
        QString shortcutName;
        QString keyCombo;
    };

    QList<QString> sortAlphabetically(const QList<QString> &input);
    void exposeShortcuts();
    QStringList getGlobalAccelComponents() const;
    void registerShortcuts();

    KIOTShared::Entities::Select *m_shortcutSelect;
    QMap<QString, ShortcutDbus> m_shortcuts;
};

