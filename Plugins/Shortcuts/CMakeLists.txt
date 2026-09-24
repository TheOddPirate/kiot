cmake_minimum_required(VERSION 3.31)

project(Shortcuts VERSION 0.0.1 DESCRIPTION "Lets you trigger automation from shortcuts and trigger every global shortcut" HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

set(CMAKE_AUTOMOC ON)


message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")

add_library(${PROJECT_NAME} MODULE
    shortcutplugin.cpp
    shortcutplugin.h
    shortcut.h
    shortcut.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})


find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    DBusAddons
    GlobalAccel

)

target_link_libraries(${PROJECT_NAME} PRIVATE
    KIOTShared
    KF6::GlobalAccel
    KF6::ConfigCore
    KF6::CoreAddons
    Qt6::DBus
)

# Plasser den ferdige .so-filen direkte i plugins-katalogen der Core forventer å finne den
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLUGIN_NAME=\"${PROJECT_NAME}\"
    PLUGIN_VERSION=\"${PROJECT_VERSION}\"
    PLUGIN_DESCRIPTION=\"${PROJECT_DESCRIPTION}\"
    PLUGIN_DOMAIN=\"${PROJECT_HOMEPAGE_URL}\"
    CURRENT_YEAR=\"${CURRENT_YEAR}\"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_DATADIR}/${PARENT_APP_NAME}/plugins
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
)