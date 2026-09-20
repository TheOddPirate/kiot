cmake_minimum_required(VERSION 3.16)

project(AccentColourWatcher VERSION 0.0.1 DESCRIPTION "Exposes template info and more" HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")

set(CMAKE_AUTOMOC ON)

add_library(AccentColourWatcher MODULE
    accentcolour.cpp
    accentcolour.h
)

target_include_directories(AccentColourWatcher PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(AccentColourWatcher PRIVATE
    KIOTShared
    KF6::IdleTime
)

# Plasser den ferdige .so-filen direkte i plugins-katalogen der Core forventer å finne den
set_target_properties(AccentColourWatcher PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

target_compile_definitions(AccentColourWatcher PRIVATE
    PLUGIN_NAME=\"${PROJECT_NAME}\"
    PLUGIN_VERSION=\"${PROJECT_VERSION}\"
    PLUGIN_DESCRIPTION=\"${PROJECT_DESCRIPTION}\"
    PLUGIN_DOMAIN=\"${PROJECT_HOMEPAGE_URL}\"
    CURRENT_YEAR=\"${CURRENT_YEAR}\"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)