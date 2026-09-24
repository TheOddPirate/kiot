cmake_minimum_required(VERSION 3.31)

# ==============================================================================
# 1. Plugin Project Configuration
# ==============================================================================
project(TemplatePlugin 
    VERSION 0.0.1 
    DESCRIPTION "Description of plugin" 
    HOMEPAGE_URL "https://github.com/username/repo_name" 
    LANGUAGES CXX 
)


set(PARENT_APP_NAME "kiot")
set(CMAKE_AUTOMOC ON)
set(QT_MIN_VERSION "6.6.0")
set(KF6_MIN_VERSION "6.15")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Includes the needed libs from the sandbox
include_directories(/app/include/kiotshared/)

# Needed to complete the build
find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")

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
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
    Mqtt
)

find_package(KF6 REQUIRED COMPONENTS Config)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Mqtt
    KF6::ConfigCore
    KIOTShared
)

# ==============================================================================
# 4. Compile Definitions & Metadata
# ==============================================================================
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_NAME="${PARENT_APP_NAME}"
    PLUGIN_NAME="${PROJECT_NAME}"
    PLUGIN_VERSION="${PROJECT_VERSION}"
    PLUGIN_DESCRIPTION="${PROJECT_DESCRIPTION}"
    PLUGIN_DOMAIN="${PROJECT_HOMEPAGE_URL}"
    CURRENT_YEAR="${CURRENT_YEAR}"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

# ==============================================================================
# 5. Installation Rules
# ==============================================================================
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION plugins/${PROJECT_NAME}
)