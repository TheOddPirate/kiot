#include "policyagentinterface.h"

PolicyAgentInterface::PolicyAgentInterface(const QString &service,
                                           const QString &path,
                                           const QDBusConnection &connection,
                                           QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

PolicyAgentInterface::~PolicyAgentInterface()
{
}
