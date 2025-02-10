import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

import jsonqml
import Qt.jsonqml.qobjectPreferences 1.0

pragma ComponentBehavior: Bound

ApplicationWindow {
    id: appWindow
    title: qsTr("Json QML")
    width: 640
    height: 480
    visible: true
    readonly property int margin: 10
    property int filetype: 0

    Component.onCompleted: {

    }

    minimumWidth: mainLayout.Layout.minimumWidth + 2 * margin
    minimumHeight: mainLayout.Layout.minimumHeight + 2 * margin

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: appWindow.margin

        Rectangle {
            id: rowBox
            Layout.minimumWidth: rowLayout.Layout.minimumWidth + 2 * margin
            Layout.margins : appWindow.margin
            Layout.fillWidth: true
            height: loadButton.height + 2 * margin
            color: "#FFFFFF"
            border.width: 1
            border.color: "#D0D0D0"

            RowLayout {
                id: rowLayout
                anchors.fill: parent
                anchors.margins: appWindow.margin
                spacing: 10

                Button {
                    id: loadButton
                    icon.source: "qrc:/qt/qml/jsonqml/qml/images/ShowFilesIcon24.png"
                    text: qsTr("Load...")
                    onClicked: fileOpenDialog.open()
                }
                Button {
                    id: saveButton
                    icon.source: "qrc:/qt/qml/jsonqml/qml/images/SaveCurrentRecordIcon24.png"
                    text: qsTr("Save...")
                    onClicked: {
                        fileSaveDialog.currentFile = fileSaveDialog.currentFolder+"/"+client.fileSchemaExt(schemasList.currentText, "json");
                        fileSaveDialog.open()
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
                ComboBox {
                    id: schemasList
                    editable: false
                    implicitContentWidthPolicy: ComboBox.WidestText
                    model: client.schemasList
                    onCurrentTextChanged: {
                       client.setSchema(currentText)
                    }
                }
                Button {
                    id: schemasButton
                    implicitWidth: implicitHeight
                    text: qsTr("...")
                    onClicked: folderDialog.open()
                }
            }
        }

        JsonEditor  {
            id: jsonData
            visible: true
            Layout.margins : appWindow.margin
            Layout.fillWidth: true
            Layout.fillHeight: true
            json_client: client
            json_model: client.jsonmodel
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
        title: qsTr("Choose a JSON file")
        fileMode: FileDialog.OpenFile
        nameFilters: [qsTr("JSON files (*.json)")]
        currentFolder: Preferences.workDir
        onAccepted:  client.readJson(fileOpenDialog.selectedFile)
    }

    FileDialog {
        visible: false
        id: fileSaveDialog
        title: qsTr("Save a JSON file")
        fileMode: FileDialog.SaveFile
        nameFilters: [qsTr("JSON files (*.json)")]
        currentFolder: Preferences.workDir
        onAccepted: client.saveJson(selectedFile)
    }

    FolderDialog {
        id: folderDialog
        title: qsTr("Choose a folder with schemas")
        currentFolder: Preferences.workDir
        onAccepted: Preferences.changeScemasPath(selectedFolder)
    }

}
