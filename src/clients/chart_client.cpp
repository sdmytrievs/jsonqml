#include <QFile>
#include "jsonqml/clients/chart_client.h"
#include "jsonqml/clients/settings_client.h"
#include "jsonqml/models/csv_model.h"

namespace jsonqml {

#ifdef __APPLE__
const char  splitRow = '\r';
const char  splitCol = '\t';
#else
const char  splitRow = '\n';
const char  splitCol = '\t';
#endif

/// Internal data for selection
struct Selection
{
    int fromCol;
    int toCol;
    int fromRow;
    int toRow;

    Selection():
        fromCol(-1), toCol(-1), fromRow(-1), toRow(-1)
    {}
    Selection(int n1, int n2, int m1, int m2 ):
        fromCol(n1), toCol(n2), fromRow(m1), toRow(m2)
    {}
};


class ChartClientPrivate
{
    Q_DISABLE_COPY_MOVE(ChartClientPrivate)

public:
    explicit ChartClientPrivate()
    {
        chart_init();
    }
    virtual ~ChartClientPrivate() {}

    bool sorting_enabled() const;
    QAbstractItemModel* csv_model() const;

    void copy_selected(const QModelIndexList& selection);
    void copy_with_names(const QModelIndexList& selection);
    void paste_selected(const QModelIndexList& selection);

protected:
    int keys_table_mode = RowSortingEnabled;
    QSharedPointer<CSVModel> csv_model_data;
    QSharedPointer<SortFilterProxyModel> sort_proxy_model;

    friend class ChartClient;

    void chart_init();
    Selection get_selection_range(const QModelIndexList& selection, bool to_paste);
    QString create_header(const Selection& sel_box);
    QString create_string(const Selection& sel_box);
    void set_from_string(const QString& str, const Selection& sel);
};

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

void ChartClientPrivate::chart_init()
{
    csv_model_data.reset(new CSVModel());
    if(sorting_enabled()) {
        sort_proxy_model.reset(new SortFilterProxyModel());
        sort_proxy_model->setSourceModel(csv_model_data.get());
    }
}

Selection ChartClientPrivate::get_selection_range(const QModelIndexList& selection, bool to_paste)
{
    Selection sel_box;
    foreach(QModelIndex ndx,  selection) {
        if(sel_box.fromCol==-1) {
            sel_box.fromCol = ndx.column();
        }
        if(sel_box.fromRow==-1) {
            sel_box.fromRow = ndx.row();
        }
        if(sel_box.fromCol > ndx.column()) sel_box.fromCol = ndx.column();
        if(sel_box.toCol < ndx.column()) sel_box.toCol = ndx.column();
        if(sel_box.fromRow > ndx.row()) sel_box.fromRow = ndx.row();
        if(sel_box.toRow < ndx.row()) sel_box.toRow = ndx.row();
    }

    // only one selected => all for end of table
    if(to_paste && sel_box.fromCol==sel_box.toCol && sel_box.fromRow==sel_box.toRow) {
        sel_box.toCol = csv_model()->columnCount()-1;
        sel_box.toRow = csv_model()->rowCount()-1;
    }
    return sel_box;
}

QString ChartClientPrivate::create_header(const Selection& sel_box)
{
    QString text, clip_text;
    for(int col=sel_box.fromCol; col<=sel_box.toCol; col++) {
        if(col > sel_box.fromCol) {
            clip_text += splitCol;
        }
        text = csv_model()->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
        clip_text += (text.isEmpty() ? " " : text);;
    }
    return clip_text;
}

QString ChartClientPrivate::create_string(const Selection& sel_box)
{
    QString text, clip_text;
    for(int row=sel_box.fromRow; row<=sel_box.toRow; row++) {
        for(int col=sel_box.fromCol; col<=sel_box.toCol; col++) {
            if(col > sel_box.fromCol) {
                clip_text += splitCol;
            }
            text = csv_model()->index(row,col).data(Qt::DisplayRole).toString();
            clip_text += (text.isEmpty() ? " " : text);
        }
        clip_text += splitRow;
    }
    return clip_text;
}

void ChartClientPrivate::set_from_string(const QString& str, const Selection& sel)
{
    if(str.isEmpty()) {
        return;
    }
    const QStringList rows = str.split(splitRow, Qt::KeepEmptyParts);
    for(int it=0, row=sel.fromRow; it<rows.count()&&row<=sel.toRow; it++, row++) {
        const QStringList cells = rows[it].split(splitCol, Qt::KeepEmptyParts);
        for(int cell=0, column=sel.fromCol; cell<cells.count()&&column<=sel.toCol; cell++,column++)  {
            csv_model()->setData(csv_model()->index(row,column), cells[cell].trimmed(), Qt::EditRole);
        }
    }
}

void ChartClientPrivate::copy_selected(const QModelIndexList &selection)
{
    Selection sel = get_selection_range(selection, false);
    QString clip_text = create_string(sel);
    uiSettings().copy(clip_text);
}

void ChartClientPrivate::copy_with_names(const QModelIndexList &selection)
{
    Selection sel = get_selection_range(selection, false);
    QString clip_text = create_header(sel);
    clip_text += splitRow;
    clip_text += create_string(sel);
    uiSettings().copy(clip_text);
}

void ChartClientPrivate::paste_selected(const QModelIndexList &selection)
{
    Selection sel = get_selection_range(selection, true);
    auto clip_text = uiSettings().paste();
    set_from_string(clip_text, sel);
}

//--------------------------------------------------------------------------

ChartClient::ChartClient(QObject *parent):
    QObject(parent),
    impl_ptr(new ChartClientPrivate)
{
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

QItemSelection ChartClient::selectAll()
{
    auto dmodel=csvmodel();
    QItemSelection all_sel;
    if(dmodel->rowCount()>0 && dmodel->columnCount()>0) {
        all_sel.select(dmodel->index(0,0),
                       dmodel->index(dmodel->rowCount()-1,dmodel->columnCount()-1));
    }
    return all_sel;
}

void ChartClient::copySelected(const QModelIndexList& selection)
{
    if(!selection.isEmpty()) {
        impl_func()->copy_selected(selection);
    }
}

void ChartClient::copyWithNames(const QModelIndexList& selection)
{
    if(!selection.isEmpty()) {
        impl_func()->copy_with_names(selection);
    }
}

void ChartClient::pasteSelected(const QModelIndexList& selection)
{
    if(!selection.isEmpty()) {
        impl_func()->paste_selected(selection);
    }
}

void ChartClient::toggleX(int column)
{

}

void ChartClient::toggleY(int column)
{

}

}
