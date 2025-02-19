#include <QFileInfo>
#include <QSizeF>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDrag>

#include <QtCharts/QAreaSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
#include <QtCharts/QValueAxis>
//#include <QGraphicsLayout>
//#include <QRubberBand>

//#include <QPrinter>
#include <QtSvg/QSvgGenerator>
#include <QPixmap>

#include "jsonqml/charts/graph_data.h"
#include "jsonqml/charts/chart_view.h"
#ifndef NO_JSONIO
#include "jsonio/service.h"
#include "jsonio/exceptions.h"
#endif

namespace jsonqml {

class PlotChartViewPrivate
{

public:
    explicit PlotChartViewPrivate(ChartData *graphdata, QChartView *parent, QChart* achart):
        view(parent), chart(achart), gr_data(graphdata)
    {}

    ~PlotChartViewPrivate()
    {
        clear_all();
    }

    void showPlot()
    {
        show_plot_internal();
        make_grid();
    }

    void updateSeries(size_t nline);
    void highlightSeries(size_t line, bool enable);

    void updateAll()
    {
        clear_all();
        showPlot();
    }

    void updateLines()
    {
        clear_lines();
        show_plot_internal();
        attachAxis();
    }

    void updateGrid();
    void updateMinMax();
    void attachAxis();

    void updateFragment(bool new_fragment)
    {
        if(is_fragment != new_fragment)  {
            is_fragment = new_fragment;
            updateMinMax();
        }
    }

    void addLabel(const QPointF& pointF, const QString& label)
    {
        if(map_labels.find(label) != map_labels.end()) {
            chart->removeSeries(map_labels[label].get());
        }
        QScatterSeries *series  = new_scatter_label(pointF, label);
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        map_labels[label] = std::shared_ptr<QScatterSeries>(series);
    }

    void addPoint(const QPointF& pointF, const QString& label)
    {
        if(show_point) {
            chart->removeSeries(show_point.get());
        }
        QScatterSeries *series  = new_scatter_label(pointF, label);
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        show_point.reset(series);
    }

protected:
    QChartView *view;
    QChart* chart;
    ChartData *gr_data;
    QValueAxis *axisX =nullptr;
    QValueAxis *axisY =nullptr;

    std::vector<std::shared_ptr<QXYSeries> >       gr_series;
    std::vector<std::shared_ptr<QVXYModelMapper> > series_mapper;
    std::vector<std::shared_ptr<QScatterSeries> >  gr_points;
    std::vector<std::shared_ptr<QVXYModelMapper> > points_mapper;
    std::vector<std::shared_ptr<QAreaSeries> >     gr_areas;
    std::map<QString,std::shared_ptr<QScatterSeries> > map_labels;
    std::shared_ptr<QScatterSeries> show_point;

    double generated_region[4];
    bool is_fragment = false;

private:
    void clear_lines();
    void clear_axis();
    void clear_all()
    {
        clear_lines();
        clear_axis();
    }

    QXYSeries *new_series_line(const SeriesLineData& linedata);
    QScatterSeries *new_scatter_series(const SeriesLineData& linedata);
    void update_scatter_series(QScatterSeries* series, const SeriesLineData& linedata);
    void map_series_line(QXYSeries *series, QVXYModelMapper *mapper,
                         ChartDataModel* chmodel, int ycolumn, int xcolumn );

    void add_plot_line(ChartDataModel* chmodel, int ycolumn, const SeriesLineData& linedata);
    void add_scatter_series(ChartDataModel* chmodel, int ycolumn, const SeriesLineData& linedata);
    void show_plot_lines();
    void make_grid();
    void show_plot_internal();
    QScatterSeries* new_scatter_label(const QPointF& pointF, const QString& label);
    void show_area_chart();
    void update_series_line(size_t nline);
    void update_area_line(size_t nline);
};



void PlotChartViewPrivate::clear_lines()
{
    map_labels.clear();
    series_mapper.clear();
    gr_series.clear();
    points_mapper.clear();
    gr_points.clear();
    gr_areas.clear();
    chart->removeAllSeries();
}

void PlotChartViewPrivate::clear_axis()
{
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);
    delete axisX;
    axisX = nullptr;
    delete axisY;
    axisY = nullptr;
}

