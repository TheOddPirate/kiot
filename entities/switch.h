#pragma once
#include "entity.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>

class Switch : public Entity
{
    Q_OBJECT
public:
    Switch(QObject *parent = nullptr);
    void setState(bool state);
Q_SIGNALS:
    void stateChangeRequested(bool state);
protected:
    void init() override;
private:
    bool m_state = false;
};