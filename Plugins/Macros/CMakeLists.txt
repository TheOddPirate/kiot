cmake_minimum_required(VERSION 3.31)
find_package(PkgConfig REQUIRED)

# ==============================================================================
# Plugin Project Configuration
# ==============================================================================
project(Macros 
    VERSION 0.0.1 
    DESCRIPTION "A Macro plugin, creates a virtual keyboard you can control from Home Assistant" 
    HOMEPAGE_URL "https://github.com/davidedmundson/kiot" 
    LANGUAGES CXX 
)

# Enable automatic Qt MOC (Meta-Object Compiler) processing
set(CMAKE_AUTOMOC ON)

# Optional: Uncomment to override the global Qt minimum version requirement
# set(QT_MIN_VERSION "6.6.0")

message(STATUS "[CMake] Configuring Plugin Module: ${PROJECT_NAME}")

# ==============================================================================
# Target Definition & Sources
# ==============================================================================
add_library(${PROJECT_NAME} MODULE
    macroplugin.cpp
    macroplugin.h
    virtualkeyboarddevice.cpp
    virtualkeyboarddevice.h
)

# Include current directory for local header resolution
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

# ==============================================================================
# Dependencies
# ==============================================================================
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
)
pkg_check_modules(XKB REQUIRED xkbcommon)

set(XKBCOMMON_LIBRARIES ${XKB_LIBRARIES})
set(XKBCOMMON_INCLUDE_DIRS ${XKB_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    KIOTShared
    Qt6::Core
    ${XKBCOMMON_LIBRARIES}
)
target_include_directories(${PROJECT_NAME} PRIVATE ${XKBCOMMON_INCLUDE_DIRS})
# ==============================================================================
# Build Properties & Output Directories
# ==============================================================================
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

# Inject project metadata as preprocessor definitions for runtime retrieval
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PARENT_APP_NAME="${PARENT_APP_NAME}"
    PLUGIN_NAME="${PROJECT_NAME}"
    PLUGIN_VERSION="${PROJECT_VERSION}"
    PLUGIN_DESCRIPTION="${PROJECT_DESCRIPTION}"
    PLUGIN_DOMAIN="${PROJECT_HOMEPAGE_URL}"
    CURRENT_YEAR="${CURRENT_YEAR}"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)


# ==============================================================================
# Installation Rules
# ==============================================================================
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_DATADIR}/${PARENT_APP_NAME}/plugins
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
)