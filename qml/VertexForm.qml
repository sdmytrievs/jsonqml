

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {

    ColumnLayout {
        id: topKeysLayout
        anchors.fill: parent

        RowLayout {
            id: rowLayout
            Layout.fillWidth: true
            Layout.margins : appWindow.margin
            spacing: 40

            Button {
                id: inButton
                icon.source: "qrc:///resources/images/Incoming24.png"
                text: qsTr("Incoming edges")
                onClicked: {
                    edgeClient.setIncomingEdges(vertexClient.editorId())
                    toEdgeKeys()
                }
            }
            Button {
                id: outButton
                icon.source: "qrc:///resources/images/Outgoing24.png"
                text: qsTr("Outgoing edges")
                onClicked: {
                    edgeClient.setOutgoingEdges(vertexClient.editorId())
                    toEdgeKeys()
                }
            }
        }

        JsonView  {
            id: jsonData
            visible: true
            //Layout.margins : appWindow.margin
            Layout.fillWidth: true
            Layout.fillHeight: true
            jsonModel: vertexClient.jsonmodel

        }
    }
}

