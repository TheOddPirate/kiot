import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts
import QtQuick.Dialogs

import "views"

QQC2.Page  {
    id: root
    clip: true
    anchors.fill: parent
    height: 600
    width: 800
    property var cppData: settingsManager  


    // Main layout
    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        
        // Tab selector
        QQC2.TabBar {
            id: tabBar
            Layout.fillWidth: true
            
            Repeater {
                model: settingsManager ? settingsManager.sectionOrder : []
                
                QQC2.TabButton {
                    text: {
                        var section = modelData
                        if (section === "general") return "General"
                        if (section === "Integrations") return "Integrations"
                        if (section === "Scripts") return "Scripts"
                        if (section === "Shortcuts") return "Shortcuts"
                        if (section === "CustomSensors") return "CustomSensors"
                        if (section === "docker") return "Docker"
                        if (section === "heroic") return "Heroic Games"
                        if (section === "steam") return "Steam"
                        if (section === "systemd") return "Systemd Services"
                        return section.charAt(0).toUpperCase() + section.slice(1)
                    }
                }
            }
        }
        
        // Content area
        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex
            
            Repeater {
                model: settingsManager ? settingsManager.sectionOrder : []
                
                Loader {
                    property string section: modelData
    
                    source: {
                        if (section === "general") {
                            return "views/GeneralSettingsView.qml"
                        } else if (section === "Scripts") {
                            return "views/ScriptsView.qml"
                        } else if (section === "Shortcuts") {
                            return "views/ShortcutsView.qml"
                        } else if (section === "CustomSensors") {
                            return "views/CustomSensorsView.qml"
                        } else {
                            return "views/GenericSettingsView.qml"
                        }
                    }
    
                    onLoaded: {
                        if (item) {
                            item.settingsManager = cppData
                        if (item.hasOwnProperty("section")) {
                            item.section = section // Setter riktig seksjonsnavn her!
                        }
                    }
                }
            }
            }
        }
    }  
}