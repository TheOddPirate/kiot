cmake_minimum_required(VERSION 3.16)
message(STATUS "[CMake] Building Qt UI for ${PROJECT_NAME}")


set(QT_MIN_VERSION "6.6.0")
set(KF6_MIN_VERSION "6.15")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMOptionalAddSubdirectory)
include(FeatureSummary)
include(KDEClangFormat)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(ECMQmlModule)
include(ECMConfiguredInstall)

include_directories(../..)
set(KIOT_UIQT_SOURCES
    mainwindow.cpp
    mainwindow.h
    settingsmanager.cpp
    settingsmanager.h
)


find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    Mqtt
    Quick
    QuickWidgets
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
)
add_library(KIOT_UIQT STATIC ${KIOT_UIQT_SOURCES})

target_link_libraries(KIOT_UIQT PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Mqtt
    KIOTShared
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