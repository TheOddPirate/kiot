#pragma once

#include <QtCore/qglobal.h>

#if defined(KIOT_SHARED_LIBRARY)
#  define KIOT_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define KIOT_SHARED_EXPORT Q_DECL_IMPORT
#endif