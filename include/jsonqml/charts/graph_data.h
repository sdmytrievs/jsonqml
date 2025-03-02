#pragma once

#include <memory>
#include <array>
#include <QObject>
#include <QFont>
#include <QColor>
#include <QJsonObject>

#include "jsonqml/charts/chart_model.h"

namespace jsonqml {

class SeriesLineData;

QImage markerShapeImage(const SeriesLineData& linedata);
QIcon markerShapeIcon(const SeriesLineData& linedata);
QImage textImage(const QFont& font, const QString& text);
void getLinePen(QPen& pen, const SeriesLineData& linedata);
QColor colorAt(const QColor &start, const QColor &end, qreal pos);

/// Description of one plot curve -
/// the representation of a series of points in the x-y plane
class SeriesLineData final
{
public:
    SeriesLineData(const QString& aname = "",
                   int mrk_type = 0, int mrk_size = 8,
                   int line_size = 2, int line_style = 1, int usespline =0,
                   const QColor& acolor = QColor(25, 0, 150)):
        name(aname), xcolumn(-1) // iterate by index
    {
        setChanges(mrk_type, mrk_size, line_size, line_style, usespline, acolor);
    }

    SeriesLineData(size_t ndx, size_t max_lines, const QString& aname = "",
                   int mrk_type = 0, int mrk_size = 8,
                   int line_size = 2,  int line_style = 1, int usespline =0):
        name(aname), xcolumn(-1)
    {
        QColor acolor;
        acolor.setHsv( static_cast<int>(360/max_lines*ndx), 200, 200);
        //aColor = colorAt(green, blue, double(ndx)/maxLines );
        setChanges(mrk_type, mrk_size, line_size, line_style, usespline, acolor);
    }

    int getMarkerShape() const
    {
        return marker_shape;
    }

    void setMarkerShape(int atype)
    {
        marker_shape = atype;
    }

    int getMarkerSize() const
    {
        return marker_size;
    }

    int getPenSize() const
    {
        return pen_size;
    }

    int getPenStyle() const
    {
        return pen_style;
    }

    int getSpline() const
    {
        return spline;
    }

    QColor getColor() const
    {
        return QColor(red, green, blue);
    }

    void setChanges(int mrk_type, int mrk_size, int pn_size,
                    int pn_style, int usespline, const QColor& acolor)
    {
        marker_shape = mrk_type;
        marker_size = mrk_size;
        pen_size = pn_size;
        pen_style = pn_style;
        spline  = usespline;
        red   = acolor.red();
        green = acolor.green();
        blue  = acolor.blue();
    }

    void setLineChanges(int pn_size, int pn_style, int usespline)
    {
        pen_size = pn_size;
        pen_style = pn_style;
        spline  = usespline;
    }

    void setName(const QString& aname)
    {
        name = aname;
    }

    const QString& getName() const
    {
        return name;
    }

    int getXColumn() const
    {
        return xcolumn;
    }

    void setXColumn(int andxX)
    {
        xcolumn = andxX;
    }

#ifndef NO_JSONIO
    void toJsonNode(jsonio::JsonBase& object) const;
    void fromJsonNode(const jsonio::JsonBase& object);
#endif
    void toJsonObject(QJsonObject& json) const;
    void fromJsonObject(const QJsonObject& json);

private:
    /// Type of points (old pointType)
    int marker_shape;
    /// Size of points (old pointSize)
    int marker_size;
    /// Size of lines  (old lineSize)
    int pen_size;
    /// Style of lines (enum Qt::PenStyle)
    int pen_style;
    /// Use Spline chart
    int spline;

    // point color  - the Constructs a color with the RGB values
    /// Constructs a color with the RGB value red
    int red;
    /// Constructs a color with the RGB value green
    int green;
    /// Constructs a color with the RGB value blue
    int blue;

    /// This property holds the name of the series
    QString name;
    /// This property holds the column of the model
    /// that contains the x-coordinates of data points (old ndxX)
    int xcolumn;
};


/// Description of 2D plotting widget
class ChartData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int graphType MEMBER graph_type NOTIFY dataChanged)
    Q_PROPERTY(QString title MEMBER title NOTIFY dataChanged)
    Q_PROPERTY(int axisX MEMBER axis_typeX NOTIFY dataChanged)
    Q_PROPERTY(int axisY MEMBER axis_typeY NOTIFY dataChanged)
    Q_PROPERTY(QString xName MEMBER xname NOTIFY dataChanged)

    Q_PROPERTY(double xMin READ xMin WRITE setxMin NOTIFY dataChanged)
    Q_PROPERTY(double xMax READ xMax WRITE setxMax NOTIFY dataChanged)
    Q_PROPERTY(double yMin READ yMin WRITE setyMin NOTIFY dataChanged)
    Q_PROPERTY(double yMax READ yMax WRITE setyMax NOTIFY dataChanged)

    Q_PROPERTY(double fxMin READ fxMin WRITE setfxMin NOTIFY dataChanged)
    Q_PROPERTY(double fxMax READ fxMax WRITE setfxMax NOTIFY dataChanged)
    Q_PROPERTY(double fyMin READ fyMin WRITE setfyMin NOTIFY dataChanged)
    Q_PROPERTY(double fyMax READ fyMax WRITE setfyMax NOTIFY dataChanged)

    Q_PROPERTY(QColor backColor READ getBackgroundColor WRITE setBackgroundColor NOTIFY dataChanged)
    Q_PROPERTY(QFont axisFont MEMBER axis_font NOTIFY dataChanged)

public slots:
    void updateXSelections();
    void updateYSelections(bool update_names);

