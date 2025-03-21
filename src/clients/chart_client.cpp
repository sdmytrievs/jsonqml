#include "jsonqml/clients/chart_client.h"
#include "table_client_p.h"

namespace jsonqml {

class ChartClientPrivate  : public TableClientPrivate
{
    Q_DISABLE_COPY_MOVE(ChartClientPrivate)

public:
    explicit ChartClientPrivate():
        TableClientPrivate()
    { }
    virtual ~ChartClientPrivate() {}


protected:

    friend class ChartClient;

};


//--------------------------------------------------------------------------

ChartClient::ChartClient(ChartClientPrivate *impl, QObject *parent):
    TableClient(impl, parent)
{
}

ChartClient::ChartClient(QObject *parent):
    ChartClient(new ChartClientPrivate(), parent)
{}

ChartClient::~ChartClient()
{}

void ChartClient::toggleX(int column)
{

}

void ChartClient::toggleY(int column)
{

}

}
