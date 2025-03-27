#pragma once

#include <QObject>
#include <QFont>
#include <QColor>
#include <QJsonObject>

#ifndef NO_JSONIO
namespace jsonio {
class JsonBase;
}
#endif

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
        line_name(aname), xcolumn(-1) // iterate by index
    {
        setChanges(mrk_type, mrk_size, line_size, line_style, usespline, acolor);
    }

    SeriesLineData(size_t ndx, size_t max_lines, const QString& aname = "",
                   int mrk_type = 0, int mrk_size = 8,
                   int line_size = 2,  int line_style = 1, int usespline =0):
        line_name(aname), xcolumn(-1)
    {
        QColor acolor;
        acolor.setHsv( static_cast<int>(360/max_lines*ndx), 200, 200);
        //aColor = colorAt(green, blue, double(ndx)/maxLines );
        setChanges(mrk_type, mrk_size, line_size, line_style, usespline, acolor);
    }

    const QString& name() const
    {
        return line_name;
    }
    void setName(const QString& aname)
    {
        line_name = aname;
    }

    int markerShape() const
    {
        return marker_shape;
    }
    void setMarkerShape(int atype)
    {
        marker_shape = atype;
    }

    int markerSize() const
    {
        return marker_size;
    }

    int penSize() const
    {
        return pen_size;
    }

    int penStyle() const
    {
        return pen_style;
    }

    int spline() const
    {
        return use_spline;
    }

    QColor color() const
    {
        return QColor(red, green, blue);
    }

    int xColumn() const
    {
        return xcolumn;
    }
    void setXColumn(int andxX)
    {
        xcolumn = andxX;
    }

    void setChanges(int mrk_type, int mrk_size, int pn_size,
                    int pn_style, int usespline, const QColor& acolor);

    void setLineChanges(int pn_size, int pn_style, int usespline);

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
    int use_spline;

    // point color  - the Constructs a color with the RGB values
    /// Constructs a color with the RGB value red
    int red;
    /// Constructs a color with the RGB value green
    int green;
    /// Constructs a color with the RGB value blue
    int blue;

    /// This property holds the name of the series
    QString line_name;
    /// This property holds the column of the model
    /// that contains the x-coordinates of data points (old ndxX)
    int xcolumn;
};

} // namespace jsonqml


