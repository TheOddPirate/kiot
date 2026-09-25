cmake_minimum_required(VERSION 3.31)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
set(LIB_TARGET "KIOTShared")

message(STATUS "[CMake] Generating Shared library (.so) for ${LIB_TARGET}")

# Eksplisitt liste over alle kilder og headere slik de faktisk ligger på disk
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

add_library(${LIB_TARGET} SHARED ${KIOT_SHARED_SRCS})

generate_export_header(${LIB_TARGET} BASE_NAME ${LIB_TARGET})


find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    Mqtt
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
)

target_link_libraries(${LIB_TARGET}
    PUBLIC
    KF6::ConfigCore
    Qt6::Core
    Qt6::Widgets
    Qt6::Mqtt
    Qt6::DBus
)

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

target_include_directories(${LIB_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/KIOTShared>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/kiotshared>
)

set_target_properties(${LIB_TARGET} PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "1"
    EXPORT_NAME KIOTShared
)

add_library(KIOTShared::KIOTShared ALIAS ${LIB_TARGET})


# ==============================================================================
# Installasjon (Speiler disken 1:1 uten magi eller generering)
# ==============================================================================

set(KIOT_CMAKECONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/KIOTShared")

install(TARGETS ${LIB_TARGET}
    EXPORT KIOTSharedTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Installer alle undermapper direkte med mappestrukturen intakt fra disk
install(DIRECTORY KIOTShared Plugins Transport Config entities
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kiotshared
    FILES_MATCHING PATTERN "*.h"
)

# Installer eventuelle løse headerfiler som ligger i roten på disk
install(FILES
    platformhelper.h
    dbusproperty.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kiotshared
    OPTIONAL
)

# Eksporter CMake-targets
install(EXPORT KIOTSharedTargets
    FILE KIOTSharedTargets.cmake
    NAMESPACE KIOTShared::
    DESTINATION ${KIOT_CMAKECONFIG_DESTINATION}
)

# Generer og installer Config-filer
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/KIOTSharedConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/KIOTSharedConfig.cmake" "
include(\"\${CMAKE_CURRENT_LIST_DIR}/KIOTSharedTargets.cmake\")
_check_required_components(KIOTShared)
")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KIOTSharedConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KIOTSharedConfigVersion.cmake"
    DESTINATION ${KIOT_CMAKECONFIG_DESTINATION}
)