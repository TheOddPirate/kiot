cmake_minimum_required(VERSION 3.31)


set(LIB_TARGET "KIOTShared")

message(STATUS "[CMake] Generating Shared library (.so) for ${LIB_TARGET}")

set(KIOT_SHARED_SRC
    Transport/transportmanager.cpp
    Config/configmanager.cpp
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
    entities/camera.cpp
    entities/update.cpp
    platformhelper.cpp
    dbusproperty.cpp
)

add_library(${LIB_TARGET} SHARED ${KIOT_SHARED_SRC})

target_sources(${LIB_TARGET} PUBLIC
    FILE_SET HEADERS
    BASE_DIRS .
    FILES
        KIOTShared/kiotshared.h
        KIOTShared/kiotshared_export.h
        Plugins/plugininterface.h
        Plugins/uiinterface.h
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

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    Mqtt
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
)

target_link_libraries(${LIB_TARGET}
    PUBLIC
    KF6::ConfigCore
    Qt6::Core
    Qt6::Widgets
    Qt6::Mqtt
    Qt6::DBus
)

target_compile_definitions(${LIB_TARGET} PRIVATE
    PROJECT_NAME="${PROJECT_NAME}"
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_DESCRIPTION="${PROJECT_DESCRIPTION}"
    PROJECT_DOMAIN="${PROJECT_HOMEPAGE_URL}"
    PROJECT_DEVELOPERS="${PROJECT_DEVELOPERS}"
    CURRENT_YEAR="${CURRENT_YEAR}"
    KIOT_SHARED_LIBRARY=1
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

target_include_directories(${LIB_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/KIOTShared>
    $<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/kiotshared>
)

# Installasjonen blir kjempeenkel fordi FILE_SET tar seg av alt!
install(TARGETS ${LIB_TARGET}
    EXPORT "${LIB_TARGET}Targets"
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${KDE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${KDE_INSTALL_LIBDIR}
    FILE_SET HEADERS DESTINATION ${KDE_INSTALL_INCLUDEDIR}/kiotshared
)