QXYSeries* PlotChartViewPrivate::new_series_line(const SeriesLineData& linedata)
{
    QXYSeries* series = nullptr;
    if(linedata.getPenSize() <= 0) {
        return series;
    }
    if(linedata.getSpline()) {
        series = new QSplineSeries;
    }
    else {
        series = new QLineSeries;
    }
    series->setName(QString::fromStdString(linedata.getName()));
    QPen pen = series->pen();
    getLinePen(pen, linedata);
    series->setPen(pen);
    return series;
}


QScatterSeries* PlotChartViewPrivate::new_scatter_series(const SeriesLineData& linedata)
{
    QScatterSeries *series = nullptr;
    if(linedata.getMarkerSize() <= 0/*2*/) {
        return series;
    }
    series = new QScatterSeries;
    update_scatter_series(series, linedata);
    return series;
}

void PlotChartViewPrivate::update_scatter_series(QScatterSeries* series, const SeriesLineData& linedata)
{
    series->setName(QString::fromStdString(linedata.getName()));
    series->setPen( QPen(Qt::transparent));
    series->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    auto msize = linedata.getMarkerSize()+2;
    series->setMarkerSize(msize);
#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
    series->setBrush( markerShapeImage( linedata ).scaled(msize, msize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
#else
    series->setLightMarker(markerShapeImage(linedata));
#endif

}

void PlotChartViewPrivate::map_series_line(QXYSeries *series,
                                           QVXYModelMapper *mapper, ChartDataModel* chmodel,
                                           int ycolumn, int xcolumn )
{
    mapper->setXColumn(xcolumn+1);
    mapper->setYColumn(ycolumn+1);
    mapper->setSeries(series);
    mapper->setModel(chmodel);
}

void PlotChartViewPrivate::add_plot_line(ChartDataModel* chmodel,
                                         int ycolumn, const SeriesLineData& linedata)
{
    QXYSeries *series = new_series_line(linedata);
    QVXYModelMapper *mapper = new QVXYModelMapper;
    map_series_line(series, mapper, chmodel, ycolumn, chmodel->getXColumn(linedata.getXColumn()));
    if(series) {
        chart->addSeries(series);
    }
    gr_series.push_back(std::shared_ptr<QXYSeries>(series));
    series_mapper.push_back(std::shared_ptr<QVXYModelMapper>(mapper));
}

void PlotChartViewPrivate::add_scatter_series(ChartDataModel* chmodel,
                                              int ycolumn, const SeriesLineData& linedata)
{
    QScatterSeries *series = new_scatter_series(linedata);
    QVXYModelMapper *mapper = new QVXYModelMapper;
    map_series_line(series, mapper, chmodel, ycolumn, chmodel->getXColumn(linedata.getXColumn()));
    if(series) {
        chart->addSeries(series);
    }
    gr_points.push_back(std::shared_ptr<QScatterSeries>(series));
    points_mapper.push_back(std::shared_ptr<QVXYModelMapper>(mapper));
}

void PlotChartViewPrivate::show_plot_lines()
{
    for(size_t ii=0, nline=0; ii<gr_data->modelsNumber(); ii++) {
        auto  srmodel = gr_data->modelData(ii);
        for(size_t jj=0; jj<srmodel->getSeriesNumber(); jj++, nline++)  {
            add_plot_line(srmodel, srmodel->getYColumn(jj), gr_data->lineData(nline));
            add_scatter_series(srmodel, srmodel->getYColumn(jj), gr_data->lineData(nline));
        }
    }
}

void PlotChartViewPrivate::show_area_chart()
{
    // The lower series initialized to zero values
    QLineSeries *lower_series = nullptr;
    size_t ii, jj, nline;

    for(ii=0, nline =0; ii<gr_data->modelsNumber(); ii++) {
        auto  srmodel = gr_data->modelData(ii);
        srmodel->clearXColumn();
        for(jj=0; jj<srmodel->getSeriesNumber(); jj++, nline++) {
            auto linedata = gr_data->lineData(nline);

            srmodel->addXColumn(linedata.getXColumn());
            if(linedata.getXColumn() < -1) {
                gr_areas.push_back(std::shared_ptr<QAreaSeries>(nullptr));
                continue;
            }

            linedata.setLineChanges(1,1,0);
            add_plot_line(srmodel, srmodel->getYColumn(jj), linedata);

            QLineSeries *upper_series = dynamic_cast<QLineSeries *>(gr_series.back().get());
            if(upper_series)  {
                QAreaSeries *area = new QAreaSeries(upper_series, lower_series);
                // define colors
                area->setName(QString::fromStdString(linedata.getName()));
                QPen pen = area->pen();
                getLinePen(pen, linedata);
                area->setPen(pen);
                area->setColor(pen.color());
                area->setOpacity(0.5);

                chart->addSeries(area);
                gr_areas.push_back(std::shared_ptr<QAreaSeries>(area));
                lower_series = upper_series;
            }
        }
    }
}

void PlotChartViewPrivate::show_plot_internal()
{
    switch(gr_data->getGraphType()) {
    case LineChart:
        show_plot_lines();
        break;
    case AreaChart:
        show_area_chart();
        break;
    case BarChart:
    case Isolines:
    case lines_3D:
        break;
    }
}

void PlotChartViewPrivate::updateMinMax()
{
    if(!axisX || !axisY) {
        return;
    }

    if(is_fragment) {
        if(!jsonio::essentiallyEqual(gr_data->part[0], gr_data->part[1]) &&
                !jsonio::essentiallyEqual( gr_data->part[2], gr_data->part[3]))  {
            axisY->setRange(gr_data->part[2], gr_data->part[3]);
            axisX->setRange(gr_data->part[0], gr_data->part[1]);
        }
    }
    else  {
        if(!jsonio::essentiallyEqual(gr_data->region[0],gr_data->region[1]) &&
                !jsonio::essentiallyEqual(gr_data->region[2], gr_data->region[3]))  {
            axisX->setRange(gr_data->region[0], gr_data->region[1]);
            axisY->setRange(gr_data->region[2], gr_data->region[3]);
        }
        else  {
            axisX->setRange(generated_region[0], generated_region[1]);
            axisY->setRange(generated_region[2], generated_region[3]);
        }
    }
}

void PlotChartViewPrivate::updateGrid()
{
    chart->setFont(gr_data->axis_font);
    auto title_font = gr_data->axis_font;
    title_font.setPointSize(title_font.pointSize()+2);
    chart->setTitleFont(title_font);
    chart->setTitle(QString::fromStdString(gr_data->title));

    if(!axisX || !axisY) {
        return;
    }

    updateMinMax();
    chart->setBackgroundBrush(gr_data->getBackgroundColor());

    axisX->setTickCount(gr_data->axis_typeX+1);
    //axisX->setMinorTickCount(4);
    axisX->setTitleFont(title_font);
    axisX->setLabelsFont(gr_data->axis_font);
    axisX->setTitleText(QString::fromStdString(gr_data->xname));
    auto penX = axisX->linePen();
    penX.setWidth(3);
    penX.setColor(Qt::darkGray);
    axisX->setLinePen(penX);

    axisY->setTickCount(gr_data->axis_typeY+1);
    //axisY->setMinorTickCount(4);
    axisY->setTitleFont(title_font);
    axisY->setLabelsFont(gr_data->axis_font);
    axisY->setTitleText(QString::fromStdString(gr_data->yname));
    axisY->setLinePen(penX);

    // must be setPen(QChartPrivate::defaultPen()) for lines and points
    // and default background
    // chart->setTheme(QChart::ChartThemeLight);
}

void PlotChartViewPrivate::attachAxis()
{
    size_t ii;
    if(!axisX || !axisY) {
        return;
    }

    for(ii=0; ii<gr_series.size(); ii++) {
        if(gr_series[ii].get()) {
            gr_series[ii]->attachAxis(axisX);
            gr_series[ii]->attachAxis(axisY);
        }
    }
    for(ii=0; ii<gr_points.size(); ii++) {
        if(gr_points[ii].get()) {
            gr_points[ii]->attachAxis(axisX);
            gr_points[ii]->attachAxis(axisY);
        }
    }
    for(ii=0; ii<gr_areas.size(); ii++) {
        if(gr_areas[ii].get()) {
            gr_areas[ii]->attachAxis(axisX);
            gr_areas[ii]->attachAxis(axisY);
        }
    }
}

void PlotChartViewPrivate::make_grid()
{
    if(jsonio::essentiallyEqual(gr_data->region[0], gr_data->region[1]) ||
            jsonio::essentiallyEqual(gr_data->region[2], gr_data->region[3])) {
        // default
        if(gr_data->linesNumber() == 0) {
            jsonio::JSONIO_THROW("ChartData", 11, " empty ordinate list.");
        }
        chart->createDefaultAxes();
        auto axises = chart->axes(Qt::Horizontal);
        if(axises.size() > 0) {
            axisX = dynamic_cast<QValueAxis*>(axises[0]);
        }
        axises = chart->axes(Qt::Vertical);
        if(axises.size() > 0) {
            axisY = dynamic_cast<QValueAxis*>(axises[0]);
        }

        if(axisX) {
            generated_region[0] = axisX->min();
            generated_region[1] = axisX->max();
        }
        if(axisY) {
            generated_region[2] = axisY->min();
            generated_region[3] = axisY->max();
        }
        gr_data->part[0] = 0;
        gr_data->part[1] = 0;
        gr_data->part[2] = 0;
        gr_data->part[3] = 0;

    }
    else {
        axisX = new QValueAxis;
        chart->addAxis(axisX, Qt::AlignBottom);
        axisY = new QValueAxis;
        chart->addAxis(axisY, Qt::AlignLeft);
        attachAxis();
    }
    updateGrid();
}

void PlotChartViewPrivate::updateSeries(size_t nline)
{
    switch(gr_data->getGraphType())  {
    case LineChart:
        update_series_line(nline);
        break;
    case AreaChart:
        update_area_line(nline);
        break;
    case BarChart:
    case Isolines:
    case lines_3D:
        break;
    }
}

void PlotChartViewPrivate::highlightSeries(size_t line, bool enable)
{
    if(line >= gr_data->linesNumber()) {
        return;
    }

    if(gr_data->getGraphType() == LineChart) {
        // update series lines
        auto  linedata = gr_data->lineData(line);
        QXYSeries *series =  gr_series[line].get();
        if(series) {
            QPen pen = series->pen();
            getLinePen( pen, linedata  );
            if(enable) {
                pen.setWidth(linedata.getPenSize()*2);
            }
            series->setPen(pen);
        }

        QScatterSeries *scatterseries = gr_points[line].get();
        if(scatterseries) {
            auto shsize = linedata.getMarkerSize();
            if(enable) {
                shsize *=2;
            }
#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
            scatterseries->setBrush( markerShapeImage( linedata ).scaled( shsize,shsize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
#else
            scatterseries->setLightMarker(markerShapeImage(linedata));
#endif
            scatterseries->setMarkerSize(shsize);
        }
    }
    else if(gr_data->getGraphType() == AreaChart) {
        QAreaSeries* areaseries = gr_areas[line].get();
        if(areaseries) {
            areaseries->setOpacity( (enable ? 1: 0.5) );
        }
    }
}

void PlotChartViewPrivate::update_series_line(size_t nline)
{
    if(nline >= gr_data->linesNumber()) {
        return;
    }

    auto nplot =  gr_data->getPlot(nline);
    if(nplot < 0) {
        return;
    }
    auto  srmodel = gr_data->modelData(static_cast<size_t>(nplot));
    auto  linedata = gr_data->lineData(nline);

    // update series lines
    QXYSeries *series = new_series_line(linedata);
    // delete old series
    if(gr_series[nline].get()) {
        chart->removeSeries(gr_series[nline].get());
    }

    gr_series[nline].reset(series);
    series_mapper[nline]->setSeries(series);
    series_mapper[nline]->setXColumn(srmodel->getXColumn(linedata.getXColumn())+1);
    if(series) {
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    QScatterSeries *scatterseries = new_scatter_series(linedata);
    // delete old series
    if(gr_points[nline].get()) {
        chart->removeSeries(gr_points[nline].get());
    }

    gr_points[nline].reset(scatterseries);
    points_mapper[nline]->setSeries(scatterseries);
    points_mapper[nline]->setXColumn( srmodel->getXColumn(linedata.getXColumn())+1);
    if(scatterseries) {
        chart->addSeries(scatterseries);
        scatterseries->attachAxis(axisX);
        scatterseries->attachAxis(axisY);
    }
}

void PlotChartViewPrivate::update_area_line(size_t nline)
{
    if(nline >= gr_data->linesNumber()) {
        return;
    }

    if(gr_areas[nline].get()) {
        gr_areas[nline]->setName(QString::fromStdString(gr_data->lineData(nline).getName()));
        QPen pen = gr_areas[nline]->pen();
        getLinePen( pen, gr_data->lineData(nline)  );
        gr_areas[nline]->setPen(pen);
        gr_areas[nline]->setColor(pen.color());
    }
}


QScatterSeries* PlotChartViewPrivate::new_scatter_label(const QPointF& pointF, const QString& label)
{
    QScatterSeries *series  =  new QScatterSeries;
    QFontMetrics fm(gr_data->axis_font);
    int size = std::max(fm.horizontalAdvance(label)+2, fm.height());

    series->setName(label);
    series->setPen(QPen(Qt::transparent));
    series->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    series->setMarkerSize(size);
    series->setBrush(textImage(gr_data->axis_font, label));

    auto pointV = chart->mapToValue(pointF);
    series->append(pointV);
    return series;
}


//-------------------------------------------------------------------


PlotChartView::PlotChartView(ChartData *graphdata, QWidget *parent):
    QChartView(new QChart(), parent),
    impl_ptr(new PlotChartViewPrivate(graphdata, this, chart()))
{
    setRubberBand(QChartView::RectangleRubberBand);
    setAcceptDrops(true);

    impl_ptr->showPlot();
    chart()->setDropShadowEnabled(false);
    chart()->legend()->hide();
    ///chart()->layout()->setContentsMargins(0, 0, 0, 0);
    chart()->setMargins( QMargins( 5,5, 0, 0) );
    // chart()->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    ///rubberBand = findChild<QRubberBand *>();
}

PlotChartView::~PlotChartView()
{
}

void PlotChartView::updateLine(size_t line)
{
    impl_ptr->updateSeries(line);
}

void PlotChartView::updateAll()
{
    impl_ptr->updateAll();
}

void PlotChartView::updateLines()
{
    //pdata->updateAll();
    impl_ptr->updateLines();
}

void PlotChartView::updateFragment(bool isfragment)
{
    impl_ptr->updateFragment(isfragment);
}

void PlotChartView::highlightLine(size_t line, bool enable)
{
    impl_ptr->highlightSeries(line, enable);
}

void PlotChartView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else {
            event->acceptProposedAction();
        }
    }
    else {
        event->ignore();
    }
}

void PlotChartView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else {
            event->acceptProposedAction();
        }
    }
    else {
        event->ignore();
    }
}

void PlotChartView::dropEvent(QDropEvent* event)
{
    if(event->mimeData()->hasFormat("text/plain")) {
        QString text_ = event->mimeData()->text();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        auto posF =  event->posF();
#else
        auto posF =  event->position();
#endif
        impl_ptr->addLabel(posF, text_);
    }
}

void PlotChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton) {
        /*QPointF fp = chart()->mapToValue(event->pos());
        QString label = QString("%1:%2").arg( fp.x()).arg( fp.y());
        pdata->addPoint( event->pos(), label);*/
        event->accept();
        return;
    }
