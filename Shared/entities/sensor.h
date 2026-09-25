// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include "entity.h"
using KIOTShared::Entities::Entity;
#include "kiotshared_export.h"
namespace KIOTShared {
namespace Entities {

class KIOT_SHARED_EXPORT Sensor : public Entity
{
    Q_OBJECT
public:
    Sensor(QObject *parent = nullptr);

    void setState(const QString &state);
    QString state()
    {
        return m_state;
    };

protected:
    void init() override;

private:
    QString m_state;

    void publishState();
};
}
}