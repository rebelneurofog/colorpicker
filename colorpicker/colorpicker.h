// -*- mode: c++ -*-

#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QFrame>

class ColorPicker: public QFrame
{
    Q_OBJECT

public:
    ColorPicker (QWidget*);
    QColor getCurrentColor ();
    double getCurrentHue ();
    void setColor (double, double, double);
    void setMarkupMode (bool);
    
protected:
    void resizeEvent (QResizeEvent*);
    void mousePressEvent (QMouseEvent*);
    void mouseReleaseEvent (QMouseEvent*);
    void mouseMoveEvent (QMouseEvent*);
    void paintEvent (QPaintEvent*);

private:
    void estimateGeometry ();
    void updateSize ();
    void updatePaletteImage ();
    void updateMarkupImage ();
    void adjustCursor ();

private:
    int margin;
    double hue_offset;
    QSize estimated_size;
    int estimated_triangle_rib;
    int estimated_hue_area_start_y;
    int estimated_hue_area_spectrum_start_y;
    int estimated_hue_area_spectrum_h;
    double estimated_x1;
    double estimated_y1;
    double estimated_x2;
    double estimated_y2;
    double estimated_x3;
    double estimated_y3;
    double estimated_dx21;
    double estimated_dy21;
    double estimated_c1;
    double estimated_value_max_distf;
    QImage palette_image;
    QImage markup_image;
    QPoint pointer_pos;
    bool select_mode;
    bool move_mode;
    QPoint move_start;
    double move_start_hue_offset;
    bool markup_mode;

signals:
    void colorChanged (const QColor&, double);
};

#endif
