#pragma once
#include "entity.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>


class Button : public Entity
{
    Q_OBJECT
public:
    Button(QObject *parent = nullptr);
Q_SIGNALS:
    void triggered();
protected:
    void init() override;

};