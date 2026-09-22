cmake_minimum_required(VERSION 3.16)

project(Docker VERSION 0.0.1 DESCRIPTION "Lets you controll local docker images from home assistant" HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

set(CMAKE_AUTOMOC ON)


message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")

add_library(${PROJECT_NAME} MODULE
    dockerplugin.cpp
    dockerplugin.h
    dockereventlistener.cpp
    dockereventlistener.h
    dockerswitch.h
    dockerswitch.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})


find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Network
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    KIOTShared
    Qt6::Core
    Qt6::Network
    KF6::CoreAddons
    KF6::ConfigCore

)

# Plasser den ferdige .so-filen direkte i plugins-katalogen der Core forventer å finne den
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLUGIN_NAME="${PROJECT_NAME}"
    PLUGIN_VERSION="${PROJECT_VERSION}"
    PLUGIN_DESCRIPTION="${PROJECT_DESCRIPTION}"
    PLUGIN_DOMAIN="${PROJECT_HOMEPAGE_URL}"
    CURRENT_YEAR="${CURRENT_YEAR}"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_DATADIR}/${PARENT_APP_NAME}/plugins
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
)