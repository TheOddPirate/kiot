// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include "entity.h"
using KIOTShared::Entities::Entity;
#include "kiotshared_export.h"

namespace KIOTShared {
namespace Entities {

class KIOT_SHARED_EXPORT  Button : public Entity
{
    Q_OBJECT
public:
    Button(QObject *parent = nullptr);
Q_SIGNALS:
    void triggered();

protected:
    void init() override;
};
}
}