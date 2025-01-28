

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {

    ColumnLayout {
        id: topKeysLayout
        anchors.fill: parent

        RowLayout {
            id: inrowLayout
            Layout.fillWidth: true
            Layout.margins : appWindow.margin
            spacing: 4

            Button {
                id: inButton
                icon.source: "qrc:///resources/images/Incoming24.png"
                onClicked: {
                    vertexClient.readEditorId(edgeClient.incomingVertex())
                    toVertexForm()
                }
            }
            SelectTableView {
                id: inkeysTable
                height: inButton.height*2+4
                Layout.fillWidth: true
                keysModel: edgeClient.inmodel
                sortingEnabled: false

                function doubleClickFunction() {
                    vertexClient.readEditorId(edgeClient.incomingVertex())
                    toVertexForm()
                }
            }
        }

        RowLayout {
            id: outrowLayout
            Layout.fillWidth: true
            Layout.margins : appWindow.margin
            spacing: 4

            Button {
                id: outButton
                icon.source: "qrc:///resources/images/Outgoing24.png"
                onClicked: {
                    vertexClient.readEditorId(edgeClient.outgoingVertex())
                    toVertexForm()
                }
            }
            SelectTableView {
                id: outkeysTable
                height: outButton.height*2+4
                Layout.fillWidth: true
                keysModel: edgeClient.outmodel
                sortingEnabled: false

                function doubleClickFunction() {
                    vertexClient.readEditorId(edgeClient.outgoingVertex())
                    toVertexForm()
                }
            }

        }

        JsonView  {
            id: jsonData
            visible: true
            Layout.margins : appWindow.margin
            Layout.fillWidth: true
            Layout.fillHeight: true
            jsonModel: edgeClient.jsonmodel

        }
    }
}

