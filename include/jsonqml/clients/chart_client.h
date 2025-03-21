#ifndef CHARTCLIENT_H
#define CHARTCLIENT_H

#include "jsonqml/clients/table_client.h"

namespace jsonqml {

class ChartClientPrivate;

class ChartClient : public TableClient
{
    Q_OBJECT

    Q_DISABLE_COPY(ChartClient)

signals:

public slots:
    void toggleX(int column);
    void toggleY(int column);

public:
    explicit ChartClient(QObject *parent = nullptr);
    ~ChartClient();

protected:

    ChartClientPrivate* impl_func()
    { return reinterpret_cast<ChartClientPrivate *>(impl_ptr.get()); }
    const ChartClientPrivate* impl_func() const
    { return reinterpret_cast<const ChartClientPrivate *>(impl_ptr.get()); }

    ChartClient(ChartClientPrivate *impl, QObject *parent);
};

}

#endif // CHARTCLIENT_H
