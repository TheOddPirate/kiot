// SPDX-FileCopyrightText: 2025 David Edmundson <davidedmundson@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QVariant>
#include "KIOTShared/kiotshared_export.h"

namespace KIOTShared {

class KIOT_SHARED_EXPORT DBusProperty : public QObject
{
    Q_OBJECT
public:
    explicit DBusProperty(const QString &service, const QString &path, const QString &interface, const QString &property, QObject *parent = nullptr);
    QVariant value() const;
Q_SIGNALS:
    void valueChanged(const QVariant &value);
private Q_SLOTS:
    void onFdoPropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated);

private:
    QString m_service;
    QString m_path;
    QString m_interface;
    QString m_property;
    QVariant m_value;
};

} // namespace KIOTShared