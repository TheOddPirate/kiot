cmake_minimum_required(VERSION 3.16)
message(STATUS "[CMake] Building Qt UI for ${PROJECT_NAME}")

include_directories(../..)
set(KIOT_UIQT_SOURCES
    mainwindow.cpp
    mainwindow.h
    settingsmanager.cpp
    settingsmanager.h
)

add_library(KIOT_UIQT STATIC ${KIOT_UIQT_SOURCES})

target_link_libraries(KIOT_UIQT PRIVATE
    KIOTShared
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Mqtt
    KF6::ConfigCore
    KF6::CoreAddons
)
qt_policy(SET QTP0004 NEW)
qt_add_qml_module(KIOT_UIQT
    URI main_qt.window
    RESOURCE_PREFIX /imports
    VERSION 1.0
    QML_FILES
        ui/main.qml
        ui/views/ShortcutsView.qml
        ui/views/ScriptsView.qml
        ui/views/CustomSensorsView.qml
        ui/views/GeneralSettingsView.qml
        ui/views/GenericSettingsView.qml
        
    RESOURCES
        resources.qrc
)

target_include_directories(KIOT_UIQT PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)