cmake_minimum_required(VERSION 3.31)

project(TemplatePlugin 
    VERSION 0.0.1 
    DESCRIPTION "Template Plugin for KIOT" 
    HOMEPAGE_URL "https://github.com/davidedmundson/kiot" 
    LANGUAGES CXX 
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opsjon for å styre om vi bygger for Flatpak-sandboxen
option(BUILD_FLATPAK_EXTENSION "Build as a Flatpak extension module" OFF)

message(STATUS "[CMake] Plugin: ${PROJECT_NAME} | Flatpak Mode: ${BUILD_FLATPAK_EXTENSION}")

# ==============================================================================
# 1. Stier og avhengigheter basert på byggemodus
# ==============================================================================
if(BUILD_FLATPAK_EXTENSION)
    # Hardcoded path inside the sandbox for sharedlib
    include_directories(/app/include/kiotshared/)
    
    # Make sure this follows the root cmaakelists version
    set(KF6_MIN_VERSION "6.15")
    # Make sure this follows the root cmaakelists version
    set(QT_MIN_VERSION "6.6.0")
    find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
else()
    # Make sure this follows the root cmaakelists version
    set(KF6_MIN_VERSION "6.15")
    # Make sure this follows the root cmaakelists version
    set(QT_MIN_VERSION "6.6.0")
    find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

endif()

# ==============================================================================
# 2. Target Definition & Sources
# ==============================================================================
add_library(${PROJECT_NAME} MODULE
    template.cpp
    template.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

# ==============================================================================
# 3. Dependencies
# ==============================================================================
# Needed for KIOTShared
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS 
    Core
    Widgets
    Mqtt
)

# Optinonal dependencies
find_package(KF6 ${KF6_MIN_VERSION} COMPONENTS 
    ConfigCore 
    QUIET
)
#Finds our Shared lib on disk
find_package(KIOTShared REQUIRED)
# Bygg opp target link-biblioteker dynamisk basert på hva som fantst
set(PLUGIN_LINK_LIBS 
    KIOTShared 
    Qt6::Core
    Qt6::Widgets
    Qt6::Mqtt    
)


if(TARGET KF6::ConfigCore)
    list(APPEND PLUGIN_LINK_LIBS KF6::ConfigCore)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${PLUGIN_LINK_LIBS})

# ==============================================================================
# 4. Compile Definitions & Metadata
# ==============================================================================
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLUGIN_NAME="${PROJECT_NAME}"
    PLUGIN_VERSION="${PROJECT_VERSION}"
    PLUGIN_DESCRIPTION="${PROJECT_DESCRIPTION}"
    PLUGIN_DOMAIN="${PROJECT_HOMEPAGE_URL}"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

# ==============================================================================
# 5. Smart Installation Rules
# ==============================================================================
if(BUILD_FLATPAK_EXTENSION)
    # Flatpak forventer at prefix er /app, og installerer til plugins-mappen der
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION plugins/${PROJECT_NAME}
    )
else()
    # Native installering på host-systemet (f.eks. Arch / Debian)
    set(PARENT_APP_NAME "kiot" CACHE STRING "Parent app name")
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_DATADIR}/${PARENT_APP_NAME}/plugins
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    
    # Sørg for at den også spytter ut i lokal bin-mappe under lokal test-bygg
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
    )
endif()