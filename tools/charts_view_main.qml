import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts 2.15

import jsonqml
import Qt.jsonqml.qobjectPreferences 1.0

pragma ComponentBehavior: Bound

ApplicationWindow {
    id: appWindow
    title: qsTr("Charts QML")
    width: 640
    height: 520
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

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                icon.source: "qrc:/qt/qml/jsonqml/qml/images/ShowFilesIcon24.png"
                onClicked: fileOpenDialog.open()
            }
            ToolButton {
                icon.source: "qrc:/qt/qml/jsonqml/qml/images/SaveCurrentRecordIcon24.png"
                onClicked: fileSaveDialog.open()
            }
            Label {
                text: client.csvfile
                elide: Label.ElideLeft
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

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

            EditTableView {
                id: dataForm
                Layout.fillHeight: true
                Layout.fillWidth: true

                keysModel: client.csvmodel
                sortingEnabled: false //client.sortingEnabled
                chartEnabled: true
            }

            ChartsSeriesView {
                id: chartForm
                Layout.fillHeight: true
                Layout.fillWidth: true


                //legendData: client.legendModel.lineData(0)
            }

            ChartsPreferences {
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

    FileDialog {
        visible: false
        id: fileOpenDialog
        title: qsTr("Choose a CSV file")
        fileMode: FileDialog.OpenFile
        nameFilters: [qsTr("CSV files (*.csv)")]
        currentFolder: Preferences.workDir
        onAccepted:  client.readCSV(selectedFile)
    }

    FileDialog {
        visible: false
        id: fileSaveDialog
        title: qsTr("Save a CSV file")
        fileMode: FileDialog.SaveFile
        nameFilters: [qsTr("CSV files (*.csv)")]
        currentFolder: Preferences.workDir
        onAccepted: client.saveCSV(selectedFile)
    }

}
