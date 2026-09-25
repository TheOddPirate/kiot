// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include "entity.h"

#include "KIOTShared/kiotshared_export.h"
 using KIOTShared::Entities::Entity;
namespace KIOTShared {
namespace Entities {

class KIOT_SHARED_EXPORT Lock : public Entity
{
    Q_OBJECT
public:
    Lock(QObject *parent = nullptr);
    void setState(bool state);

    Q_SIGNALS:
    void stateChangeRequested(bool state);

protected:
    void init() override;

private:
    bool m_state = false;

};
}
}