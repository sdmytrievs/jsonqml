import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: jsonRectangle
    color: contrastColor()
    border.width: 2
    border.color: "#D0D0D0"
    property var jsonModel
    property var expandedRows

    function contrastColor() {
        return palette.alternateBase
        //return Qt.styleHints.colorScheme === Qt.Light ? palette.dark : palette.light
    }

    function saveExpandedRows() {
        expandedRows = []
        for(var i=0; i<jsonTreeView.rows; i++) {
            if(jsonTreeView.isExpanded(i)) {
                expandedRows.push(i)
            }
        }
    }

    function getExpandedRows() {
        for(var i=0; i<expandedRows.length; i++) {
            jsonTreeView.expand(expandedRows[i])
        }
    }

    Connections{
        target : jsonRectangle.jsonModel
        function onModelAboutToBeReset() {
            saveExpandedRows()
        }
        function onModelExpand() {
            getExpandedRows()
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        anchors.margins: 2

        contentItem: TreeView {
            id: jsonTreeView
            clip: true
            Layout.fillWidth: true
            Layout.fillHeight: true
            boundsBehavior: Flickable.StopAtBounds
            model: jsonRectangle.jsonModel

            delegate: TreeViewDelegate {
                leftMargin: 30
                //editing: true
                //            required property TreeView treeView
                //                         required property bool isTreeNode
                //                         required property bool expanded
                //                         required property int hasChildren
                //                         required property int depth
                //                         required property int row
                //                         required property int column
                //                         required property bool current
            }
        }
    }
}
