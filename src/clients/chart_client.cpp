#include <QFile>
#include "jsonqml/clients/chart_client.h"
#include "jsonqml/clients/settings_client.h"
#include "jsonqml/models/csv_model.h"

namespace jsonqml {

class ChartClientPrivate
{
    Q_DISABLE_COPY_MOVE(ChartClientPrivate)

public:
    explicit ChartClientPrivate();
    virtual ~ChartClientPrivate() {}

    void chart_init();

    bool sorting_enabled() const;
    QAbstractItemModel* csv_model() const;

protected:
    int keys_table_mode = RowSortingEnabled;
    QSharedPointer<CSVModel> csv_model_data;
    QSharedPointer<SortFilterProxyModel> sort_proxy_model;

    friend class ChartClient;
};

ChartClientPrivate::ChartClientPrivate()
{
    chart_init();
}

void ChartClientPrivate::chart_init()
{
    csv_model_data.reset(new CSVModel());
    if(sorting_enabled()) {
        sort_proxy_model.reset(new SortFilterProxyModel());
        sort_proxy_model->setSourceModel(csv_model_data.get());
    }
}

QAbstractItemModel *ChartClientPrivate::csv_model() const
{
    if(sorting_enabled()) {
        return sort_proxy_model.get();
    }
    return csv_model_data.get();
}

bool ChartClientPrivate::sorting_enabled() const
{
    return keys_table_mode&RowSortingEnabled;
}

//--------------------------------------------------------------------------

ChartClient::ChartClient(QObject *parent):
    QObject(parent),
    impl_ptr(new ChartClientPrivate)
{
    //QObject::connect(this, &JsonClient::schemaChanged, this, &JsonClient::setModelSchema);
    //QObject::connect(&uiSettings(), &Preferences::scemasPathChanged, this, &JsonClient::updateSchemaList);
}


ChartClient::~ChartClient() {}

QAbstractItemModel *ChartClient::csvmodel()
{
    return impl_func()->csv_model();
}

bool ChartClient::sortingEnabled()
{
    return impl_func()->sorting_enabled();
}

void ChartClient::setCsvFile(const QString& file)
{
    csv_file = file;
    emit csvFileChanged();
}

void ChartClient::readCSV(const QString &url)
{
    if(url.isEmpty()) {
        return;
    }
    uiSettings().setError(QString());
    try {
        // save work path
        auto path = uiSettings().handleFileChosen(url);
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            uiSettings().setError("could not open file");
            return;
        }
        QByteArray ba = file.readAll();
        std::string csv_srtring =ba.toStdString();
        impl_func()->csv_model_data->setCsvString(std::move(csv_srtring));
        file.close();
        setCsvFile(path);
    }
    catch(std::exception& e) {
        uiSettings().setError(e.what());
    }
}

void ChartClient::saveCSV(const QString &url)
{
    if(url.isEmpty()) {
        return;
    }
    uiSettings().setError(QString());
    try {
        auto path = uiSettings().handleFileChosen(url);
        auto csv_string = impl_func()->csv_model_data->getCsvString();
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly)) {
            uiSettings().setError("could not open file");
            return;
        }
        QTextStream stream(&file);
        stream << csv_string.c_str();
        file.close();
        setCsvFile(path);
    }
    catch(std::exception& e) {
        uiSettings().setError(e.what());
    }
}

void ChartClient::selectAll()
{

}

void ChartClient::copySelected()
{

}

void ChartClient::copyWithNames()
{

}

void ChartClient::pasteSelected()
{

}

void ChartClient::pasteWithNames()
{

}

void ChartClient::toggleX(int column)
{

}

void ChartClient::toggleY(int column)
{

}


}
