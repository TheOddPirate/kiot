#pragma once
#include "entity.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>

class Event : public Entity
{
    Q_OBJECT
public:
    Event(QObject *parent = nullptr);
    void trigger();
protected:
    void init() override;
};