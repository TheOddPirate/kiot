cmake_minimum_required(VERSION 3.16)

project(kiot VERSION 0.0.2)
set(PROJECT_VERSION "0.0.2")
set(PROJECT_VERSION_MAJOR 0)
# For automatic firmware updates in connected node
add_definitions(-DKIOT_VERSION="${PROJECT_VERSION}")

set(QT_MIN_VERSION "6.6.0")
set(KF6_MIN_VERSION "6.15")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

# Used for building flatpak
option(BUILD_FLATPAK "Build for Flatpak environment" OFF)

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

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    Mqtt
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    DBusAddons
    Notifications
    IdleTime
    GlobalAccel
    KCMUtils
    I18n
    Solid
    BluezQt
)

find_package(KF6 "1.7.0" REQUIRED COMPONENTS
    PulseAudioQt
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.hpp *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})


set(SOURCES
    main.cpp
    core.cpp
    dbusproperty.cpp
    dbusproperty.h
    entities/entity.cpp
    entities/binarysensor.cpp
    entities/button.cpp
    entities/number.cpp
    entities/switch.cpp
    entities/event.cpp
    entities/select.cpp
    entities/sensor.cpp
    entities/textbox.cpp
    entities/lock.cpp
    entities/mediaplayer.cpp
    entities/notify.cpp
    entities/camera.cpp
    entities/update.cpp
    dbus/policyagentinterface.cpp  # <- DBus proxy C++-fil
)

# Add the path for dbus proxy
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dbus)

# Integrations
add_subdirectory(integrations)
# UI
add_subdirectory(ui)

# And flatpak if we are building for flatpak
if(BUILD_FLATPAK)
    add_subdirectory(flatpak)
endif()

# Hovedexecutabel
add_executable(kiot ${SOURCES})

# Link mot nødvendige biblioteker
target_link_libraries(
    kiot
    Qt6::Gui
    Qt6::Mqtt
    Qt6::DBus
    KIOTIntegrations
    KF6::CoreAddons
    KF6::DBusAddons
)

# Turn of debug output if build as Release
target_compile_definitions(kiot PRIVATE
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

# Install bin
install(TARGETS kiot ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

# Installer .desktop file if not flatpak (that is maintained in the flatpak folder)
if(NOT BUILD_FLATPAK)
    ecm_install_configured_files(INPUT org.davidedmundson.kiot.desktop.in @ONLY DESTINATION ${KDE_INSTALL_AUTOSTARTDIR})
    ecm_install_configured_files(INPUT org.davidedmundson.kiot.desktop.in @ONLY DESTINATION ${KDE_INSTALL_APPDIR})
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
