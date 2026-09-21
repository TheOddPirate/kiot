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
    property string section: "CustomSensors"

    // --- INTERN DIALOG: NYTT CUSTOM SENSOR ---
    QQC2.Dialog {
        id: newCustomSensorDialog
        title: "Create New Custom Sensor"
        standardButtons: QQC2.Dialog.Ok | QQC2.Dialog.Cancel
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            GridLayout {
                columns: 2
                columnSpacing: 10
                rowSpacing: 10
                
                QQC2.Label { text: "Sensor ID:" }
                QQC2.TextField {
                    id: newSensorIdField
                    placeholderText: "e.g., gpu_power, custom_metric"
                    Layout.fillWidth: true
                }
                
                QQC2.Label { text: "Display Name:" }
                QQC2.TextField {
                    id: newSensorNameField
                    placeholderText: "e.g., Gpu power draw"
                    Layout.fillWidth: true
                }
                
                QQC2.Label { text: "Command:" }
                QQC2.TextField {
                    id: newSensorCommandField
                    placeholderText: "e.g., nvidia-smi ..."
                    Layout.fillWidth: true
                }
                
                QQC2.Label { text: "Interval:" }
                QQC2.TextField {
                    id: newSensorIntervalField
                    placeholderText: "e.g., 60s, 10s"
                    text: "60s"
                    Layout.fillWidth: true
                }
                
                QQC2.Label { text: "Unit:" }
                QQC2.TextField {
                    id: newSensorUnitField
                    placeholderText: "e.g., W, C, %, GiB"
                    Layout.fillWidth: true
                }
                
                QQC2.Label { text: "Device Class:" }
                QQC2.TextField {
                    id: newSensorDeviceClassField
                    placeholderText: "e.g., power, temperature"
                    Layout.fillWidth: true
                }
                
                QQC2.Label { text: "State Class:" }
                QQC2.TextField {
                    id: newSensorStateClassField
                    placeholderText: "e.g., measurement"
                    text: "measurement"
                    Layout.fillWidth: true
                }
            }
        }
        
        onAccepted: {
            if (newSensorIdField.text.trim() !== "" && settingsManager) {
                var sensorId = newSensorIdField.text.trim()
                var name = newSensorNameField.text.trim() || sensorId
                var command = newSensorCommandField.text.trim()
                var interval = newSensorIntervalField.text.trim() || "60s"
                var unit = newSensorUnitField.text.trim()
                var deviceClass = newSensorDeviceClassField.text.trim()
                var stateClass = newSensorStateClassField.text.trim()
                
                settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "name", name)
                settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "command", command)
                settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "interval", interval)
                if (unit) settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "unit_of_measurement", unit)
                if (deviceClass) settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "device_class", deviceClass)
                if (stateClass) settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "state_class", stateClass)
                
                newSensorIdField.text = ""
                newSensorNameField.text = ""
                newSensorCommandField.text = ""
                newSensorIntervalField.text = "60s"
                newSensorUnitField.text = ""
                newSensorDeviceClassField.text = ""
                newSensorStateClassField.text = "measurement"
            }
        }
        onRejected: {
            newSensorIdField.text = ""
            newSensorNameField.text = ""
            newSensorCommandField.text = ""
            newSensorIntervalField.text = "60s"
            newSensorUnitField.text = ""
            newSensorDeviceClassField.text = ""
            newSensorStateClassField.text = "measurement"
        }
    }

    // --- INTERN DIALOG: SLETT CUSTOM SENSOR ---
    QQC2.Dialog {
        id: deleteCustomSensorDialog
        title: "Delete Custom Sensor"
        standardButtons: QQC2.Dialog.Yes | QQC2.Dialog.No
        property string sensorId: ""
    
        ColumnLayout {
            QQC2.Label {
                text: "Are you sure you want to delete this custom sensor?"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        
            QQC2.Label {
                text: deleteCustomSensorDialog.sensorId ? "Sensor: " + deleteCustomSensorDialog.sensorId : ""
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }
    
        onAccepted: {
            if (deleteCustomSensorDialog.sensorId && settingsManager) {
                settingsManager.deleteNestedConfig("CustomSensors", deleteCustomSensorDialog.sensorId)
            }
            deleteCustomSensorDialog.sensorId = ""
        }
        onRejected: {
            deleteCustomSensorDialog.sensorId = ""
        }
    }

    // --- HOVEDINNHOLD ---
    ColumnLayout {
        id: contentColumn
        width: parent.width
        spacing: 10
        
        QQC2.Label {
            text: "Custom Sensors"
            font.bold: true
            font.pixelSize: 16
            Layout.fillWidth: true
        }
        
        QQC2.Button {
            text: "Create New Sensor"
            onClicked: newCustomSensorDialog.open()
            Layout.fillWidth: true
        }
        
        property var sensorKeys: {
            var sectionData = settingsManager ? settingsManager.configSections[section] : {}
            if (!sectionData) return []
            return Object.keys(sectionData).sort()
        }
        
        Connections {
            target: settingsManager
            enabled: settingsManager !== null
            
            function onConfigSectionsChanged() {
                contentColumn.sensorKeys = contentColumn.sensorKeys
            }
        }
        
        Repeater {
            model: contentColumn.sensorKeys
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5
                
                property string sensorId: modelData
                property var sensorData: {
                    var sectionData = settingsManager ? settingsManager.configSections[section] : {}
                    if (!sectionData || !sensorId) return {}
                    return sectionData[sensorId] ? sectionData[sensorId] : {}
                }
                
                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "lightgray"
                    visible: model.index > 0
                }
                
                QQC2.Label {
                    text: sensorId
                    font.bold: true
                    Layout.fillWidth: true
                }
                
                QQC2.Button {
                    text: "Delete"
                    onClicked: {
                        deleteCustomSensorDialog.sensorId = sensorId
                        deleteCustomSensorDialog.open()
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
                        text: sensorData["name"] || ""
                        onEditingFinished: {
                            if (settingsManager) {
                                settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "name", text)
                            }
                        }
                        Layout.fillWidth: true
                    }
                    
                    QQC2.Label { text: "Command:"; Layout.alignment: Qt.AlignRight }
                    QQC2.TextField {
                        text: sensorData["command"] || ""
                        onEditingFinished: {
                            if (settingsManager) {
                                settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "command", text)
                            }
                        }
                        Layout.fillWidth: true
                    }
                    
                    QQC2.Label { text: "Interval:"; Layout.alignment: Qt.AlignRight }
                    QQC2.TextField {
                        text: sensorData["interval"] || ""
                        onEditingFinished: {
                            if (settingsManager) {
                                settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "interval", text)
                            }
                        }
                        Layout.fillWidth: true
                    }
                    
                    QQC2.Label { text: "Unit:"; Layout.alignment: Qt.AlignRight }
                    QQC2.TextField {
                        text: sensorData["unit_of_measurement"] || ""
                        onEditingFinished: {
                            if (settingsManager) {
                                settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "unit_of_measurement", text)
                            }
                        }
                        Layout.fillWidth: true
                    }
                    
                    QQC2.Label { text: "Device Class:"; Layout.alignment: Qt.AlignRight }
                    QQC2.TextField {
                        text: sensorData["device_class"] || ""
                        onEditingFinished: {
                            if (settingsManager) {
                                settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "device_class", text)
                            }
                        }
                        Layout.fillWidth: true
                    }
                    
                    QQC2.Label { text: "State Class:"; Layout.alignment: Qt.AlignRight }
                    QQC2.TextField {
                        text: sensorData["state_class"] || ""
                        onEditingFinished: {
                            if (settingsManager) {
                                settingsManager.saveNestedConfigValue("CustomSensors", sensorId, "state_class", text)
                            }
                        }
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}