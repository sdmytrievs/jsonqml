import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts
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
        if(Preferences.dbConnected ) {
            tabBar.setCurrentIndex(1)
        }
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
                text: "Preferences"
            }

            TabButton {
                text: "Vertex Keys"
            }

            TabButton {
                text: "Vertex"
                onClicked: { vertexClient.readEditorData(vrtxKeys.selRow) }
            }

            TabButton {
                text: "Edge Keys"
            }

            TabButton {
                text: "Edge"
                onClicked: { edgeClient.readEditorData(edgeKeys.selRow) }
            }
        }

        StackLayout {
            id: formStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            PreferencesForm {
                id: prefForm
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            VertexKeys {
                id: vrtxKeys
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            VertexForm {
                id: vrtxForm
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            EdgeKeys {
                id: edgeKeys
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            EdgeForm {
                id: edgeForm
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
        title: qsTr("Choose a JSON file")
        fileMode: FileDialog.OpenFile
        nameFilters: [qsTr("JSON files (*.json)")]
        currentFolder: Preferences.workDir
        //onAccepted: client.readJson(fileOpenDialog.selectedFile)

    }

    FileDialog {
        visible: false
        id: fileSaveDialog
        title: qsTr("Save a JSON file")
        fileMode: FileDialog.SaveFile
        nameFilters: [qsTr("JSON files (*.json)")]
        currentFolder: Preferences.workDir
        // onAccepted: client.saveJson(selectedFile)
    }

    FolderDialog {
        id: folderDialog
        title: qsTr("Choose a folder with schemas")
        currentFolder: Preferences.workDir
        onAccepted: Preferences.changeScemasPath(selectedFolder)
    }

}
