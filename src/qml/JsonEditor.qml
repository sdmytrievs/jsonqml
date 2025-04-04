import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.jsonqml.qobjectPreferences 1.0

Rectangle {
    id: jsonRectangle
    color: contrastColor()
    border.width: 2
    border.color: "#D0D0D0"

    property var json_client
    property var json_model
    property var expanded_rows
    property var model_index: json_model.index(-1, -1)

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

    Connections{
        target : jsonRectangle.json_client
        function onJsonModelAboutChanged() {
            console.log("onJsonModelChanged")
            expanded_rows = []
            model_index = json_model.index(-1, -1)
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

            selectionModel: ItemSelectionModel {
                id: selModel
                onCurrentChanged:  (current, previous) =>  {
                                       if(previous) {
                                           var item =  jsonTreeView.itemAtIndex(previous)
                                           if(item) {
                                               item.editing_mode = false
                                           }
                                       }
                                   }
            }

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
                property bool editing_mode: false

                // Assigned to by TreeView:
                required property TreeView treeView
                required property bool isTreeNode
                required property bool expanded
                required property bool hasChildren
                required property int depth
                required property int row
                required property int column
                required property bool current
                required property bool editing

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
                    opacity: (row === treeView.currentRow) ? 0.3 : 1.0
                }

                Label {
                    id: menublock
                    x: padding
                    anchors.verticalCenter: parent.verticalCenter
                    visible: isTreeNode
                    text: "⊞" // "⊞" "□" "◫"
                    color: "grey"

                    TapHandler {
                        onSingleTapped: {
                            let index = treeView.index(row, column)
                            treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
                            console.log("menu: ", treeView.model.getFieldPath(index))
                            model_index = index
                            jsonMenu.popup()
                        }
                    }
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
                    visible: !editing_mode
                    TapHandler {
                        onDoubleTapped: {
                            let index = treeView.index(row, column)
                            if(json_model.isEditable(index)) {
                                editing_mode = true
                                if(json_model.useComboBox) {
                                    var data = model.display
                                    if(column === 0) {
                                        data = json_model.data(treeView.index(row, 1), Qt.DisplayRole);
                                    }
                                    checkField.currentIndex = checkField.find(data)
                                }
                                model_index = index
                            }
                        }
                    }
                }

                TextField {
                    id: textField
                    x: padding + (isTreeNode ? (depth + 2) * indentation : 0)
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - padding - x
                    color: "red"
                    text: model.display
                    visible: editing_mode && !json_model.useComboBox
                    onEditingFinished: {
                        editing_mode = false
                    }
                    onAccepted: {
                        model.edit = text
                        //let index = treeView.index(row, column)
                        //json_model.setData(index, textField.text, Qt.EditRole)
                    }
                }

                ComboBox {
                    id: checkField
                    textRole: "text"
                    valueRole: "value"
                    x: padding + (isTreeNode ? (depth + 2) * indentation : 0)
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - padding - x
                    visible: editing_mode && json_model.useComboBox
                    model: json_model.editorValues

                    onActivated: {
                        editing_mode = false
                        let index = treeView.index(row, column)
                        json_model.setData(index, currentValue, Qt.EditRole)
                        if(column === 0) {
                            json_model.setData(treeView.index(row, 1), currentText, Qt.EditRole);
                        }
                    }
                }

            }

            //            Keys.onPressed: function (event) {
            //                if(event.modifiers & Qt.ControlModifier) {
            //                    if(event.key === Qt.Key_Tab) {
            //                        if(event.modifiers & Qt.ShiftModifier)
            //                            console.log('backward')
            //                        else
            //                            console.log('forward')
            //                    }
            //                }
            //            }
        }
    }

    Menu {
        id: jsonMenu
        implicitWidth: pathItem.implicitContentWidth*1.8

        MenuItem {
            text: qsTr("Insert")
            enabled: json_model.canBeAdd(model_index)
            onClicked: {
                if(json_model.useSchema) {
                    namesBox.model = json_model.fieldNames(model_index)
                    selectDialog.open()
                }
                else {
                    insertDialog.open()
                }
            }
        }
        MenuItem {
            text: qsTr("Resize")
            enabled: json_model.canBeResized(model_index)
            onClicked: {
                sizeBox.value = json_model.rowCount(model_index)
                resizeDialog.open()
            }
        }
        MenuItem {
            text: qsTr("Duplicate")
            enabled: json_model.canBeCloned(model_index)
            onClicked: {
                json_model.cloneObject(model_index)
            }
        }
        MenuItem {
            text: qsTr("Remove")
            enabled: json_model.canBeRemoved(model_index)
            onClicked: {
                json_model.removeObject(model_index)
            }
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr("Copy")
            onClicked: {
                Preferences.copy(json_model.getFieldData(model_index))
            }
        }
        MenuItem {
            id: pathItem
            text: qsTr("Copy path")
            onClicked: {
                Preferences.copy(json_model.getFieldPath(model_index))
            }
        }
        MenuItem {
            text: qsTr("Paste")
            onClicked: {
                saveExpandedRows()
                json_model.setFieldData(model_index, Preferences.paste())
                getExpandedRows()
            }
        }

    }

    Dialog {
        id: insertDialog
        title: "Insert"
        contentItem: ColumnLayout {
            implicitHeight: typeBox.implicitHeight*3
            TextInput {
                id: inputKey
                Layout.fillWidth: true
                text: "new_name"
            }
            ComboBox {
                id: typeBox
                Layout.fillWidth: true
                model: json_model.typeNames
            }
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: json_model.addObject(model_index, typeBox.currentText, inputKey.text)
    }

    Dialog {
        id: selectDialog
        title: "Insert"
        contentItem: ColumnLayout {
            implicitHeight: namesBox.implicitHeight*3
            ComboBox {
                id: namesBox
                Layout.fillWidth: true
            }
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            saveExpandedRows()
            json_model.addObject(model_index, "object", namesBox.currentText)
            getExpandedRows()
         }
    }

    Dialog {
        id: resizeDialog
        title: "New Size"
        contentItem: ColumnLayout {
            implicitHeight: sizeBox.implicitHeight*2
            SpinBox {
                id: sizeBox
                Layout.fillWidth: true
                value: 0
            }
        }
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted:  json_model.resizeArray(model_index, sizeBox.value)
    }

}

