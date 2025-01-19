import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.jsonqml.qobjectPreferences 1.0

Item {
    property bool isCompleted: false

    ColumnLayout {
        id: topLayout
        anchors.fill: parent

        GroupBox {
            id: dbBox
            visible: Preferences.withDatabase
            Layout.fillWidth: true
            Layout.minimumWidth: dbLayout.Layout.minimumWidth + 4 * 10
            Layout.minimumHeight: dbLayout.Layout.minimumHeight + 4 * 10

            GridLayout {
                id: dbLayout
                anchors.fill: parent
                anchors.margins: 10
                rows: 5
                columns: 3

                ComboBox {
                    id: connectBox
                    implicitContentWidthPolicy: ComboBox.WidestText
                    implicitWidth : passLabel.implicitWidth
                    model: Preferences.dbConnectList
                    Component.onCompleted: {
                           currentIndex = find(Preferences.dbConnect)
                           isCompleted=true
                    }
                    onCurrentTextChanged: {
                        if(isCompleted) {  // do not run when init
                            Preferences.changeDBConnect(currentText)
                        }
                        nameBox.currentIndex = nameBox.find(Preferences.dbName)
                        userBox.currentIndex = userBox.find(Preferences.dbUser)
                    }
                }

                CheckBox {
                    id: readonlyCheck
                    checked: Preferences.dbAccess
                    text: qsTr("Readonly mode")
                    onCheckedChanged: Preferences.dbAccess = checked
                }

                CheckBox {
                    id: createCheck
                    checked: Preferences.dbCreate
                    text: qsTr("Create database")
                    onCheckedChanged: Preferences.dbCreate = checked
                }

                Label {
                    id: urlLlabel
                    text: qsTr("ArangoDB server URL")
                }

                TextField {
                    id: urlField
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    text: Preferences.dbUrl
                    onTextEdited: Preferences.dbUrl = text
                }

                Label {
                    id: nameLabel
                    text: qsTr("Database name")
                }

                ComboBox {
                    id: nameBox
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    editable: true
                    model: Preferences.dbNamesList
                    onCurrentTextChanged: Preferences.dbName = currentText
                    onAccepted: {
                        if (find(editText) === -1)
                            Preferences.addDBName(editText)
                    }

                }

                Label {
                    id: userLabel
                    text: qsTr("Database user name")
                }

                ComboBox {
                    id: userBox
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    editable: true
                    model: Preferences.dbUsersList
                    onCurrentTextChanged: Preferences.dbUser = currentText
                    onAccepted: {
                        if (find(editText) === -1)
                            Preferences.addDBUser(editText)
                    }
                }

                Label {
                    id: passLabel
                    text: qsTr("Database user password")
                }

                TextField {
                    id: textField
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    text: Preferences.dbUserPassword
                    onTextEdited: Preferences.dbUserPassword = text
                }
            }
        }

        GroupBox {
            id: otherBox
            visible: Preferences.withSchemas
            Layout.fillWidth: true
            Layout.minimumWidth: otherLayout.Layout.minimumWidth + 4 * 10
            Layout.minimumHeight: otherLayout.Layout.minimumHeight + 4 * 10

            GridLayout {
                id: otherLayout
                anchors.fill: parent
                anchors.margins: 10
                rows: 3
                columns: 4

                Label {
                    id: schemasLabel
                    text: qsTr("JSON schema folder ")
                }

                TextField {
                    id: shemasField
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    readOnly: true
                    text: Preferences.schemasDir
                }

                Button {
                    id: schemasButton
                    implicitWidth: implicitHeight
                    text: qsTr("...")
                    onClicked: folderDialog.open()
                }

                CheckBox {
                    id: expandedCheck
                    checked: Preferences.keepExpanded
                    text: qsTr("Keep data fields expanded")
                    Layout.columnSpan: 2
                    onCheckedChanged: Preferences.keepExpanded = checked
                }

                CheckBox {
                    id: commentCheck
                    checked: Preferences.showComments
                    text: qsTr("Show schema comments into editor")
                    Layout.columnSpan: 2
                    onCheckedChanged: Preferences.showComments = checked
                }

                CheckBox {
                    id: enumCheck
                    checked: Preferences.showEnumNames
                    text: qsTr("Show enum names into editor")
                    Layout.columnSpan: 2
                    onCheckedChanged: Preferences.showEnumNames = checked
                }

                CheckBox {
                    id: idCheck
                    checked: Preferences.canEditID
                    text: qsTr("Edit system data")
                    Layout.columnSpan: 2
                    onCheckedChanged: Preferences.canEditID = checked
                }
            }
        }

        Button {
            id: applyButton
            Layout.alignment: Qt.AlignRight
            implicitWidth : connectBox.implicitWidth
            implicitHeight: schemasButton.implicitHeight*2
            text: qsTr("Apply")
            onClicked: {
                if(Preferences.applyChanges() ) {
                    tabBar.setCurrentIndex(1)
                }
           }
        }
    }
}
