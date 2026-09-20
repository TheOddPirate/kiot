// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include "entity.h"
using KIOTShared::Entities::Entity;
#include <KIOTShared/kiotshared_export.h>
namespace KIOTShared {
namespace Entities {

class KIOT_SHARED_EXPORT Select : public Entity
{
    Q_OBJECT
public:
    explicit Select(QObject *parent = nullptr);

    void setOptions(const QStringList &opts);
    void setState(const QString &state);
    QString state() const;
    QStringList options() const;

protected:
    void init() override;

signals:
    void optionSelected(QString newOption);

private:
    void publishState();

    QString m_state;
    QStringList m_options;
};
}
}