//    else {
//        if(event->button() == Qt::LeftButton) {
//            if(rubberBand && rubberBand->isVisible()) {
//                if(rubberBand->height() < 3 || rubberBand->width()< 3 )  {
//                    rubberBand->hide();
//                    event->accept();
//                    return;
//                }

//                QPointF fp = chart()->mapToValue(rubberBand->geometry().topLeft());
//                QPointF tp = chart()->mapToValue(rubberBand->geometry().bottomRight());
//                QRectF  rect(fp, tp);
//                emit fragmentChanged(rect);
//            }
//        }
//    }
    QChartView::mouseReleaseEvent(event);
}


void PlotChartView::renderDocument(const QString &title, const QString &file_name)
{
    const QString fmt = QFileInfo(file_name).suffix().toLower();
    if(fmt == "pdf") {
#ifndef QT_NO_PRINTER
//        QPrinter printer(QPrinter::HighResolution);
//        printer.setDocName(title);
//        printer.setOutputFormat( QPrinter::PdfFormat );
//        printer.setOutputFileName( file_name );
//        printer.setColorMode(QPrinter::Color);
//        printer.setFullPage(true);
//        printer.setPageSize(QPageSize(QPageSize::A4));

//        QPainter p(&printer);
//        render(&p);
#endif
    }
    else if(fmt == "svg") {
        QSvgGenerator generator;
        generator.setTitle(title);
        generator.setFileName(file_name);
        generator.setSize(size());
        generator.setViewBox(rect());
        setCacheMode(QGraphicsView::CacheNone);
        QPainter painter;
        painter.begin(&generator);
        this->render(&painter);
        painter.end();
    }
    else {
        QPixmap picture = grab();
        picture.save(file_name);
    }
}

} // namespace jsonqml
