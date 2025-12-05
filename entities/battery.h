#pragma once
#include "entity.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>



class Battery : public Entity
{
    Q_OBJECT
public:
    Battery(QObject *parent = nullptr);

    void setState(const QString &state);
    void setAttributes(const QVariantMap &attrs);

protected:
    void init() override;

private:
    QString m_state;
    QVariantMap m_attributes;

    void publishState();
    void publishAttributes();
};