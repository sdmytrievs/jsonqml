import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
//import Qt.jsonqml.qobjectPreferences 1.0

Item {
    id: root

    property var legendData

    function apply_symbol() {
      var new_data =  legendData
      new_data.markerShape = radioGroup.checkedButton.ch_index
      new_data.markerSize = symbolBox.value
      new_data.penSize = sizeBox.value
      new_data.penStyle = lineBox.currentIndex
      new_data.useSpline = splineButton.checked
      new_data.color = colorRectangle.color
      legendData = new_data
    }

    onLegendDataChanged: {
      colorRectangle.color = legendData.color
      //console.log("changed:", legendData.color)
    }

    GridLayout {
        id: symbolLayout
        anchors.fill: root
        anchors.margins: 10
        rows: 4
        columns: 4

        Label {
            id: label1
            text: qsTr("Line size ")
        }

        SpinBox {
            id: sizeBox
            Layout.preferredWidth: Layout.preferredHeight*2
            value: legendData.penSize
        }

        Label {
            id: label
            text: qsTr("Line type")
        }

        ComboBox {
            id: lineBox
            model: [ "No Pen", "SolidLine", "DashLine", "DotLine", "DashDotLine"]
            currentIndex: legendData.penStyle
        }

        Label {
            id: label2
            text: qsTr("Symbol size")
        }

        SpinBox {
            id: symbolBox
            Layout.preferredWidth: Layout.preferredHeight*2
            value: legendData.markerSize
        }

        RadioButton {
            id: splineButton
            Layout.columnSpan: 2
            Layout.alignment : Qt.AlignRight
            text: qsTr("Use spline")
            checked: legendData.useSpline
        }

        GroupBox {
            id: groupBox
            Layout.columnSpan: 4
            Layout.fillWidth: true
            title: qsTr("Select symbol")

            ButtonGroup {
                id: radioGroup
            }
            GridLayout {
                anchors.fill: parent
                rows: 4
                columns: 5
                Repeater {
                    model: 20
                    Row {
                        id: row
                        required property int index
                        RadioButton {
                            id: chartButton
                            //text: row.index
                            checked: (index===legendData.markerShape ? true: false)
                            ButtonGroup.group: radioGroup
                            property int ch_index: row.index
                        }
                        Image {
                            id: chartImagerequired
                            width: chartButton.width*0.75
                            height: chartButton.height*0.75
                            //source: "image://charts/" + row.index+ "/0/#c82b2b"
                            source: "image://charts/" + row.index+ "/0/gray"
                        }
                    }
                }
            }
        }

        Button {
            id: colorButton
            text: qsTr("Color...")
            onClicked: symbolColorDialog.open()
        }

        Rectangle {
            id: colorRectangle
            color: legendData.color
            Layout.columnSpan: 3
            Layout.preferredHeight: colorButton.implicitHeight
            Layout.fillWidth: true
        }
    }

    ColorDialog {
        id: symbolColorDialog
        title: "Please choose a color"
        selectedColor: colorRectangle.color
        onAccepted: colorRectangle.color = selectedColor
    }
}
