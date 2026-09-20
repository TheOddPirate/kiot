cmake_minimum_required(VERSION 3.16)

find_package(PkgConfig REQUIRED)
message(STATUS "[CMake] Building Shared library (.so) for ${PROJECT_NAME}")
include_directories(..)

# Globale definisjoner arves fra roten, så disse kan evt. forenkles, 
# men skader ikke å ha de her for sikkerhets skyld.
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")
add_definitions(-DPROJECT_DESCRIPTION="${PROJECT_DESCRIPTION}")
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")
add_definitions(-DPROJECT_DOMAIN="${PROJECT_HOMEPAGE_URL}")
add_definitions(-DAPP_ID="${APP_ID}")
add_definitions(-DPROJECT_DEVELOPERS="${PROJECT_DEVELOPERS}")

set(KIOT_SHARED_SRC
    kiotshared_export.h
    Plugins/plugininterface.h

    Transport/transportmanager.cpp
    Transport/transportmanager.h
    
    entities/entity.cpp
    entities/binarysensor.cpp
    entities/button.cpp
    entities/number.cpp
    entities/switch.cpp
    entities/event.cpp
    entities/select.cpp
    entities/sensor.cpp
    entities/text.cpp
    entities/lock.cpp
    entities/notify.cpp
    entities/mediaplayer.cpp
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

# 1. Endret fra OBJECT til SHARED her!
add_library(KIOTShared SHARED ${KIOT_SHARED_SRC})

target_link_libraries(KIOTShared
    PUBLIC
    KF6::CoreAddons
    KF6::ConfigCore
    Qt6::Widgets
    Qt6::Mqtt
    Qt6::DBus
)

# Legg til denne linjen! Den forteller preprosessoren at vi bygger selve biblioteket akkurat nå:
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
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 2. Viktig for SHARED biblioteker: Sørg for at den blir installert riktig
install(TARGETS KIOTShared ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})