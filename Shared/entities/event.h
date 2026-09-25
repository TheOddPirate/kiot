// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include "entity.h"
using KIOTShared::Entities::Entity;
#include "KIOTShared/kiotshared_export.h"
namespace KIOTShared {
namespace Entities {

class KIOT_SHARED_EXPORT Event : public Entity
{
    Q_OBJECT
public:
    Event(QObject *parent = nullptr);
    void trigger();

protected:
    void init() override;
};

}
}