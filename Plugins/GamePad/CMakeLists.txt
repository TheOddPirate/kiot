cmake_minimum_required(VERSION 3.16)
find_package(PkgConfig REQUIRED)

project(GamePad VERSION 0.0.1 DESCRIPTION "Short Plugin Description" HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

set(CMAKE_AUTOMOC ON)


message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")

add_library(${PROJECT_NAME} MODULE
    gamepadplugin.cpp
    gamepadplugin.h
    gamepad.h
    gamepad.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

pkg_check_modules(LIBUDEV libudev)

include_directories(${LIBUDEV_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    KIOTShared
    ${LIBUDEV_LIBRARIES}
)
link_directories(${LIBUDEV_LIBRARY_DIRS})
add_definitions(${LIBUDEV_CFLAGS_OTHER})

set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLUGIN_NAME=\"${PROJECT_NAME}\"
    PLUGIN_VERSION=\"${PROJECT_VERSION}\"
    PLUGIN_DESCRIPTION=\"${PROJECT_DESCRIPTION}\"
    PLUGIN_DOMAIN=\"${PROJECT_HOMEPAGE_URL}\"
    CURRENT_YEAR=\"${CURRENT_YEAR}\"
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_DATADIR}/${PARENT_APP_NAME}/plugins
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
)