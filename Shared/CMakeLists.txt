cmake_minimum_required(VERSION 3.16)

message(STATUS "[CMake] Building Shared library (.so) for ${PROJECT_NAME}")

set(KIOT_SHARED_SRC
    kiotshared_export.h
    kiotshared.h
    Plugins/plugininterface.h

    Transport/transportmanager.cpp
    Transport/transportmanager.h
    
    entities/entity.cpp
    entities/entity.h
    entities/binarysensor.cpp
    entities/binarysensor.h
    entities/button.cpp
    entities/button.h
    entities/number.cpp
    entities/number.h
    entities/switch.cpp
    entities/switch.h
    entities/event.cpp
    entities/event.h
    entities/select.cpp
    entities/select.h
    entities/sensor.cpp
    entities/sensor.h
    entities/text.cpp
    entities/text.h
    entities/lock.cpp
    entities/lock.h
    entities/notify.cpp
    entities/notify.h
    entities/mediaplayer.cpp
    entities/mediaplayer.h
    entities/image.cpp
    entities/image.h 
    entities/camera.cpp
    entities/camera.h 
    entities/update.cpp
    entities/update.h 

    platformhelper.cpp
    platformhelper.h
    dbusproperty.cpp
    dbusproperty.h
)

# Bygger som dynamisk bibliotek (.so)
add_library(KIOTShared SHARED ${KIOT_SHARED_SRC})

target_link_libraries(KIOTShared
    PUBLIC
    KF6::CoreAddons
    KF6::ConfigCore
    Qt6::Widgets
    Qt6::Mqtt
    Qt6::DBus
)

target_compile_definitions(KIOTShared PRIVATE
    PROJECT_NAME="${PROJECT_NAME}"
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_DESCRIPTION="${PROJECT_DESCRIPTION}"
    PROJECT_DOMAIN="${PROJECT_HOMEPAGE_URL}"
    PROJECT_DEVELOPERS="${PROJECT_DEVELOPERS}"
    APP_ID="${APP_ID}"
    CURRENT_YEAR="${CURRENT_YEAR}"
    KIOT_SHARED_LIBRARY=1
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

target_include_directories(KIOTShared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/kiotshared>
)

# Sørg for installasjon av target og eventuelle public headers
install(TARGETS KIOTShared ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})