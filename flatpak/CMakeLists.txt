install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.davidedmundson.kiot.svg
  DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps
)

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.davidedmundson.kiot.metainfo.xml
    DESTINATION ${KDE_INSTALL_METAINFODIR}
)

# Install Flatpak-specific desktop file for autostart
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.davidedmundson.kiot.desktop
    DESTINATION ${KDE_INSTALL_AUTOSTARTDIR}
)

# Also install it in applications directory for manual launching
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.davidedmundson.kiot.desktop
    DESTINATION ${KDE_INSTALL_APPDIR}
)

