cmake_minimum_required(VERSION 3.16)

message(STATUS "[CMake] Building Shared library (.so) for ${PROJECT_NAME}")

set(KIOT_SHARED_SRC
  #  kiotshared_export.h
    KIOTShared/kiotshared.h
    KIOTShared/kiotshared_export.h
    Plugins/plugininterface.h
    Transport/transportmanager.cpp
    Transport/transportmanager.h
    Config/configmanager.cpp
    Config/configmanager.h
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

set(LIB_HEADERS 
    KIOTShared/kiotshared.h
    KIOTShared/kiotshared_export.h
    Plugins/plugininterface.h
    Transport/transportmanager.h
    Config/configmanager.h
    entities/entity.h
    entities/binarysensor.h
    entities/button.h
    entities/number.h
    entities/switch.h
    entities/event.h
    entities/select.h
    entities/sensor.h
    entities/text.h
    entities/lock.h
    entities/notify.h
    entities/mediaplayer.h
    entities/image.h 
    entities/camera.h 
    entities/update.h 

    platformhelper.h
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
set_target_properties(KIOTShared PROPERTIES PUBLIC_HEADER "${LIB_HEADERS}")
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/KIOTShared> # Sørg for at undermapper funker i build
    $<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/kiotshared>
)

#install(TARGETS KIOTShared ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(TARGETS KIOTShared
    EXPORT "KIOTSharedTargets"
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${KDE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${KDE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${KDE_INSTALL_INCLUDEDIR}/kiotshared
)