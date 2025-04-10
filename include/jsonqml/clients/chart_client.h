#ifndef CHARTCLIENT_H
#define CHARTCLIENT_H

#include "jsonqml/clients/table_client.h"
#include "jsonqml/charts/graph_data.h"
#include "jsonqml/charts/legend_model.h"
#include "jsonqml/charts/xyseries_decorator.h"

namespace jsonqml {

class ChartClientPrivate;

class ChartClient : public TableClient
{
    Q_OBJECT

    Q_PROPERTY(ChartData* chartData READ chartData NOTIFY chartDataChanged)
    Q_PROPERTY(LegendModel* legendModel READ legendModel NOTIFY legendModelChanged)
    Q_PROPERTY(LegendModel* editLegendModel READ editLegendModel NOTIFY editLegendModelChanged)
    Q_PROPERTY(QXYSeriesDecorator* seriesDecorator READ seriesDecorator NOTIFY chartDataChanged)

    Q_DISABLE_COPY(ChartClient)

signals:
    void chartDataChanged();
    void legendModelChanged();
    void editLegendModelChanged();

public slots:
    void toggleX(int column);
    void toggleY(int column);

public:
    explicit ChartClient(QObject *parent = nullptr);
    ~ChartClient();

    ChartData* chartData();
    LegendModel *legendModel();
    LegendModel *editLegendModel();
    QXYSeriesDecorator *seriesDecorator();

    /// Get list of Abscissa indexes to QComboBox
    Q_INVOKABLE QStringList abscissaList(int index);

    /// Apply changes into legend model
    Q_INVOKABLE void applyLegend();

    Q_INVOKABLE static QStringList imageFilters();

protected:

    ChartClientPrivate* impl_func()
    { return reinterpret_cast<ChartClientPrivate *>(impl_ptr.get()); }
    const ChartClientPrivate* impl_func() const
    { return reinterpret_cast<const ChartClientPrivate *>(impl_ptr.get()); }

    ChartClient(ChartClientPrivate *impl, QObject *parent);
};

}

#endif // CHARTCLIENT_H
