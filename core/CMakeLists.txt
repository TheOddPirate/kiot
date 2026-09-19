cmake_minimum_required(VERSION 3.16)
message(STATUS "[CMake] Building Core for ${PROJECT_NAME}")
include_directories(../)
# Kildekilder for selve kjerne-executabelen
set(KIOT_CORE_SOURCES
    main.cpp
    core.cpp
    logging/messagehandler.cpp
    logging/messagehandler.h
    startup/startupmanager.cpp
    startup/startupmanager.h
    startup/systemdmanager.cpp
    startup/systemdmanager.h
    startup/desktopmanager.cpp
    startup/desktopmanager.h    
    startup/backgroundmanager.cpp
    startup/backgroundmanager.h    
)

# Dbus interfaces used by core
set_source_files_properties(
    ../dbus/org.freedesktop.portal.Background.xml
    PROPERTIES
    NO_NAMESPACE ON
)
qt_add_dbus_interface(KIOT_CORE_SOURCES ../dbus/org.freedesktop.portal.Background.xml backgroundportal)

# Core libraries used in final result
set(KIOT_PARTS
    KIOTShared
    KIOTIntegrations
)

if(BUILD_QTUI AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ui_qt/CMakeLists.txt")
    add_subdirectory(ui_qt)
    list(APPEND KIOT_PARTS KIOT_UIQT)
endif()

if(BUILD_APPDATA AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/appdata/CMakeLists.txt")
    add_subdirectory(appdata)
endif()

message(STATUS "[CMake] Building Core bin for ${PROJECT_NAME}")
add_executable(${PROJECT_NAME} ${KIOT_CORE_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${KIOT_PARTS}
    Qt6::Gui
    Qt6::Mqtt
    Qt6::DBus
    Qt6::Quick
    Qt6::QuickWidgets
    KF6::CoreAddons
    KF6::DBusAddons
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_NAME="${PROJECT_NAME}"
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_DESCRIPTION="${PROJECT_DESCRIPTION}"
    PROJECT_DOMAIN="${PROJECT_HOMEPAGE_URL}"
    PROJECT_DEVELOPERS="${PROJECT_DEVELOPERS}"
    APP_ID="${APP_ID}"
    CURRENT_YEAR="${CURRENT_YEAR}"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

install(TARGETS ${PROJECT_NAME} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})