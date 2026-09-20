cmake_minimum_required(VERSION 3.16)

project(KDEActiveWindowWatcher VERSION 0.0.1 DESCRIPTION "Makes active window title and attributes available in home assistant" HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")
set(CMAKE_AUTOMOC ON)

set(QT_MIN_VERSION "6.6.0")
set(KF6_MIN_VERSION "6.15")

add_library(KDEActiveWindowWatcher MODULE
    activewindow.cpp
    activewindow.h
)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    Mqtt
)

target_include_directories(KDEActiveWindowWatcher PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(KDEActiveWindowWatcher PRIVATE
    KIOTShared
    Qt6::DBus
    Qt6::Core
    Qt6::Widgets
)
#Adds kwin script as resource
qt_add_resources(INTEGRATIONS_RESOURCES resources.qrc)

# Plasser den ferdige .so-filen direkte i plugins-katalogen der Core forventer å finne den
set_target_properties(KDEActiveWindowWatcher PROPERTIES
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

target_sources(KDEActiveWindowWatcher PRIVATE 
    ${INTEGRATIONS_RESOURCES}
)
