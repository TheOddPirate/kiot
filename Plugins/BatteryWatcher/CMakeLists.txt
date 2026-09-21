cmake_minimum_required(VERSION 3.16)

project(Battery VERSION 0.0.1 DESCRIPTION "Gives you batttery information of devices connected" HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

set(CMAKE_AUTOMOC ON)


message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")

add_library(Battery MODULE
    batteryPlugin.cpp
    batteryPlugin.h
    batterywatcher.cpp
    batterywatcher.h
)



find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    Solid

)


target_include_directories(Battery PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(Battery PRIVATE
    KIOTShared
    Qt6::Core
    KF6::ConfigCore
    KF6::CoreAddons
    KF6::Solid
)

# Plasser den ferdige .so-filen direkte i plugins-katalogen der Core forventer å finne den
set_target_properties(Battery PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

target_compile_definitions(Battery PRIVATE
    PLUGIN_NAME=\"${PROJECT_NAME}\"
    PLUGIN_VERSION=\"${PROJECT_VERSION}\"
    PLUGIN_DESCRIPTION=\"${PROJECT_DESCRIPTION}\"
    PLUGIN_DOMAIN=\"${PROJECT_HOMEPAGE_URL}\"
    CURRENT_YEAR=\"${CURRENT_YEAR}\"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)


install(TARGETS Battery
    LIBRARY DESTINATION ${CMAKE_INSTALL_DATADIR}/${PARENT_APP_NAME}/plugins
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
)