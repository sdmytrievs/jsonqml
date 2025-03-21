#ifndef CHARTCLIENT_H
#define CHARTCLIENT_H

#include <memory>
#include <QObject>
#include <QAbstractItemModel>
#include <QItemSelectionModel>

namespace jsonqml {

class ChartClientPrivate;

class ChartClient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel* csvmodel READ csvmodel NOTIFY csvModelChanged)
    Q_PROPERTY(bool sortingEnabled READ sortingEnabled NOTIFY tablePropertiesChanged)
    Q_PROPERTY(QString csvfile READ csvfile WRITE setCsvFile NOTIFY csvFileChanged)

    Q_DISABLE_COPY(ChartClient)

signals:
    void csvModelChanged();
    void tablePropertiesChanged();
    void csvFileChanged();

public slots:
    void copySelected(const QModelIndexList& selection);
    void copyWithNames(const QModelIndexList& selection);
    void pasteSelected(const QModelIndexList& selection);
    void toggleX(int column);
    void toggleY(int column);

public:
    explicit ChartClient(QObject *parent = nullptr);
    ~ChartClient();

    QAbstractItemModel *csvmodel();
    bool sortingEnabled();
    const QString &csvfile() {
        return csv_file;
    }
    void setCsvFile(const QString& file);

    Q_INVOKABLE void readCSV(const QString& path);
    Q_INVOKABLE void saveCSV(const QString& path);
    Q_INVOKABLE QItemSelection selectAll();

protected:
    QString csv_file{"File"};

    std::unique_ptr<ChartClientPrivate> impl_ptr;
    ChartClientPrivate* impl_func()
    { return reinterpret_cast<ChartClientPrivate *>(impl_ptr.get()); }
    const ChartClientPrivate* impl_func() const
    { return reinterpret_cast<const ChartClientPrivate *>(impl_ptr.get()); }

};

}

#endif // CHARTCLIENT_H
