cmake_minimum_required(VERSION 3.16)

project(Active VERSION 0.0.1 DESCRIPTION "Shows user activity" HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")

set(CMAKE_AUTOMOC ON)

add_library(Active MODULE
    active.cpp
    active.h
)

target_include_directories(Active PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(Active PRIVATE
    KIOTShared
    KF6::IdleTime
)

set_target_properties(Active PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)
target_compile_definitions(Active PRIVATE
    PLUGIN_NAME=\"${PROJECT_NAME}\"
    PLUGIN_VERSION=\"${PROJECT_VERSION}\"
    PLUGIN_DESCRIPTION=\"${PROJECT_DESCRIPTION}\"
    PLUGIN_DOMAIN=\"${PROJECT_HOMEPAGE_URL}\"
    CURRENT_YEAR=\"${CURRENT_YEAR}\"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)