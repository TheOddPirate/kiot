cmake_minimum_required(VERSION 3.16)

project(Bluetooth VERSION 0.0.1 DESCRIPTION Plugins/Bluetooth/plugin.json HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

set(CMAKE_AUTOMOC ON)


message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")

add_library(Bluetooth MODULE
    bluetoothadapter.cpp
    bluetoothadapter.h
    bluetoothdeviceswitch.cpp
    bluetoothdeviceswitch.h
    bluetoothplugin.cpp
    bluetoothplugin.h
)

target_include_directories(Bluetooth PRIVATE ${CMAKE_SOURCE_DIR})


find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    BluezQt
)


target_include_directories(Bluetooth PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(Bluetooth PRIVATE
    KIOTShared
    Qt6::Core
    KF6::ConfigCore
    KF6::CoreAddons
    KF6::BluezQt
)


set_target_properties(Bluetooth PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

target_compile_definitions(Bluetooth PRIVATE
    PLUGIN_NAME=\"${PROJECT_NAME}\"
    PLUGIN_VERSION=\"${PROJECT_VERSION}\"
    PLUGIN_DESCRIPTION=\"${PROJECT_DESCRIPTION}\"
    PLUGIN_DOMAIN=\"${PROJECT_HOMEPAGE_URL}\"
    CURRENT_YEAR=\"${CURRENT_YEAR}\"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)


install(TARGETS Bluetooth
    LIBRARY DESTINATION ${CMAKE_INSTALL_DATADIR}/${PARENT_APP_NAME}/plugins
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
)