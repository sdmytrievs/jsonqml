


import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//import Qt.jsonqml.qobjectPreferences 1.0


Rectangle {
    id: tableItem
    property int selectedRow: -1
    property var keysModel
    property bool sortingEnabled: false

    function contrastColor() {
        return palette.alternateBase
        //return Qt.styleHints.colorScheme === Qt.Light ? palette.dark : palette.light
    }
    function lighterColor(col) {
        return Qt.styleHints.colorScheme === Qt.Light ? col : Qt.lighter(col, 1.2)
    }
    function darkerColor(col) {
        return Qt.styleHints.colorScheme === Qt.Light ? Qt.darker(col, 1.2) : col
    }

    color: contrastColor()

    GridLayout {
        id: tableLayout
        anchors.fill: parent
        anchors.margins: 0
        columns: 2
        rows: 2
        columnSpacing: 0
        rowSpacing: 0

        HorizontalHeaderView {
            id: horizontalHeaderView
            Layout.row: 0
            Layout.column: 1
            Layout.fillWidth: true
            clip: true
            syncView: tableView

            property int selColumn: -1
            property bool desent: false

            delegate: Rectangle {
                id: horizontalHeaderDelegate
                implicitWidth: horizontalLabel.implicitWidth + upDownIndicator.implicitWidth + 6
                implicitHeight: horizontalLabel.implicitHeight+10

                border {
                    width: 1
                    color: contrastColor()
                }
                color: contrastColor()
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: lighterColor(horizontalHeaderDelegate.color)
                    }
                    GradientStop {
                        position: 1
                        color: darkerColor(horizontalHeaderDelegate.color)
                    }
                }
                Label {
                    id: horizontalLabel
                    anchors.centerIn: parent
                    text: display
                }
                Label {
                        id: upDownIndicator
                        anchors.right: parent.right
                        text: " ^ "
                        visible: sortingEnabled && column === horizontalHeaderView.selColumn
                        rotation: ( horizontalHeaderView.desent ? 180:0)
                    }
                TapHandler {
                    id: horizontalHandler
                    enabled: sortingEnabled
                    gesturePolicy: TapHandler.ReleaseWithinBounds
                    onTapped: {
                        if(horizontalHeaderView.selColumn===column) {
                           horizontalHeaderView.desent = !horizontalHeaderView.desent
                        }
                        else {
                          horizontalHeaderView.desent =false
                          horizontalHeaderView.selColumn=column
                        }
                        if(sortingEnabled) {
                            keysModel.sort(horizontalHeaderView.selColumn,
                                           horizontalHeaderView.desent? Qt.DescendingOrder : Qt.AscendingOrder)
                        }
                    }
                }

            }
        }

        VerticalHeaderView {
            id: verticalHeaderView
            Layout.fillHeight: true
            clip: true
            syncView: tableView

            delegate: Rectangle {
                id: verticalHeaderDelegate
                implicitWidth: verticalLabel.implicitWidth+10
                implicitHeight: verticalLabel.implicitHeight+10

                border {
                    width: 1
                    color: contrastColor()
                }
                color: contrastColor()
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: lighterColor(verticalHeaderDelegate.color)
                    }
                    GradientStop {
                        position: 1
                        color: darkerColor(verticalHeaderDelegate.color)
                    }
                }
                Label {
                    id: verticalLabel
                    anchors.centerIn: parent
                    text: display
                    clip: false
                }
                TapHandler {
                    id: verticalHandler
                    gesturePolicy: TapHandler.ReleaseWithinBounds
                    onTapped: {
                        selectedRow=row
                        selModel.select(keysModel.index(row,0), ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Rows)
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                id: scrollView
                anchors.fill: parent

                contentItem: TableView {
                    id: tableView
                    clip: true
                    interactive: true
                    columnSpacing: 0
                    rowSpacing: 0
                    boundsBehavior: Flickable.StopAtBounds

                    model: keysModel

                    selectionBehavior: TableView.SelectRows
                    selectionMode: TableView.SingleSelection
                    selectionModel: ItemSelectionModel {
                        id: selModel
                        onCurrentChanged: {
                            select(currentIndex, ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Rows)
                            selectedRow =  currentIndex.row;
                            //console.log("SelectedRow: ", selectedRow)
                        }
                    }

                    columnWidthProvider: function(column) {
                        const width = explicitColumnWidth(column)
                        if (width > 0)
                            return width
                        const width1 = implicitColumnWidth(column)
                        const width2 = horizontalHeaderView.implicitColumnWidth(column)
                        return Math.max(width1, width2)
                    }

                    delegate: Rectangle {
                        implicitWidth: Math.max(keyDelegate.implicitWidth+6,20)
                        implicitHeight: keyDelegate.implicitHeight+6

                        border {
                            width: current ? 2:1
                            color: contrastColor()
                        }
                        required property bool selected
                        required property bool current

                        color: selected ? palette.highlight : palette.base

                        Label {
                            id: keyDelegate
                            anchors { margins: 5 }
                            leftPadding: 10
                            rightPadding: 10
                            text: display
                            anchors.centerIn: parent
                        }
                    }

                } // TableView
            }

            TapHandler {
                id: tapHandler
                gesturePolicy: TapHandler.ReleaseWithinBounds
                onDoubleTapped: {
                    doubleClickFunction()
                }
            }
        }
    }
}


