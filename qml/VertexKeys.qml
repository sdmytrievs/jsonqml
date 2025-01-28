


import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//import Qt.jsonqml.qobjectPreferences 1.0

// To do: add <filter>, <query> and <fields list> buttons

Item {
    id: keysItem
    property int selRow: -1

    ColumnLayout {
        id: topKeysLayout
        anchors.fill: parent
        anchors.margins: appWindow.margin

        ComboBox {
            id: vertChemaBox
            Layout.alignment: Qt.AlignLeft
            implicitContentWidthPolicy: ComboBox.WidestText

            model: vertexClient.schemasList
            Component.onCompleted: editText = vertexClient.schema
            onCurrentTextChanged: {
                vertexClient.setSchema(currentText)
            }
        }

        SelectTableView {
            id: keysTable
            Layout.fillHeight: true
            Layout.fillWidth: true
            keysModel: vertexClient.keysmodel
            sortingEnabled: vertexClient.sortingEnabled
            onSelectedRowChanged: selRow = selectedRow


            function doubleClickFunction() {
                console.log("doubleClick: ", selRow)
                vertexClient.readEditorData(selRow)
                tabBar.setCurrentIndex(2)
            }

        }
    }

    BusyIndicator {
      id: vertexquery
      width: 50
      height: 50
      anchors.centerIn: parent
      running: vertexClient.queryExecuting
    }
}
