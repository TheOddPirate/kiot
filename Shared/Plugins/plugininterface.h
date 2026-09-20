#pragma once
#include <QString>
#include <QVersionNumber>
#include "kiotshared_export.h"

namespace KIOTShared {
namespace Plugins {

class KIOT_SHARED_EXPORT KIOTPluginInterface {
public:
    virtual ~KIOTPluginInterface() = default;

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QVersionNumber version() const = 0;
    virtual bool checkCompatibility() = 0;
    
    virtual bool startPlugin() = 0;
    virtual bool stopPlugin() = 0;
};

} // namespace Plugins
} // namespace KIOTShared

// Bruker APP_ID-makroen kombinert med en unik suffiks for interfacet
#define KIOTPluginInterface_iid (APP_ID ".KIOTPluginInterface")

// Husk det fulle navnet i Q_DECLARE_INTERFACE siden klassen er i namespace
Q_DECLARE_INTERFACE(KIOTShared::Plugins::KIOTPluginInterface, KIOTPluginInterface_iid)