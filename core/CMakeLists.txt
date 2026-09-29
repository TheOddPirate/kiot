cmake_minimum_required(VERSION 3.31)


project(${PARENT_APP_NAME} VERSION ${PARENT_PROJECT_VERSION}  DESCRIPTION "${PARENT_PROJECT_DESCRIPTION}" HOMEPAGE_URL "${PARENT_PROJECT_DOMAIN}"  LANGUAGES CXX)



message(STATUS "[CMake] Building Core for ${PROJECT_NAME}")
include_directories(../)

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
    plugins/pluginmanager.cpp
    plugins/pluginmanager.h
)


# ------------------------------------------------------------------------------
# Language support for non english speakers and what not?
# ------------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core LinguistTools)

# What languages to have as default (add more if you want to support more languages)
# (see https://doc.qt.io/qt-6/linguist-manager.html#languages for more info)
set(TS_FILES 
    translations/kiot_en.ts
    translations/kiot_nb.ts
    translations/kiot_de.ts
    translations/kiot_fr.ts
    translations/kiot_es.ts
    translations/kiot_zh_CN.ts
)
qt_add_translations(${PROJECT_NAME}
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX "/translations"
    SOURCES ${KIOT_CORE_SOURCES}
)

# ------------------------------------------------------------------------------
# DBus header generation
# ------------------------------------------------------------------------------

set(DBUS_XML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dbus")
# Dbus interfaces used by core
set_source_files_properties(
    ${DBUS_XML_DIR}/org.freedesktop.portal.Background.xml
    PROPERTIES
    NO_NAMESPACE ON
)
qt_add_dbus_interface(KIOT_CORE_SOURCES ${DBUS_XML_DIR}/org.freedesktop.portal.Background.xml backgroundportal)

# ==============================================================================
# Dependencies
# ==============================================================================
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    DBus
    Mqtt
    Quick
    QuickWidgets
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    DBusAddons
)

# Core libraries used in final result
set(KIOT_PARTS
    KIOTShared
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
    Qt6::QuickWidgets
    KF6::CoreAddons
    KF6::DBusAddons
)
# ==============================================================================
# Compile Definitions & Metadata
# ==============================================================================
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


# ==============================================================================
# Installation Rules
# ==============================================================================

install(TARGETS ${PROJECT_NAME} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})