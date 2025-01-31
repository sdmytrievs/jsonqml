import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: jsonRectangle
    color: "#FFFFFF"
    border.width: 1
    border.color: "#D0D0D0"
    property var jsonModel
    property var expandedRows

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

    TreeView {
        id: jsonTreeView
        anchors.fill: parent
        anchors.margins: 5
        model: jsonRectangle.jsonModel

        delegate: TreeViewDelegate {

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