signals:
    void dataChanged();

public:
    template <class T>
    ChartData(const std::vector<std::shared_ptr<T>>& aplots,  const QString& atitle,
              const QString& aXname, const QString& aYname,
              int agraph_type = LineChart ):
        graph_type(agraph_type),
        title(atitle),
        axis_typeX(5),
        axis_typeY(5),
        xname(aXname),
        yname(aYname),
        axis_font("Sans Serif", 14)
    {
        // Define background color
        setBackgroundColor(QColor(Qt::white));

        // Insert Plots and curves description
        modelsdata.clear();
        for(auto plot: aplots) {
            addNewPlot(plot);
         }

        // Graph&Fragment Min Max Region
        double regg[4] = {0., 0., 0., 0.};
        setMinMaxRegion(regg);
    }

    virtual ~ChartData();

    /// add new plot lines selection
    template <class T>
    void addNewPlot(const std::shared_ptr<T>& aplot)
    {
        int defined_lines = static_cast<int>(linesdata.size());
        int nlines = getSeriesNumber();

        aplot->setGraphType(static_cast<GRAPHTYPES>(graph_type));
        modelsdata.push_back(aplot);
        int nlin = aplot->getSeriesNumber();
        for(int jj=0; jj<nlin; jj++, nlines++) {
            if(nlines >= defined_lines) {
                linesdata.push_back(SeriesLineData( jj, nlin, aplot->getName(jj)));
            }
        }
        connect( modelsdata.back().get(), &ChartDataModel::changedXSelections,
                 this,  &ChartData::updateXSelections );
        connect( modelsdata.back().get(), &ChartDataModel::changedYSelections,
                 this,  &ChartData::updateYSelections );
    }

    /// get plot from index
    int getPlot(size_t line, size_t* modelline=nullptr) const
    {
        size_t sizecnt=0;
        for(size_t ii=0 ; ii<modelsdata.size(); ii++) {
            sizecnt += modelsdata[ii]->getSeriesNumber();
            if(line < sizecnt)  {
                if(modelline) {
                    *modelline = line - sizecnt + modelsdata[ii]->getSeriesNumber();
                }
                return static_cast<int>(ii);
            }
        }
        return -1;
    }

    int getGraphType() const
    {
        return graph_type;
    }
    void setGraphType(int newtype);

    /// Get number of series
    int getSeriesNumber() const
    {
        int nmb = 0;
        for(const auto& model: modelsdata) {
            nmb += static_cast<int>(model->getSeriesNumber());
        }
        return nmb;
    }
    size_t modelsNumber() const
    {
        return modelsdata.size();
    }
    size_t linesNumber() const
    {
        return linesdata.size();
    }

    ChartDataModel* modelData(size_t ndx)
    {
        return  modelsdata[ndx].get();
    }

    Q_INVOKABLE const SeriesLineData& lineData(size_t ndx) const
    {
        return  linesdata[ndx];
    }
    Q_INVOKABLE void setLineData(size_t ndx, const SeriesLineData& newdata)
    {
        linesdata[ndx] = newdata;
    }
    void setLineData(size_t ndx, const QString andx_x)
    {
        int modelndx = getPlot(ndx);
        if(modelndx >= 0)   {
            linesdata[ndx].setXColumn(modelsdata[static_cast<size_t>(modelndx)]->indexAbscissaName(andx_x));
        }
    }
    void setLineData(size_t ndx, const QString& aname)
    {
        linesdata[ndx].setName(aname);
    }

    void setMinMaxRegion(double reg[4]);
    double xMin() const;
    void setxMin(double val);
    double xMax() const;
    void setxMax(double val);
    double yMin() const;
    void setyMin(double val);
    double yMax() const;
    void setyMax(double val);

    double fxMin() const;
    void setfxMin(double val);
    double fxMax() const;
    void setfxMax(double val);
    double fyMin() const;
    void setfyMin(double val);
    double fyMax() const;
    void setfyMax(double val);

    QColor getBackgroundColor() const
    {
        return QColor(b_color[0], b_color[1], b_color[2]);
    }
    void setBackgroundColor(const QColor& acolor)
    {
        b_color[0] = acolor.red();
        b_color[1] = acolor.green();
        b_color[2] = acolor.blue();
    }

#ifndef NO_JSONIO
    void toJsonNode(jsonio::JsonBase& object) const;
    void fromJsonNode(const jsonio::JsonBase& object);
#endif
    void toJsonObject(QJsonObject& json) const;
    void fromJsonObject(const QJsonObject& json);

protected:
    /// GRAPHTYPES (0-line by line, 1- cumulative, 2 - isolines)
    int graph_type;
    /// Title of graphic
    QString title;

    /// Grid type for Abscissa
    int axis_typeX;
    /// Grid type for Ordinate
    int axis_typeY;
    /// Abscissa name
    QString xname;
    /// Ordinate name
    QString yname;
    /// Graph Min Max Region
    std::array<double, 4> region;
    /// Fragment Min Max Region
    std::array<double, 4> part;

    /// Define background color ( the Constructs a color with the RGB values)
    std::array<int, 3> b_color; // red, green, blue

    /// Define Axis Font
    QFont axis_font;

    /// Descriptions of model extracting data
    std::vector<std::shared_ptr<ChartDataModel>> modelsdata;
    /// Descriptions of all lines
    std::vector<SeriesLineData> linesdata;

    ChartData(const ChartData& data);   // not defined
    ChartData& operator=(const ChartData&); // not defined

    friend class PlotChartViewPrivate;
};

} // namespace jsonqml


