#pragma once
#include <QObject>
#include <KIOTShared/kiotshared.h>
using KIOTShared::Entities::Lock;

class LockedState : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE LockedState(QObject *parent);

private slots:
    void screenLockedChanged(bool active);
    void stateChangeRequested(bool state);

private:
    Lock *m_locked;
};
