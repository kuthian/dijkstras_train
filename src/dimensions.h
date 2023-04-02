#ifndef dimensions_H
#define dimensions_H

#include <QPoint>

namespace dimensions {

  static constexpr int rows = 20;
  static constexpr int columns = 40;
  static constexpr int row_size = 28;     // in pixels
  static constexpr int column_size = 28;  // in pixels

  /// Converts a QPoint in pixel units to a QPoint in row,column
  inline QPoint convert(const QPoint &p)
  {
    return QPoint(p.x() / column_size, p.y() / row_size);
  }

  /// Returns the pixel coordinates for the top left corner of the row,column
  inline QPoint top_left(const QPoint &p)
  {
    return QPoint(p.x() * column_size, p.y() * row_size);
  }

}

#endif