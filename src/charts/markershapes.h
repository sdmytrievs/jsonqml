#pragma once

#include <QPainterPath>

namespace jsonqml {

/// Class to define the shape of the marker used to render the points in the series
class MarkerShapes final
{
    friend MarkerShapes& shapes();

public:
    /// This enum value describes the shape used when rendering marker items.
    enum Style
    {
        // Poligons ( filled and not filled )
        MsCircle,        ///< Circle
        MsRectangle,     ///< Rectangle
        MsDiamond,       ///< Diamond
        MsDTriangle,     ///< Triangle pointing downwards
        MsUTriangle,     ///< Triangle pointing upwards
        MsLTriangle,     ///< Triangle pointing left
        MsRTriangle,     ///< Triangle pointing right
        MsStar,          ///< Five-pointed star

        // Symbols
        MsCross,     ///< Cross (+)
        MsXCross,    ///< Diagonal cross (X)
        MsHLine,     ///< Horizontal line
        MsVLine,     ///< Vertical line
        MsLineStar,  ///< Symbol star (*)
        NoSymbol_    ///< No Style. The symbol cannot be drawn.
    };

    std::size_t size() const
    {
        return markers.size();
    }

    const QPainterPath& shape(size_t ndx) const
    {
        return markers[ndx%markers.size()];
    }

    void addShape(const QPainterPath& ashape)
    {
        markers.push_back(ashape);
    }

protected:
    std::vector<QPainterPath> markers;

    enum TriangleType
    {
        TriangleLeft,
        TriangleRight,
        TriangleUp,
        TriangleDown
    };

    MarkerShapes();
    QPolygonF triangle_polygon(TriangleType type, const QSize& size);
};

MarkerShapes& shapes()
{
    static MarkerShapes  msh;
    return msh;
}

} // namespace jsonqml

