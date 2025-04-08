import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts


Item {
    id: mainItem
    property int old_index: -1

    function setChartDataSeries() {
        chartView.removeAllSeries()
        if( client.chartData.graphType === 1) {
            for (var i = 0; i <= client.seriesDecorator.size; i++) {
                var area = chartView.createSeries(ChartView.SeriesTypeArea, i, axisX, axisY);
                client.seriesDecorator.updateAreaSeries(i, area);
            }
        }
        else {
            for (var is = 0; is <= client.seriesDecorator.size; is++) {
                var scatter = chartView.createSeries(ChartView.SeriesTypeScatter, is, axisX, axisY);
                client.seriesDecorator.updateSeries(is, scatter);
            }
        }
        client.seriesDecorator.updateMinMax();
    }

    function highlightSeries(index) {
        client.seriesDecorator.highlightSeries(old_index, false);
        if(old_index !== index) {
            old_index = index
            client.seriesDecorator.highlightSeries(index, true);
        }
        else {
            old_index = -1
        }
    }

    Connections{
        target : client
        function onChartDataChanged() {
            console.log("onChartDataChanged")
            setChartDataSeries();
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        spacing: 0

        ChartView {
            id: chartView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 0
            legend.visible: false

            title: client.chartData.title
            titleFont : client.chartData.titleFont
            backgroundColor: client.chartData.backColor

            ValueAxis {
                id: axisX
                titleText: client.chartData.xName
                tickCount: client.chartData.axisX+1
                titleFont: client.chartData.axisFont
                labelsFont: client.chartData.axisFont
            }

            ValueAxis {
                id: axisY
                titleText: client.chartData.yName
                tickCount: client.chartData.axisY+1
                titleFont: client.chartData.axisFont
                labelsFont: client.chartData.axisFont
            }

            DropArea {
                id: dropArea
                anchors.fill: parent
                onDropped: (drop) => {
                               client.seriesDecorator.dropLabel(Qt.point(drop.x, drop.y), drop.text)
                           }
            }
        }

        GridView {
            id: legendView
            Layout.fillWidth: true
            Layout.margins: 0
            height: 20
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            boundsMovement: Flickable.StopAtBounds

            model: client.legendModel
            delegate:  Row {
                id: listItem
                Button {
                    id: iconElement
                    implicitWidth: implicitHeight*2
                    icon.source: "image://charts/" + model.icon
                    onClicked: highlightSeries(index);
                }
                Label {
                    id: nameElement
                    text: "  " + model.name
                }

                Drag.dragType: Drag.Automatic
                Drag.supportedActions: Qt.CopyAction
                Drag.mimeData: {
                    "text/plain": model.name
                }

                DragHandler {
                    id: dragHandler
                    onActiveChanged:
                        if (active) {
                            nameElement.grabToImage(function(result) {
                                parent.Drag.imageSource = result.url
                                parent.Drag.active = true
                            })
                        } else {
                            parent.Drag.active = false
                        }
                }
            }
        }
    }
}


