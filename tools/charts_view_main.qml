import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

import jsonqml
import Qt.jsonqml.qobjectPreferences 1.0

pragma ComponentBehavior: Bound

ApplicationWindow {
    id: appWindow
    title: qsTr("Charts QML")
    width: 640
    height: 480
    visible: true
    readonly property int margin: 10
    property int filetype: 0

    Component.onCompleted: {
    }

    function toCharts() {
        tabBar.setCurrentIndex(1)
    }
    function toSettings() {
        tabBar.setCurrentIndex(2)
    }

    minimumWidth: mainLayout.Layout.minimumWidth + 2 * margin
    minimumHeight: mainLayout.Layout.minimumHeight + 2 * margin

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: appWindow.margin

        TabBar {
            id: tabBar
            Layout.fillWidth: true
            contentHeight : 40

            TabButton {
                text: "Data"
            }

            TabButton {
                text: "Charts"
            }

            TabButton {
                text: "Preferences"
                onClicked: {  }
            }

        }

        StackLayout {
            id: formStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            SelectTableView {
                id: dataForm
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Item {
                id: chartForm
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Item {
                id: settingsForm
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

        }

    }

    footer: Label {
       id: error_label
       text: Preferences.error
       color: "red"
    }

    FolderDialog {
        id: folderDialog
        title: qsTr("Choose a folder with schemas")
        currentFolder: Preferences.workDir
        onAccepted: Preferences.changeScemasPath(selectedFolder)
    }

}
