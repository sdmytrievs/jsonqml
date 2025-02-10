import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: jsonRectangle
    color: contrastColor()
    border.width: 2
    border.color: "#D0D0D0"

    property var json_model
    property var expanded_rows

    function contrastColor() {
        return palette.alternateBase
        //return Qt.styleHints.colorScheme === Qt.Light ? palette.dark : palette.light
    }

    function saveExpandedRows() {
        expanded_rows = []
        for(var i=0; i<jsonTreeView.rows; i++) {
            if(jsonTreeView.isExpanded(i)) {
                expanded_rows.push(i)
            }
        }
    }

    function getExpandedRows() {
        for(var i=0; i<expanded_rows.length; i++) {
            jsonTreeView.expand(expanded_rows[i])
        }
    }

    Connections{
        target : jsonRectangle.json_model
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
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            editTriggers: TableView.NoEditTriggers

            model: jsonRectangle.json_model

            selectionModel: ItemSelectionModel { }

            columnWidthProvider: function(column) {
                const width1 = explicitColumnWidth(column)
                if (width1 > 0)
                    return width
                const width2 = implicitColumnWidth(column)
                if(column===columns-1) {
                    return Math.max(scrollView.width-columnWidth(0), width2)
                }
                return width2
            }

            delegate: Item {
                id: jsondelegate
                implicitWidth: padding + label.x + label.implicitWidth + padding
                implicitHeight: label.implicitHeight * 1.5

                readonly property real indentation: 20
                readonly property real padding: 10

                // Assigned to by TreeView:
                required property TreeView treeView
                required property bool isTreeNode
                required property bool expanded
                required property bool hasChildren
                required property int depth
                required property int row
                required property int column
                required property bool current
                //required property bool editing

                // Rotate indicator when expanded by the user
                // (requires TreeView to have a selectionModel)
                property Animation indicatorAnimation: NumberAnimation {
                    target: indicator
                    property: "rotation"
                    from: expanded ? 0 : 90
                    to: expanded ? 90 : 0
                    duration: 100
                    easing.type: Easing.OutQuart
                }
                TableView.onPooled: indicatorAnimation.complete()
                TableView.onReused: if (current) indicatorAnimation.start()
                onExpandedChanged: indicator.rotation = expanded ? 90 : 0

                Rectangle {
                    id: background
                    anchors.fill: parent
                    color: row === treeView.currentRow
                           ? palette.highlight
                           : (treeView.alternatingRows && row % 2 !== 0
                              ? contrastColor() : palette.base)
                    //opacity: (row === treeView.currentRow) ? 0.3 : 1.0
                }

                Label {
                    id: indicator
                    x: padding + ((depth+1) * indentation)
                    anchors.verticalCenter: parent.verticalCenter
                    visible: isTreeNode && hasChildren
                    text: "▶"
                    color: "grey"
                    TapHandler {
                        onSingleTapped: {
                            let index = treeView.index(row, column)
                            treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
                            treeView.toggleExpanded(row)
                        }
                    }
                }

                Label {
                    id: label
                    x: padding + (isTreeNode ? (depth + 2) * indentation : 0)
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - padding - x
                    clip: true
                    text: model.display
                    visible: true
                }

            }
        }
    }
}
