
#include "jsonqml/charts/chart_model.h"
#include "jsonqml/clients/chart_client.h"
#include "table_client_p.h"
#include "jsonio/txt2file.h"

namespace jsonqml {

class ChartClientPrivate  : public TableClientPrivate
{
    Q_DISABLE_COPY_MOVE(ChartClientPrivate)

public:
    explicit ChartClientPrivate():
        TableClientPrivate()
    {
        init_charts();
    }
    virtual ~ChartClientPrivate() {}

    ChartData* chartdata() const
    {
        return chart_data.get();
    }
    LegendModel *legend_model()
    {
        return series_model.get();
    }

    void read_files(const QString &path) override
    {
        read_CSV(path);
        auto chart_path = path;
        chart_path.replace(".csv", "", Qt::CaseInsensitive);
        chart_path += ".json";
        read_charts_settings(chart_path);
    }
    void save_files(const QString &path) override
    {
        save_CSV(path);
        auto chart_path = path;
        chart_path.replace(".csv", "", Qt::CaseInsensitive);
        chart_path += ".json";
        save_charts_settings(chart_path);
    }

    QStringList abscissa_list(int index);
    void toggle_X(int column);
    void toggle_Y(int column);

protected:
    /// Abscissa columns list
    std::vector<int> xColumns;
    /// Ordinate columns list
    std::vector<int> yColumns;
    /// Descriptions of model extracting graph data
    std::vector<std::shared_ptr<ChartDataModel>> chart_models;
    /// Description of 2D plotting widget
    std::shared_ptr<ChartData> chart_data;

    QSharedPointer<LegendModel> series_model;
    //friend class ChartClient;

    void init_charts();
    void read_charts_settings(const QString &path);
    void save_charts_settings(const QString &path);
};

void ChartClientPrivate::init_charts()
{
    chart_models.push_back(std::shared_ptr<ChartDataModel>(new ChartDataModel(csv_model_data.get())));
    chart_data = std::make_shared<ChartData>(chart_models, "Graph for window", "x", "y");

    series_model.reset(new LegendModel());
}

void ChartClientPrivate::read_charts_settings(const QString &path)
{

}

void ChartClientPrivate::save_charts_settings(const QString &path)
{
    jsonio::JsonFile file(path.toStdString());
    auto js_free = jsonio::JsonFree::object();
    chart_data->toJsonNode(js_free);
    file.save(js_free);
}

QStringList ChartClientPrivate::abscissa_list(int index)
{
    int abscissa_number = 0;
    int ndx_plot = chart_data->plot(index);
    if(ndx_plot >= 0) {
        abscissa_number = chart_data->modelData(ndx_plot)->getAbscissaNumber();
    }
    return LegendModel::abscissaIndexes(abscissa_number);
}

void ChartClientPrivate::toggle_X(int column)
{
    auto it = std::find(xColumns.begin(), xColumns.end(), column);
    if(it != xColumns.end()) {
        xColumns.erase(it);
    }
    else {
        xColumns.push_back(column);
    }
    csv_model_data->setXColumns(xColumns);
    chart_models[0]->setXColumns(xColumns);
}

void ChartClientPrivate::toggle_Y(int column)
{
    auto it = std::find(yColumns.begin(), yColumns.end(), column);
    if(it != yColumns.end()) {
        yColumns.erase(it);
    }
    else {
        yColumns.push_back(column);
    }
    csv_model_data->setYColumns(yColumns);
    chart_models[0]->setYColumns(yColumns, true);
}


//--------------------------------------------------------------------------

ChartClient::ChartClient(ChartClientPrivate *impl, QObject *parent):
    TableClient(impl, parent)
{
    connect(chartData(), &ChartData::changedXSelections,
            this, [this]() {legendModel()->updateLines(chartData()->lines()); } );
    connect(chartData(), &ChartData::changedYSelections,
            this,  [this]() {legendModel()->updateLines(chartData()->lines()); } );
}

ChartClient::ChartClient(QObject *parent):
    ChartClient(new ChartClientPrivate(), parent)
{}

ChartClient::~ChartClient()
{}

ChartData *ChartClient::chartData()
{
    return impl_func()->chartdata();
}

LegendModel *ChartClient::legendModel()
{
    return impl_func()->legend_model();
}

QStringList ChartClient::abscissaList(int index)
{
    return impl_func()->abscissa_list(index);
}

void ChartClient::toggleX(int column)
{
    impl_func()->toggle_X(column);
}

void ChartClient::toggleY(int column)
{
    impl_func()->toggle_Y(column);
}

}
