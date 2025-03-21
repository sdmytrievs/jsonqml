import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//import Qt.jsonqml.qobjectPreferences 1.0


Rectangle {
    id: tableItem
    property var keysModel
    property bool sortingEnabled: false
    property bool chartEnabled: true
    property int menu_row: -1
    property int menu_column: -1

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
    function isNumber(column) {
        return keysModel.sourceModel.is_number(column)
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
                    gesturePolicy: TapHandler.ReleaseWithinBounds
                    onTapped: (event, button) => {
                                  if(sortingEnabled) {
                                      if(horizontalHeaderView.selColumn===column) {
                                          horizontalHeaderView.desent = !horizontalHeaderView.desent
                                      }
                                      else {
                                          horizontalHeaderView.desent =false
                                          horizontalHeaderView.selColumn=column
                                      }
                                      keysModel.sort(horizontalHeaderView.selColumn,
                                                     horizontalHeaderView.desent? Qt.DescendingOrder : Qt.AscendingOrder)
                                  }
                              }
                    onLongPressed: () => {
                                       menu_row = -1
                                       menu_column = column
                                       tableMenu.popup()
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
                    editTriggers: TableView.DoubleTapped | TableView.AnyKeyPressed

                    model: keysModel

                    selectionBehavior: TableView.SelectCells
                    selectionMode: TableView.ExtendedSelection
                    selectionModel: ItemSelectionModel {
                        id: selModel
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
                        id: cell
                        implicitWidth: Math.max(keyDelegate.implicitWidth+6,20)
                        implicitHeight: keyDelegate.implicitHeight+6

                        border {
                            width: current ? 2:1
                            color: contrastColor()
                        }
                        required property bool current
                        required property bool selected
                        required property bool editing

                        color: selected ? palette.highlight : palette.base

                        Label {
                            id: keyDelegate
                            visible: !editing
                            anchors { margins: 5 }
                            leftPadding: 10
                            rightPadding: 10
                            text: display
                            anchors.centerIn: parent
                        }

                        TableView.editDelegate: Rectangle {
                            id: delegate
                            anchors.fill: cell
                            property string textvalue

                            TextField {
                                anchors.fill: delegate
                                visible: isNumber(column)
                                text: display
                                validator:  DoubleValidator{}
                                onEditingFinished: textvalue = text
                            }
                            TextField {
                                anchors.fill: delegate
                                visible: !isNumber(column)
                                text: display
                                onEditingFinished: textvalue = text
                            }
                            TableView.onCommit: display = textvalue
                        }
                        MouseArea {
                            anchors.fill: cell
                            acceptedButtons: Qt.RightButton
                            onClicked: (mouse) => {
                                           if (mouse.button === Qt.RightButton) {
                                               menu_row = row
                                               menu_column = column
                                               tableMenu.popup()
                                           }
                                       }
                        }

                    }

                } // TableView

                SelectionRectangle {
                        target: tableView
                    }
            }
        }
    }

    Menu {
        id: tableMenu
        implicitWidth: copyitem.implicitContentWidth*1.8

        MenuItem {
            text: qsTr("Select row")
            enabled: menu_row != -1
            onClicked: {
                selModel.select(keysModel.index(menu_row,0), ItemSelectionModel.ClearAndSelect|ItemSelectionModel.Rows)
            }
        }
        MenuItem {
            text: qsTr("Select column")
            enabled: menu_column != -1
            onClicked: {
                selModel.select(keysModel.index(0,menu_column), ItemSelectionModel.ClearAndSelect|ItemSelectionModel.Columns)
            }
        }
        MenuItem {
            text: qsTr("Select all")
            onClicked: {
                selModel.select(client.selectAll(), ItemSelectionModel.Select)
            }
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr("Copy")
            enabled: selModel.hasSelection
            onClicked: {
                client.copySelected(selModel.selectedIndexes);
            }
        }
        MenuItem {
            id: copyitem
            text: qsTr("Copy with names")
            enabled: selModel.hasSelection
            onClicked: {
                client.copyWithNames(selModel.selectedIndexes);
            }
        }
        MenuItem {
            text: qsTr("Paste")
            onClicked: {
                if(!selModel.hasSelection) {
                   selModel.select(keysModel.index((menu_row>0 ? menu_row:0), menu_column), ItemSelectionModel.Select)
                }
                client.pasteSelected(selModel.selectedIndexes);
            }
        }
        MenuSeparator {}
        MenuItem {
            id: pathItem
            visible: chartEnabled
            enabled:  isNumber(menu_column)
            text: qsTr("Toggle X")
            onClicked: {
                client.toggleX(menu_column);
            }
        }
        MenuItem {
            text: qsTr("Toggle Y")
            visible: chartEnabled
            enabled:  isNumber(menu_column)
            onClicked: {
                client.toggleY(menu_column);
            }
        }

    }
}


