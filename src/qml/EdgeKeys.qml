


import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//import Qt.jsonqml.qobjectPreferences 1.0

// To do: add <filter>, <query> and <fields list> buttons

Item {
    id: eggekeysItem
    property int selRow: -1

    ColumnLayout {
        id: topKeysLayout
        anchors.fill: parent
        anchors.margins: appWindow.margin

        ComboBox {
            id: vertChemaBox
            Layout.alignment: Qt.AlignLeft
            implicitContentWidthPolicy: ComboBox.WidestText

            model: edgeClient.schemasList
            Component.onCompleted: editText = edgeClient.schema
            onCurrentTextChanged: {
                edgeClient.setSchema(currentText)
            }
        }

        SelectTableView {
            id: keysTable
            Layout.fillHeight: true
            Layout.fillWidth: true
            keysModel: edgeClient.keysmodel
            sortingEnabled: edgeClient.sortingEnabled
            onSelectedRowChanged: selRow = selectedRow


            function doubleClickFunction() {
                //console.log("edge doubleClick: ", selRow)
                edgeClient.readEditorData(selRow)
                toEdgeForm()
            }

        }
    }
    BusyIndicator {
      id: edgequery
      width: 50
      height: 50
      anchors.centerIn: parent
      running: edgeClient.queryExecuting
    }
}
