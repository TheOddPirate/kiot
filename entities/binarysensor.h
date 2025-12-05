#pragma once
#include "entity.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>
class BinarySensor : public Entity
{
    Q_OBJECT
public:
    BinarySensor(QObject *parent = nullptr);
    void setState(bool state);
    bool state() const;
protected:
    void init() override;
private:
    void publish();
    bool m_state = false;
};
