#pragma once
#include <QString>
#include <QUrl>
#include <QVersionNumber>
#include "KIOTShared/kiotshared_export.h"

namespace KIOTShared {
namespace Plugins {

class KIOT_SHARED_EXPORT KIOTUiInterface {
public:
    virtual ~KIOTUiInterface() = default;

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QVersionNumber version() const = 0;
    virtual QUrl url() const = 0;
    
    virtual bool checkCompatibility() = 0;

    virtual bool showUI() = 0;
    virtual bool hideUI() = 0;
};

} // namespace Plugins
} // namespace KIOTShared

#define KIOTUiInterface_iid "org.kiot.UiInterface/1.0" // Bruk gjerne en fast streng for testing, eller APP_ID hvis den er definert

// Pakk klassenavnet inn i ekstra parenteser for å unngå at MOC feiltolker navnerommet
Q_DECLARE_INTERFACE(KIOTShared::Plugins::KIOTUiInterface, KIOTUiInterface_iid)