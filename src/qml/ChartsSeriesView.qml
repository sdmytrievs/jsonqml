import QtQuick
import QtCharts


ChartView {
    id: chartView
    antialiasing: true
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

    Connections{
        target : client
        function onChartDataChanged() {
            console.log("onChartDataChanged")
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
    }

}
