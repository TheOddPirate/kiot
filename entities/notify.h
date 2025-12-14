// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include "entity.h"


class Notify : public Entity
{
    Q_OBJECT
public:
    Notify(QObject *parent = nullptr);
    QString mapMdiToTheme(QString mdi);
protected:
    void init() override;
    
Q_SIGNALS:
    void notificationReceived(const QString &message);

};

