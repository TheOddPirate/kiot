import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts

ColumnLayout {
    id: root
    anchors.fill: parent
    spacing: 10

    property var settingsManager: null

    QQC2.Label {
        text: "MQTT Connection"
        font.bold: true
        font.pixelSize: 16
        Layout.fillWidth: true
    }

    GridLayout {
        columns: 2
        columnSpacing: 10
        rowSpacing: 10
        Layout.fillWidth: true

        QQC2.Label { text: "Hostname:"; Layout.alignment: Qt.AlignRight }
        QQC2.TextField {
            id: hostField
            Layout.fillWidth: true
            text: root.settingsManager ? root.settingsManager.getHost() : ""
            onEditingFinished: if (root.settingsManager) root.settingsManager.setHost(text)
        }

        QQC2.Label { text: "Port:"; Layout.alignment: Qt.AlignRight }
        QQC2.TextField {
            id: portField
            Layout.fillWidth: true
            text: root.settingsManager ? root.settingsManager.getPort() : 1883
            onEditingFinished: if (root.settingsManager) root.settingsManager.setPort(parseInt(text) || 1883)
        }

        QQC2.Label { text: "Username:"; Layout.alignment: Qt.AlignRight }
        QQC2.TextField {
            id: userField
            Layout.fillWidth: true
            text: root.settingsManager ? root.settingsManager.getUser() : ""
            onEditingFinished: if (root.settingsManager) root.settingsManager.setUser(text)
        }

        QQC2.Label { text: "Password:"; Layout.alignment: Qt.AlignRight }
        QQC2.TextField {
            id: passwordField
            Layout.fillWidth: true
            text: root.settingsManager ? root.settingsManager.getPassword() : ""
            echoMode: TextInput.Password
            onEditingFinished: if (root.settingsManager) root.settingsManager.setPassword(text)
        }

        QQC2.Label { text: "Discovery Prefix:"; Layout.alignment: Qt.AlignRight }
        QQC2.TextField {
            id: discoveryField
            Layout.fillWidth: true
            text: root.settingsManager ? root.settingsManager.getDiscoveryPrefix() : "homeassistant"
            onEditingFinished: if (root.settingsManager) root.settingsManager.setDiscoveryPrefix(text)
        }

        QQC2.Label { text: "Use SSL:"; Layout.alignment: Qt.AlignRight }
        QQC2.CheckBox {
            id: sslCheckbox
            checked: root.settingsManager ? root.settingsManager.getConfigValue("general", "useSSL", false) : false
            onToggled: if (root.settingsManager) root.settingsManager.saveConfigValue("general", "useSSL", checked)
        }

        QQC2.Label { text: "Show system tray:"; Layout.alignment: Qt.AlignRight }
        QQC2.CheckBox {
            id: systrayCheckbox
            checked: root.settingsManager ? root.settingsManager.getConfigValue("general", "systray", true) : true
            onToggled: if (root.settingsManager) root.settingsManager.saveConfigValue("general", "systray", checked)
        }

        QQC2.Label { text: "Autostart:"; Layout.alignment: Qt.AlignRight }
        QQC2.CheckBox {
            id: autostartCheckbox
            checked: root.settingsManager ? root.settingsManager.getConfigValue("general", "autostart", false) : false
            onToggled: if (root.settingsManager) root.settingsManager.saveConfigValue("general", "autostart", checked)
        }
    }

    // ---- SPACER ----
    Item { 
        Layout.fillHeight: true 
        Layout.fillWidth: true
    }

    // ---- Apply and Defaults buttons ----
    RowLayout {
        Layout.fillWidth: true
        spacing: 10
        QQC2.Button {
            text: "Apply"
            Layout.fillWidth: true
            onClicked: if (root.settingsManager) root.settingsManager.applySettings()
        }

        QQC2.Button {
            text: "Defaults"
            Layout.fillWidth: true
            onClicked: if (root.settingsManager) root.settingsManager.restoreDefaults()
        }
    }
}