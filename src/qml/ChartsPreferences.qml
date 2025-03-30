import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.jsonqml.qobjectPreferences 1.0

Item {
    id: topItem

    function contrastColor() {
        return palette.alternateBase
        //return Qt.styleHints.colorScheme === Qt.Light ? palette.dark : palette.light
    }

    function apply_changes() {
        client.chartData.graphType = chartType.currentIndex
        client.chartData.title = chartTitle.text
        client.chartData.axisX = abscissaGrid.value
        client.chartData.xName = abscissaName.text
        client.chartData.axisY = ordinateGrid.value
        client.chartData.yName = ordinateName.text

        client.chartData.xMin = minX.text
        client.chartData.xMax = maxX.text
        client.chartData.yMin = minY.text
        client.chartData.yMax = maxY.text
        client.chartData.fxMin = minXF.text
        client.chartData.fxMax = maxXF.text
        client.chartData.fyMin = minYF.text
        client.chartData.fyMax = maxYF.text

        client.chartData.axisFont = fontText.text
        client.chartData.backColor = txt_back.color
    }

    component ListHeader : Rectangle {
        id: listHeader
        color: contrastColor()

        RowLayout {
            anchors {
                fill: parent
                leftMargin: 0
                rightMargin: 0
            }
            Text {
                id: iconHdr
                text: qsTr("Legend")
            }
            Text {
                id: abscissaHdr
                text: qsTr("x#   ")
            }
            Text {
                id: nameHdr
                text: qsTr("Label Y")
                Layout.fillWidth: true
            }
        }
    }

    component ListItem : Rectangle {
        id: listItem
        height: iconElement.height
        color: contrastColor()

        required property int index
        required property var model

        RowLayout {
            anchors {
                fill: parent
                leftMargin: 0
                rightMargin: 0
            }
            spacing: 0
            Button {
                id: iconElement
                implicitWidth: implicitHeight*2
                icon.source: "image://charts/" + listItem.model.icon
                onClicked: {
                    // open dialog
                }
            }
            ComboBox {
                id: abscissaElement
                implicitWidth: implicitHeight*2
                model: client.abscissaList(index)
                currentIndex: listItem.model.abscissa
                onCurrentTextChanged: listItem.model.abscissa = currentIndex
            }
            TextField {
                id: nameElement
                Layout.fillWidth: true
                text: listItem.model.name
                onTextEdited: listItem.model.name = text
            }
        }
    }

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
                    model: ["0 - Lines/Symbols", "1 - Cumulative"]
                    currentIndex: client.chartData.graphType
                }
                TextField {
                    id: chartTitle
                    Layout.fillWidth: true
                    text: client.chartData.title
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
                        value: client.chartData.axisX
                    }

                    TextField {
                        id: abscissaName
                        Layout.columnSpan: 3
                        Layout.fillWidth: true
                        text: client.chartData.xName
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
                        value: client.chartData.axisY
                    }

                    TextField {
                        id: ordinateName
                        Layout.columnSpan: 3
                        Layout.fillWidth: true
                        text: client.chartData.yName
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
                        text: client.chartData.xMin
                    }

                    TextField {
                        id: maxX
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                        text: client.chartData.xMax
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
                        text: client.chartData.yMin
                    }

                    TextField {
                        id: maxY
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                        text: client.chartData.yMax
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
                        text: client.chartData.fxMin
                    }

                    TextField {
                        id: maxXF
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                        text: client.chartData.fxMax
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
                        text: client.chartData.fyMin
                    }

                    TextField {
                        id: maxYF
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        validator:  DoubleValidator{}
                        text: client.chartData.fyMax
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
                        text: client.chartData.axisFont
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
                            color: client.chartData.backColor
                        }
                    }
                }

            }

            GroupBox {
                id: legendBox
                anchors.margins: 0

                ScrollView {
                    id: scrollView
                    anchors.fill: parent

                    contentItem: ListView {
                        id: legendTable
                        anchors.fill: parent

                        clip: true
                        boundsBehavior: Flickable.StopAtBounds
                        model: client.legendModel
                        header: ListHeader {
                            height: 30
                            width: legendTable.width
                        }
                        headerPositioning: ListView.InlineHeader
                        delegate: ListItem {
                            width: legendTable.width
                        }
                    }
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
                apply_changes()
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

