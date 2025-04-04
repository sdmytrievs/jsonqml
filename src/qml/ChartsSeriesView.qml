import QtQuick
import QtCharts

//![1]
ChartView {
    id: chartView
    antialiasing: true
    legend.visible: false

    title: client.chartData.title
    titleFont : client.chartData.axisFont
    backgroundColor: client.chartData.backColor

    ValueAxis {
        id: axisX
        titleText: client.chartData.xName
        tickCount: client.chartData.axisX+1
        titleFont: client.chartData.axisFont
        labelsFont: client.chartData.axisFont

        min: 0
        max: 10
    }

    ValueAxis {
        id: axisY
        titleText: client.chartData.yName
        tickCount: client.chartData.axisY+1
        titleFont: client.chartData.axisFont
        labelsFont: client.chartData.axisFont

        min: -0.5
        max: 1.5
    }

    Connections{
        target : client
        function onChartDataChanged() {
            console.log("onChartDataChanged")
            chartView.removeAllSeries()
            for (var i = 0; i <= client.seriesDecorator.size; i++) {
                var scatter = chartView.createSeries(ChartView.SeriesTypeScatter, i, axisX, axisY);
                client.seriesDecorator.updateSeries(i, scatter);
            }
            client.seriesDecorator.updateMinMax();
        }
    }

    // Add data dynamically to the series
    Component.onCompleted: {
        // for (var i = 0; i <= client.seriesDecorator.size; i++) {
        //     var scatter = chartView.createSeries(ChartView.SeriesTypeScatter, i, axisX, axisY);
        //     client.seriesDecorator.updateSeries(i, scatter);
        // }
    }
}
//![1]
