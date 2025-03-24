import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.jsonqml.qobjectPreferences 1.0

Item {
    id: topItem
    property bool isCompleted: false

    ColumnLayout {
        id: topLayout
        anchors.fill: parent

        GroupBox {
            id: nameBox
            Layout.fillWidth: true
            Layout.minimumWidth: chartLayout.Layout.minimumWidth + 4 * 10
            Layout.minimumHeight: chartLayout.Layout.minimumHeight + 4 * 10

            ColumnLayout {
                id: chartLayout
                anchors.fill: parent
                anchors.margins: 10

                ComboBox {
                    id: chartType
                    //implicitContentWidthPolicy: ComboBox.WidestText
                    //implicitWidth : passLabel.implicitWidth
                    //model: Preferences.dbConnectList
                    Component.onCompleted: {
                        // currentIndex = find(Preferences.dbConnect)
                        // isCompleted=true
                    }
                }
                TextField {
                    id: chartTitle
                    Layout.fillWidth: true
                    //text: Preferences.dbUrl
                    //onTextEdited: Preferences.dbUrl = text
                }
            }
        }

        SplitView {
            orientation: Qt.Horizontal
            Layout.fillWidth: true
            Layout.minimumWidth: axisLayout.Layout.minimumWidth + 4 * 10
            Layout.minimumHeight: axisLayout.Layout.minimumHeight + 4 * 10

            GroupBox {
                id: otherBox

                GridLayout {
                    id: axisLayout
                    anchors.fill: parent
                    anchors.margins: 10
                    rows: 8
                    columns: 6

                    Label {
                        text: qsTr("Abscissa grid")
                        Layout.columnSpan: 2
                    }

                    SpinBox {
                        id: abscissaGrid
                        Layout.preferredWidth: 60
                        editable: true
                        from: 0
                        to: 20
                    }

                    TextField {
                        id: abscissaName
                        Layout.columnSpan: 3
                        Layout.fillWidth: true
                        placeholderText: qsTr("x")
                    }

                    Label {
                        text: qsTr("Ordinate grid")
                        Layout.columnSpan: 2
                    }

                    SpinBox {
                        id: ordinateGrid
                        Layout.preferredWidth: 60
                        editable: true
                        from: 0
                        to: 20
                    }

                    TextField {
                        id: ordinateName
                        Layout.columnSpan: 3
                        Layout.fillWidth: true
                        placeholderText: qsTr("y")
                    }

                    Label {
                        text: qsTr("Graph")
                    }

                    Label {
                        text: qsTr("x:")
                    }

                    TextField {
                        id: minX
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                    }

                    TextField {
                        id: maxX
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                    }

                    Item {
                        Layout.columnSpan: 1
                    }

                    Label {
                        text: qsTr("y:")
                    }

                    TextField {
                        id: minY
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                    }

                    TextField {
                        id: maxY
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                    }

                    Label {
                        text: qsTr("Fragment")
                        Layout.columnSpan: 1
                    }

                    Label {
                        text: qsTr("x:")
                    }

                    TextField {
                        id: minXF
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                    }

                    TextField {
                        id: maxXF
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                    }
                    Item {
                        Layout.columnSpan: 1
                    }

                    Label {
                        text: qsTr("y:")
                    }

                    TextField {
                        id: minYF
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                    }

                    TextField {
                        id: maxYF
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                    }

                    Button {
                        id: fontButton
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        text: qsTr("Label font ... ")
                        onClicked: fontDialog.open()
                    }

                    TextField {
                        id: fontText
                        Layout.columnSpan: 4
                        Layout.fillWidth: true
                        autoScroll: false
                    }

                    Button {
                        id: colorButton
                        Layout.columnSpan: 2
                        text: qsTr("Background ... ")
                        onClicked: colorDialog.open()
                    }

                    TextField {
                        id: colorText
                        enabled: false
                        Layout.columnSpan: 4
                        Layout.fillWidth: true
                        background: Rectangle {
                                id: txt_back
                                color: "white"
                            }
                    }
                }

            }

            GroupBox {
                id: legendBox

                TableView {
                    id: legendTable
                    anchors.fill: parent

                }
            }

        }

        Button {
            id: applyButton
            Layout.alignment: Qt.AlignRight
            implicitWidth : topItem.width/3
            implicitHeight: colorButton.implicitHeight*2
            text: qsTr("Apply")
            onClicked: {
                //Preferences.dbName = nameBox.currentText
                //Preferences.dbUser = userBox.currentText
                //Preferences.applyChanges()
            }
        }
    }

    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        selectedColor: txt_back.color
        onAccepted: txt_back.color = selectedColor
    }

    FontDialog {
        id: fontDialog
        title: "Please choose a font"
        onAccepted: fontText.text = selectedFont
    }
}

