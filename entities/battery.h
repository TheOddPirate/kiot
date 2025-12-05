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

    void setState(const int &state);
    int getState();
    void setAttributes(const QVariantMap &attrs);
    QVariantMap getAttributes();
protected:
    void init() override;

private:
    int m_state;
    QVariantMap m_attributes;

    void publishState();
    void publishAttributes();
};