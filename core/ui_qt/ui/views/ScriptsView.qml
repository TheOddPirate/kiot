import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts
import QtQuick.Dialogs

Flickable {
    id: root
    contentWidth: width
    contentHeight: contentColumn.height
    clip: true

    property var settingsManager: null
    property string section: "Scripts"

    // --- INTERN DIALOG: NYTT SKRIPT ---
    QQC2.Dialog {
        id: newScriptDialog
        title: "Create New Script"
        standardButtons: QQC2.Dialog.Ok | QQC2.Dialog.Cancel
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            GridLayout {
                columns: 2
                columnSpacing: 10
                rowSpacing: 10
                
                QQC2.Label { text: "Script ID:" }
                QQC2.TextField {
                    id: newScriptNameField
                    placeholderText: "e.g., browser, steam, custom"
                    Layout.fillWidth: true
                }
                
                QQC2.Label { text: "Display Name:" }
                QQC2.TextField {
                    id: newScriptDisplayNameField
                    placeholderText: "e.g., Launch Browser, Open Steam"
                    Layout.fillWidth: true
                }
                
                QQC2.Label { text: "Command:" }
                QQC2.TextField {
                    id: newScriptCommandField
                    placeholderText: "e.g., /usr/bin/brave, steam steam://..."
                    Layout.fillWidth: true
                }
                
                QQC2.Label { text: "Icon:" }
                QQC2.TextField {
                    id: newScriptIconField
                    placeholderText: "e.g., mdi:web, mdi:steam"
                    text: "mdi:script-text"
                    Layout.fillWidth: true
                }
            }
        }
        
        onAccepted: {
            if (newScriptNameField.text.trim() !== "" && settingsManager) {
                var scriptId = newScriptNameField.text.trim()
                var displayName = newScriptDisplayNameField.text.trim() || scriptId
                var command = newScriptCommandField.text.trim()
                var icon = newScriptIconField.text.trim() || "mdi:script-text"
                
                settingsManager.saveNestedConfigValue("Scripts", scriptId, "Name", displayName)
                settingsManager.saveNestedConfigValue("Scripts", scriptId, "Exec", command)
                settingsManager.saveNestedConfigValue("Scripts", scriptId, "icon", icon)
                
                newScriptNameField.text = ""
                newScriptDisplayNameField.text = ""
                newScriptCommandField.text = ""
                newScriptIconField.text = "mdi:script-text"
            }
        }
        onRejected: {
            newScriptNameField.text = ""
            newScriptDisplayNameField.text = ""
            newScriptCommandField.text = ""
            newScriptIconField.text = "mdi:script-text"
        }
    }
    
    // --- INTERN DIALOG: SLETT SKRIPT ---
    QQC2.Dialog {
        id: deleteScriptDialog
        title: "Delete Script"
        standardButtons: QQC2.Dialog.Yes | QQC2.Dialog.No
        property string scriptId: ""
    
        ColumnLayout {
            QQC2.Label {
                text: "Are you sure you want to delete this script?"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        
            QQC2.Label {
                text: deleteScriptDialog.scriptId ? "Script: " + deleteScriptDialog.scriptId : ""
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }
    
        onAccepted: {
            if (deleteScriptDialog.scriptId && settingsManager) {
                var parts = deleteScriptDialog.scriptId.split("/")
                if (parts.length > 1) {
                    settingsManager.deleteNestedConfig(parts[0], parts[1])
                } else {
                    settingsManager.deleteNestedConfig("Scripts", deleteScriptDialog.scriptId)
                }
            }
            deleteScriptDialog.scriptId = ""
        }
        onRejected: {
            deleteScriptDialog.scriptId = ""
        }
    }

    // --- HOVEDINNHOLD ---
    ColumnLayout {
        id: contentColumn
        width: parent.width
        spacing: 10
        
        QQC2.Label {
            text: "Scripts"
            font.bold: true
            font.pixelSize: 16
            Layout.fillWidth: true
        }
        
        QQC2.Button {
            text: "Create New Script"
            onClicked: newScriptDialog.open()
            Layout.fillWidth: true
        }
        
        property var scriptKeys: {
    if (!settingsManager || !settingsManager.configSections) return []
    var sectionData = settingsManager.configSections[section]
    if (!sectionData) return []
    var keys = Object.keys(sectionData)
    return keys.sort()
}
        
        Connections {
            target: settingsManager
            enabled: settingsManager !== null
            
            function onConfigSectionsChanged() {
                contentColumn.scriptKeys = contentColumn.scriptKeys
            }
        }
        
        Repeater {
            model: contentColumn.scriptKeys
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5
                
                property string scriptId: modelData
                property var scriptData: {
    if (!settingsManager || !settingsManager.configSections) return {}
    var sectionData = settingsManager.configSections[section]
    if (!sectionData || !scriptId || !sectionData[scriptId]) return {}
    return sectionData[scriptId]
}
                
                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "lightgray"
                    visible: model.index > 0
                }
                
                QQC2.Label {
                    text: scriptId
                    font.bold: true
                    Layout.fillWidth: true
                }
                
                QQC2.Button {
                    text: "Delete"
                    onClicked: {
                        deleteScriptDialog.scriptId = scriptId
                        deleteScriptDialog.open()
                    }
                    Layout.alignment: Qt.AlignRight
                }
                
                GridLayout {
                    columns: 2
                    columnSpacing: 10
                    rowSpacing: 10
                    Layout.fillWidth: true
                    
                    QQC2.Label { text: "Name:"; Layout.alignment: Qt.AlignRight }
                    QQC2.TextField {
                        text: scriptData["Name"] || ""
                        onEditingFinished: {
                            var parts = scriptId.split("/")
                            if (parts.length > 1 && settingsManager) {
                                settingsManager.saveNestedConfigValue(parts[0], parts[1], "Name", text)
                            } else if (settingsManager) {
                                settingsManager.saveNestedConfigValue("Scripts", scriptId, "Name", text)
                            }
                        }
                        Layout.fillWidth: true
                    }
                    
                    QQC2.Label { text: "Command:"; Layout.alignment: Qt.AlignRight }
                    QQC2.TextField {
                        text: scriptData["Exec"] || ""
                        onEditingFinished: {
                            var parts = scriptId.split("/")
                            if (parts.length > 1 && settingsManager) {
                                settingsManager.saveNestedConfigValue(parts[0], parts[1], "Exec", text)
                            } else if (settingsManager) {
                                settingsManager.saveNestedConfigValue("Scripts", scriptId, "Exec", text)
                            }
                        }
                        Layout.fillWidth: true
                    }
                    
                    QQC2.Label { text: "Icon:"; Layout.alignment: Qt.AlignRight }
                    QQC2.TextField {
                        text: scriptData["icon"] || ""
                        onEditingFinished: {
                            var parts = scriptId.split("/")
                            if (parts.length > 1 && settingsManager) {
                                settingsManager.saveNestedConfigValue(parts[0], parts[1], "icon", text)
                            } else if (settingsManager) {
                                settingsManager.saveNestedConfigValue("Scripts", scriptId, "icon", text)
                            }
                        }
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}