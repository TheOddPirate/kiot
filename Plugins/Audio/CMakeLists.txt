cmake_minimum_required(VERSION 3.16)

project(Audio VERSION 0.0.1 DESCRIPTION "Lets you control you system audio" HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

set(CMAKE_AUTOMOC ON)


message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")

add_library(Audio MODULE
    audio.cpp
    audio.h
)

find_package(KF6 "1.7.0" REQUIRED COMPONENTS
    PulseAudioQt
)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
)

target_include_directories(Audio PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(Audio PRIVATE
    KIOTShared
    KF6::PulseAudioQt
    Qt6::Core
)

# Plasser den ferdige .so-filen direkte i plugins-katalogen der Core forventer å finne den
set_target_properties(Audio PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

target_compile_definitions(Audio PRIVATE
    PLUGIN_NAME=\"${PROJECT_NAME}\"
    PLUGIN_VERSION=\"${PROJECT_VERSION}\"
    PLUGIN_DESCRIPTION=\"${PROJECT_DESCRIPTION}\"
    PLUGIN_DOMAIN=\"${PROJECT_HOMEPAGE_URL}\"
    CURRENT_YEAR=\"${CURRENT_YEAR}\"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)



install(TARGETS Audio
    LIBRARY DESTINATION ${CMAKE_INSTALL_DATADIR}/${PARENT_APP_NAME}/plugins
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
)