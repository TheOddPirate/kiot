cmake_minimum_required(VERSION 3.16)

project(AppLauncher VERSION 0.0.1 DESCRIPTION "Lets you launch any installed software from home assistant" HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

set(CMAKE_AUTOMOC ON)


message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")

add_library(AppLauncher MODULE
    applauncher.cpp
    applauncher.h
)
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    Service
    KIO
)

target_include_directories(AppLauncher PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(AppLauncher PRIVATE
    KIOTShared
    KF6::CoreAddons
    KF6::Service
    KF6::KIOGui
    KF6::ConfigCore
    Qt6::Core

)

# Plasser den ferdige .so-filen direkte i plugins-katalogen der Core forventer å finne den
set_target_properties(AppLauncher PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

target_compile_definitions(AppLauncher PRIVATE
    PLUGIN_NAME=\"${PROJECT_NAME}\"
    PLUGIN_VERSION=\"${PROJECT_VERSION}\"
    PLUGIN_DESCRIPTION=\"${PROJECT_DESCRIPTION}\"
    PLUGIN_DOMAIN=\"${PROJECT_HOMEPAGE_URL}\"
    CURRENT_YEAR=\"${CURRENT_YEAR}\"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)