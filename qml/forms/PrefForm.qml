import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Item {
    width: 1920
    height: 1080


    GroupBox {
        id: dbBox
        x: 622
        y: 242
        width: 677
        height: 350
        Layout.fillWidth: true
        anchors.margin: 10


        GridLayout {
            id: dbLayout
            anchors.fill: parent
            anchors.margin: 10
            spasing: 10

            rows: 5
            columns: 3

            ComboBox {
                id: comboBox
            }

            CheckBox {
                id: checkBox
                text: qsTr("Check Box")
                rotation: -1.471
            }

            CheckBox {
                id: checkBox1
                text: qsTr("Check Box")
            }

            Label {
                id: label1
                text: qsTr("ArangoDB server URL")
            }

            TextField {
                id: urlField
                Layout.columnSpan: 2
                placeholderText: qsTr("Text Field")
            }

            Label {
                id: label2
                text: qsTr("Database name")
            }

            ComboBox {
                id: nameBox
                Layout.columnSpan: 2
                editable: true
            }

            Label {
                id: label
                text: qsTr("Database user name")
            }

            ComboBox {
                id: userBox
                Layout.columnSpan: 2
                editable: true
            }

            Label {
                id: label3
                text: qsTr("Database user password")
            }

            TextField {
                id: textField
                Layout.columnSpan: 2
                placeholderText: qsTr("Text Field")
            }
        }
    }

    GroupBox {
        id: groupBox
        x: 628
        y: 607
        width: 677
        height: 200


        GridLayout {
            anchors.fill: parent
            anchors.margin: 10
            spasing: 10
            rows: 3
            columns: 5

            Label {
                id: label4
                text: qsTr("JSON schema folder ")
            }

            TextField {
                id: shemaField
                Layout.columnSpan: 2
                placeholderText: qsTr("Text Field")
            }

            Item {
                id: spacer
            }

            Button {
                id: button
                text: qsTr("Button")
            }

            CheckBox {
                id: checkBox2
                text: qsTr("Check Box")
                Layout.columnSpan: 2
            }

            CheckBox {
                id: checkBox4
                text: qsTr("Check Box")
                Layout.columnSpan: 3
            }

            CheckBox {
                id: checkBox3
                text: qsTr("Check Box")
                Layout.columnSpan: 2
            }

            CheckBox {
                id: checkBox5
                text: qsTr("Check Box")
                Layout.columnSpan: 3
            }
        }
    }

    Button {
        id: applyButton
        x: 1108
        y: 835
        width: 183
        height: 60
        text: qsTr("Apply")
    }
}
