cmake_minimum_required(VERSION 3.16)

project(NightMode VERSION 0.0.1 DESCRIPTION "Short Plugin Description" HOMEPAGE_URL "https://github.com/davidedmundson/kiot" LANGUAGES CXX )

set(CMAKE_AUTOMOC ON)


message(STATUS "[CMake] Plugin: ${PROJECT_NAME}")
set(DBUS_XML_DIR "${CMAKE_SOURCE_DIR}/dbus")

set(NIGHTMODE_SRC
    nightmodeplugin.cpp
    nightmodeplugin.h
    nightmode.h
    nightmode.cpp
)

set_source_files_properties(
    ${DBUS_XML_DIR}/org.freedesktop.DBus.Properties.xml
    ${DBUS_XML_DIR}/org.kde.KWin.NightLight.xml
    PROPERTIES
    NO_NAMESPACE ON
)

qt_add_dbus_interface(NIGHTMODE_SRC ${DBUS_XML_DIR}/org.freedesktop.DBus.Properties.xml dbusproperties)
qt_add_dbus_interface(NIGHTMODE_SRC ${DBUS_XML_DIR}/org.kde.KWin.NightLight.xml kwinnightlight)

add_library(${PROJECT_NAME} MODULE ${NIGHTMODE_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    KIOTShared

)

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