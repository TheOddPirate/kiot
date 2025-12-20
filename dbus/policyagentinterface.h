#ifndef POLICYAGENTINTERFACE_H
#define POLICYAGENTINTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "policyagentinhibition.h" 


// Forward-deklarasjon hvis du har denne typen
struct PolicyAgentInhibition;

class PolicyAgentInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.kde.Solid.PowerManagement.PolicyAgent"; }

    explicit PolicyAgentInterface(const QString &service,
                                  const QString &path,
                                  const QDBusConnection &connection,
                                  QObject *parent = nullptr);
    ~PolicyAgentInterface();

    Q_PROPERTY(QList<PolicyAgentInhibition> ActiveInhibitions READ activeInhibitions)
    inline QList<PolicyAgentInhibition> activeInhibitions() const
    { return qvariant_cast<QList<PolicyAgentInhibition>>(property("ActiveInhibitions")); }

    Q_PROPERTY(QList<PolicyAgentInhibition> RequestedInhibitions READ requestedInhibitions)
    inline QList<PolicyAgentInhibition> requestedInhibitions() const
    { return qvariant_cast<QList<PolicyAgentInhibition>>(property("RequestedInhibitions")); }

public Q_SLOTS:
    QDBusPendingReply<uint> AddInhibition(uint types, const QString &app_name, const QString &reason)
    {
        QList<QVariant> args{types, app_name, reason};
        return asyncCallWithArgumentList(QStringLiteral("AddInhibition"), args);
    }

    QDBusPendingReply<> ReleaseInhibition(uint cookie)
    {
        QList<QVariant> args{cookie};
        return asyncCallWithArgumentList(QStringLiteral("ReleaseInhibition"), args);
    }

    QDBusPendingReply<bool> HasInhibition(uint types)
    {
        QList<QVariant> args{types};
        return asyncCallWithArgumentList(QStringLiteral("HasInhibition"), args);
    }

    QDBusPendingReply<QList<QStringList>> ListInhibitions()
    {
        QList<QVariant> args;
        return asyncCallWithArgumentList(QStringLiteral("ListInhibitions"), args);
    }

Q_SIGNALS:
    void InhibitionsChanged(const QList<QStringList> &added, const QStringList &removed);
};

#endif
