cmake_minimum_required(VERSION 3.31)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)

set(LIB_TARGET "KIOTShared")

message(STATUS "[CMake] Generating Shared library (.so) for ${LIB_TARGET}")

# ==============================================================================
# 1. Target Sources & Library Definition
# ==============================================================================
# Explicit list of all source and header files as they reside on disk
set(KIOT_SHARED_SRCS
    Transport/transportmanager.cpp Transport/transportmanager.h
    Config/configmanager.cpp Config/configmanager.h
    entities/entity.cpp entities/entity.h
    entities/binarysensor.cpp entities/binarysensor.h
    entities/button.cpp entities/button.h
    entities/number.cpp entities/number.h
    entities/switch.cpp entities/switch.h
    entities/event.cpp entities/event.h
    entities/select.cpp entities/select.h
    entities/sensor.cpp entities/sensor.h
    entities/text.cpp entities/text.h
    entities/lock.cpp entities/lock.h
    entities/notify.cpp entities/notify.h
    entities/mediaplayer.cpp entities/mediaplayer.h
    entities/image.cpp entities/image.h
    entities/camera.cpp entities/camera.h
    entities/update.cpp entities/update.h
    platformhelper.cpp platformhelper.h
    dbusproperty.cpp dbusproperty.h
    KIOTShared/kiotshared.h
    Plugins/plugininterface.h
    Plugins/uiinterface.h
)

# Build the shared library target
add_library(${LIB_TARGET} SHARED ${KIOT_SHARED_SRCS})

# Automatically generate the export macro header (e.g., kiotshared_export.h)
generate_export_header(${LIB_TARGET} BASE_NAME ${LIB_TARGET})

# ==============================================================================
# 2. Dependencies & Linking
# ==============================================================================
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    Mqtt
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
)

# Link required Qt and KF6 modules publicly so consuming plugins inherit them
target_link_libraries(${LIB_TARGET}
    PUBLIC
    KF6::ConfigCore
    Qt6::Core
    Qt6::Widgets
    Qt6::Mqtt
    Qt6::DBus
)

# ==============================================================================
# 3. Compile Definitions & Include Directories
# ==============================================================================
target_compile_definitions(${LIB_TARGET} PRIVATE
    PROJECT_NAME="${PROJECT_NAME}"
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_DESCRIPTION="${PROJECT_DESCRIPTION}"
    PROJECT_DOMAIN="${PROJECT_HOMEPAGE_URL}"
    PROJECT_DEVELOPERS="${PROJECT_DEVELOPERS}"
    CURRENT_YEAR="${CURRENT_YEAR}"
    KIOT_SHARED_LIBRARY=1
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

# Configure include paths for both build-tree usage and installation layout
target_include_directories(${LIB_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/KIOTShared>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/kiotshared>
)

# ==============================================================================
# 4. Target Properties & Aliasing
# ==============================================================================
set_target_properties(${LIB_TARGET} PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "1"
    EXPORT_NAME KIOTShared
)

# Create a modern namespaced alias for internal and external consumption
add_library(KIOTShared::KIOTShared ALIAS ${LIB_TARGET})

# ==============================================================================
# 5. Installation Rules & Package Export Configuration
# ==============================================================================
set(KIOT_CMAKECONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/KIOTShared")

# Install the shared library binaries and archives
install(TARGETS ${LIB_TARGET}
    EXPORT KIOTSharedTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install header subdirectories while maintaining their internal directory structure
install(DIRECTORY KIOTShared Plugins Transport Config entities
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kiotshared
    FILES_MATCHING PATTERN "*.h"
)

# Install loose root-level header files (like platformhelper.h) optionally
install(FILES
    platformhelper.h
    dbusproperty.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kiotshared
    OPTIONAL
)

# Export targets file so find_package(KIOTShared) works correctly
install(EXPORT KIOTSharedTargets
    FILE KIOTSharedTargets.cmake
    NAMESPACE KIOTShared::
    DESTINATION ${KIOT_CMAKECONFIG_DESTINATION}
)

# Generate and install CMake package version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/KIOTSharedConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Generate and install CMake package configuration file
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/KIOTSharedConfig.cmake" "
include(\"\${CMAKE_CURRENT_LIST_DIR}/KIOTSharedTargets.cmake\")
_check_required_components(KIOTShared)
")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KIOTSharedConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KIOTSharedConfigVersion.cmake"
    DESTINATION ${KIOT_CMAKECONFIG_DESTINATION}
)