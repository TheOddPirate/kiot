import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts

Flickable {
    id: root
    contentWidth: width
    contentHeight: contentColumn.height
    clip: true

    property var settingsManager: null
    property string section: ""

    ColumnLayout {
        id: contentColumn
        width: parent.width
        spacing: 10
        
        property var sectionKeys: {
            var sectionData = (root.settingsManager && root.section) ? root.settingsManager.configSections[root.section] : {}
            if (!sectionData) return []
            return Object.keys(sectionData).sort()
        }
        
        Connections {
            target: root.settingsManager
            enabled: root.settingsManager !== null
            
            function onConfigSectionsChanged() {
                contentColumn.sectionKeys = contentColumn.sectionKeys
            }
        }
        
        Repeater {
            model: contentColumn.sectionKeys
            
            Item {
                Layout.fillWidth: true
                implicitHeight: childrenRect.height
                
                property string configKey: modelData
                property var configValue: {
                    var sectionData = (root.settingsManager && root.section) ? root.settingsManager.configSections[root.section] : {}
                    return sectionData ? sectionData[configKey] : undefined
                }
                
                QQC2.CheckBox {
                    visible: typeof configValue === "boolean"
                    width: parent.width
                    
                    text: {
                        var displayKey = configKey
                        displayKey = displayKey.replace(/_/g, " ")
                        displayKey = displayKey.replace(/\b\w/g, function(l) { return l.toUpperCase() })
                        return displayKey
                    }
                    checked: typeof configValue === "boolean" ? configValue : false
                    onToggled: if (root.settingsManager) root.settingsManager.saveConfigValue(root.section, configKey, checked)
                }
                
                GridLayout {
                    visible: typeof configValue !== "boolean"
                    columns: 2
                    columnSpacing: 10
                    width: parent.width
                    
                    QQC2.Label {
                        text: {
                            var displayKey = configKey
                            displayKey = displayKey.replace(/_/g, " ")
                            displayKey.replace(/\b\w/g, function(l) { return l.toUpperCase() })
                            return displayKey + ":"
                        }
                        Layout.alignment: Qt.AlignRight
                    }
                    
                    QQC2.TextField {
                        Layout.fillWidth: true
                        text: configValue ? configValue.toString() : ""
                        onEditingFinished: if (root.settingsManager) root.settingsManager.saveConfigValue(root.section, configKey, text)
                    }
                }
            }
        }
    }
}