#include "colorpicker.h"

#include <QPainter>
#include <QtCore/qmath.h>
#include <QMouseEvent>
#include <QBitmap>

#define SQRT_3 (1.7320508075689)
#define HALF_SQRT_3 (0.86602540378444)


static double interpolate (double value, double min_value, double max_value)
{
    if (qAbs (max_value - min_value) < 0.000001)
	return 0.0;
    return qMax (qMin ((value - min_value)/(max_value - min_value), 1.0), 0.0);
}
static double clamp (double value, double min_value, double max_value)
{
    return qMin (qMax (value, min_value), max_value);
}


ColorPicker::ColorPicker (QWidget *parent)
    : QFrame (parent), margin (10), hue_offset (0.0),
      estimated_size (300, 600), estimated_hue_area_start_y (0), estimated_hue_area_spectrum_start_y (0), estimated_hue_area_spectrum_h (10),
      estimated_x1 (0.0), estimated_y1 (0.0), estimated_x2 (0.0), estimated_y2 (0.0), estimated_x3 (0.0), estimated_y3 (0.0),
      estimated_dx21 (0.0), estimated_dy21 (0.0), estimated_c1 (1.0), estimated_value_max_distf (1.0),
      palette_image (300, 600, QImage::Format_RGB32),
      markup_image (300, 600, QImage::Format_ARGB32),
      pointer_pos (0, 0), select_mode (false), move_mode (false), markup_mode (false)
{
    setFrameStyle (QFrame::Panel | QFrame::Sunken);
    adjustCursor ();
    setMinimumSize (estimated_size);
    estimateGeometry ();
}
QColor ColorPicker::getCurrentColor ()
{
    return palette_image.pixel (pointer_pos);
}
double ColorPicker::getCurrentHue ()
{
    double hue = hue_offset + interpolate (pointer_pos.y (), estimated_hue_area_spectrum_start_y, estimated_size.height () - margin - 1.0);
    hue -= floor (hue);
    return hue;
}
void ColorPicker::setColor (double red, double green, double blue)
{
    // TODO: Get the values without precision loss
    QColor color (red*255.0, green*255.0, blue*255.0);
    double hue = color.hue ()/360.0;
    double saturation = color.saturation ()/255.0;
    double value = color.value ()/255.0;
    hue_offset = hue;
    updatePaletteImage ();
    updateMarkupImage ();
    int w = estimated_size.width ();
    pointer_pos = QPoint ((saturation*estimated_triangle_rib*0.5 - w)*value + w - margin,
			  saturation*estimated_triangle_rib*HALF_SQRT_3*value + margin);
    emit colorChanged (palette_image.pixel (pointer_pos), getCurrentHue ());
    update ();
}
void ColorPicker::setMarkupMode (bool marked_up)
{
    markup_mode = marked_up;
    update ();
}
void ColorPicker::estimateGeometry ()
{
    if (estimated_size.width () < 200)
	estimated_size.setWidth (200);
    if (estimated_size.height () < 400)
	estimated_size.setHeight (400);
    margin = estimated_size.width ()*0.05;
    estimated_triangle_rib = estimated_size.width () - margin*2;
    estimated_hue_area_start_y = ((estimated_triangle_rib*HALF_SQRT_3) + margin);
    estimated_hue_area_spectrum_start_y = estimated_hue_area_start_y + margin*HALF_SQRT_3;
    estimated_hue_area_spectrum_h = estimated_size.height () - margin - estimated_hue_area_start_y;
    int w = estimated_size.width ();
    estimated_x1 = margin;
    estimated_y1 = margin;
    estimated_x2 = estimated_triangle_rib*0.5 + margin;
    estimated_y2 = estimated_triangle_rib*HALF_SQRT_3 + margin;
    estimated_x3 = w - margin;
    estimated_y3 = margin;
    estimated_dx21 = estimated_x2 - estimated_x1;
    estimated_dy21 = estimated_y2 - estimated_y1;
    estimated_c1 = estimated_x2*estimated_y1 - estimated_x1*estimated_y2;
    estimated_value_max_distf = estimated_dy21*estimated_x3 - estimated_dx21*estimated_y3 + estimated_c1;
}
void ColorPicker::updateSize ()
{
    estimated_size = size ();
    estimateGeometry ();
    palette_image = QImage (estimated_size, QImage::Format_RGB32);
    markup_image = QImage (estimated_size, QImage::Format_ARGB32);
    updatePaletteImage ();
    updateMarkupImage ();
    emit colorChanged (palette_image.pixel (pointer_pos), getCurrentHue ());
}
void ColorPicker::resizeEvent (QResizeEvent*)
{
    updateSize ();
}
void ColorPicker::updatePaletteImage ()
{
    uchar *data = palette_image.bits ();
    int w = estimated_size.width ();
    int h = estimated_size.height ();
    int stride = palette_image.bytesPerLine ();
    for (int y = 0; y < h; ++y) {
	QRgb *pixel = (QRgb*) (data + y*stride);
	for (int x = 0; x < w; ++x, ++pixel) {
	    if (x < margin) { // Left border
		*pixel = 0xffffffff;
	    } else if (x >= (w - margin)) { // Right border
		*pixel = 0xff000000;
	    } else {
		if (y < estimated_hue_area_start_y) { // General triangle area (including top margin)
		    if (y < margin) { // Special top area
			double value = interpolate (x, w - margin - 1.0, margin);
			*pixel = QColor::fromHsvF (clamp (hue_offset, 0.0, 1.0), 0.0, clamp (value, 0.0, 1.0)).rgb ();
		    } else {
			if ((x - margin)*SQRT_3 < (y - margin)) {
			    double hue = hue_offset;
			    double saturation = interpolate (x, margin, w*0.5);
			    double value = interpolate (x, w - margin - 1.0, w*0.5);
			    *pixel = QColor::fromHsvF (clamp (hue, 0.0, 1.0), clamp (saturation, 0.0, 1.0), clamp (value, 0.0, 1.0)).rgb ();
			} else if ((w - (x + margin))*SQRT_3 < (y - margin)) {
			    double hue = hue_offset;
			    double saturation = interpolate (x, margin, w*0.5);
			    double value = interpolate (x, w - margin - 1.0, w*0.5);
			    *pixel = QColor::fromHsvF (clamp (hue, 0.0, 1.0), clamp (saturation, 0.0, 1.0), clamp (value, 0.0, 1.0)).rgb ();
			} else {
			    double value_distf = estimated_dy21*x - estimated_dx21*y + estimated_c1;
			    double value = interpolate (value_distf, estimated_value_max_distf, 0.0);
			    double saturation = interpolate (y, margin, margin + estimated_triangle_rib*HALF_SQRT_3*value);
			    *pixel = QColor::fromHsvF (clamp (hue_offset, 0.0, 1.0), clamp (saturation, 0.0, 1.0), clamp (value, 0.0, 1.0)).rgb ();
			}
		    }
		} else { // Hue spectrum area + hue bottom margin
		    double triangle_width = margin*interpolate (y, estimated_hue_area_start_y, estimated_hue_area_spectrum_start_y - 1.0);
		    double hue = hue_offset + interpolate (y, estimated_hue_area_spectrum_start_y, h - margin - 1.0);
		    hue -= floor (hue);
		    double saturation = interpolate (x, margin, (w - triangle_width)*0.5);
		    double value = interpolate (x, w - margin - 1.0, (w - triangle_width)*0.5 + triangle_width);
		    *pixel = QColor::fromHsvF (clamp (hue, 0.0, 1.0), clamp (saturation, 0.0, 1.0), clamp (value, 0.0, 1.0)).rgb ();
		}
	    }
	}
    }
}
void ColorPicker::updateMarkupImage ()
{
    uchar *data = markup_image.bits ();
    int w = estimated_size.width ();
    int h = estimated_size.height ();
    int stride = markup_image.bytesPerLine ();
    for (int y = 0; y < h; ++y) {
	QRgb *pixel = (QRgb*) (data + y*stride);
	for (int x = 0; x < w; ++x, ++pixel) {
	    if (x < margin) { // Left border
		*pixel = 0x44888888;
	    } else if (x >= (w - margin)) { // Right border
		*pixel = 0x44888888;
	    } else {
		if (y < estimated_hue_area_start_y) { // General triangle area (including top margin)
		    if (y < margin) { // Special top area
			*pixel = 0x44ffffff;
		    } else {
			if ((x - margin)*SQRT_3 < (y - margin)) {
			    *pixel = 0x44ffffff;
			} else if ((w - (x + margin))*SQRT_3 < (y - margin)) {
			    *pixel = 0x44ffffff;
			} else {
			    *pixel = 0x44000000;
			}
		    }
		} else { // Hue spectrum area + hue bottom margin
		    double triangle_width = margin*interpolate (y, estimated_hue_area_start_y, estimated_hue_area_spectrum_start_y - 1.0);
		    double triangle_height = triangle_width*0.86602540378;
		    if (y > (estimated_hue_area_start_y + triangle_height)) {
			if (x > ((w - triangle_width)*0.5 + triangle_width))
			    *pixel = 0x44ffffff;
			else if (x < ((w - triangle_width)*0.5))
			    *pixel = 0x44ffffff;
			else
			    *pixel = 0x44000000;
		    } else {
			if (x > ((w - triangle_width)*0.5 + triangle_width))
			    *pixel = 0x44000000;
			else if (x < ((w - triangle_width)*0.5))
			    *pixel = 0x44000000;
			else
			    *pixel = 0x44ffffff;
		    }
		}
	    }
	}
    }
}
void ColorPicker::adjustCursor ()
{
    QBitmap bitmap (1, 1);
    bitmap.clear ();
    if (move_mode) {
	setCursor (QCursor (bitmap, bitmap));
    } else if (select_mode) {
	setCursor (QCursor (bitmap, bitmap));
    } else {
	setCursor (Qt::CrossCursor);
    }
}
void ColorPicker::mousePressEvent (QMouseEvent *event)
{
    if (event->button () == Qt::LeftButton) {
	select_mode = true;
    } else if (event->button () == Qt::MiddleButton) {
	move_mode = true;
	move_start = event->pos ();
	move_start_hue_offset = hue_offset;
    } else if (event->button () == Qt::RightButton) {
	QPoint new_pos = event->pos ();
	new_pos.setX (qMax (qMin (new_pos.x (), estimated_size.width () - 1), 0));
	new_pos.setY (qMax (qMin (new_pos.y (), estimated_size.height () - 1), 0));
	hue_offset += interpolate (new_pos.y (), estimated_hue_area_spectrum_start_y, estimated_size.height () - margin - 1.0);
	hue_offset -= floor (hue_offset);
	updatePaletteImage ();
	updateMarkupImage ();
	update ();
	QCursor::setPos (mapToGlobal (QPoint (estimated_x2, (estimated_y2 - margin)*0.3 + margin)));
    }
    adjustCursor ();
    if (select_mode && !move_mode) {
	QPoint new_pos = event->pos ();
	new_pos.setX (qMax (qMin (new_pos.x (), estimated_size.width () - 1), 0));
	new_pos.setY (qMax (qMin (new_pos.y (), estimated_size.height () - 1), 0));
	if (pointer_pos != new_pos) {
	    pointer_pos = new_pos;
	    emit colorChanged (palette_image.pixel (pointer_pos), getCurrentHue ());
	    update ();
	}
    }
}
void ColorPicker::mouseReleaseEvent (QMouseEvent *event)
{
    if (event->button () == Qt::LeftButton) {
	select_mode = false;
    } if (event->button () == Qt::MiddleButton) {
	move_mode = false;
	update ();
    }
    adjustCursor ();
}
void ColorPicker::mouseMoveEvent (QMouseEvent *event)
{
    if (move_mode) {
	const QPoint &pos = event->pos ();
	if (pos.y () != move_start.y ()) {
	    hue_offset += double (move_start.y () - pos.y ())/estimated_hue_area_spectrum_h;
	    hue_offset -= floor (hue_offset);
	    QCursor::setPos (mapToGlobal (move_start));
	    updatePaletteImage ();
	    updateMarkupImage ();
	    emit colorChanged (palette_image.pixel (pointer_pos), getCurrentHue ());
	    update ();
	}
    } else if (select_mode) {
	QPoint new_pos = event->pos ();
	new_pos.setX (qMax (qMin (new_pos.x (), estimated_size.width () - 1), 0));
	new_pos.setY (qMax (qMin (new_pos.y (), estimated_size.height () - 1), 0));
	if (pointer_pos != new_pos) {
	    pointer_pos = new_pos;
	    emit colorChanged (palette_image.pixel (pointer_pos), getCurrentHue ());
	    update ();
	}
    }
}
void ColorPicker::paintEvent (QPaintEvent *event)
{
    QPainter p;
    p.begin (this);
    p.drawImage (palette_image.rect (), palette_image, palette_image.rect ());
    if (markup_mode)
	p.drawImage (markup_image.rect (), markup_image, markup_image.rect ());
    int middle_area_left = (estimated_size.width () - margin) >> 1;
    int middle_area_right = middle_area_left + margin;
    QPointF base_point (QPointF (0.5, 0.5) + pointer_pos);
    if ((pointer_pos.x () > middle_area_left) && (pointer_pos.x () < middle_area_right) &&
	(pointer_pos.y () > estimated_hue_area_spectrum_start_y)) {
	p.setPen (QColor (0, 0, 0));
	p.drawLine (base_point + QPoint (1, 0), base_point + QPoint (8, 7));
	p.drawLine (base_point + QPoint (1, 0), base_point + QPoint (8, -7));
	p.drawLine (base_point + QPoint (3, 0), base_point + QPoint (10, 7));
	p.drawLine (base_point + QPoint (3, 0), base_point + QPoint (10, -7));
	p.drawLine (base_point + QPoint (-1, 0), base_point + QPoint (-8, 7));
	p.drawLine (base_point + QPoint (-1, 0), base_point + QPoint (-8, -7));
	p.drawLine (base_point + QPoint (-3, 0), base_point + QPoint (-10, 7));
	p.drawLine (base_point + QPoint (-3, 0), base_point + QPoint (-10, -7));
	p.setPen (QColor (0xff, 0xff, 0xff));
	p.drawLine (base_point + QPoint (2, 0), base_point + QPoint (9, 7));
	p.drawLine (base_point + QPoint (2, 0), base_point + QPoint (9, -7));
	p.drawLine (base_point + QPoint (-2, 0), base_point + QPoint (-9, 7));
	p.drawLine (base_point + QPoint (-2, 0), base_point + QPoint (-9, -7));
    } else {
	if ((pointer_pos.x () > estimated_size.width () >> 1) ^ (pointer_pos.y () < estimated_hue_area_start_y)) {
	    p.setPen (QColor (0, 0, 0));
	    p.drawLine (pointer_pos + QPoint (2, 2), pointer_pos + QPoint (11, 2));
	    p.drawLine (pointer_pos + QPoint (2, 3), pointer_pos + QPoint (2, 11));
	    p.drawLine (pointer_pos + QPoint (4, 4), pointer_pos + QPoint (13, 4));
	    p.drawLine (pointer_pos + QPoint (4, 5), pointer_pos + QPoint (4, 13));
	    p.drawLine (pointer_pos + QPoint (-2, -2), pointer_pos + QPoint (-11, -2));
	    p.drawLine (pointer_pos + QPoint (-2, -3), pointer_pos + QPoint (-2, -11));
	    p.drawLine (pointer_pos + QPoint (-4, -4), pointer_pos + QPoint (-13, -4));
	    p.drawLine (pointer_pos + QPoint (-4, -5), pointer_pos + QPoint (-4, -13));
	    p.setPen (QColor (0xff, 0xff, 0xff));
	    p.drawLine (pointer_pos + QPoint (3, 3), pointer_pos + QPoint (12, 3));
	    p.drawLine (pointer_pos + QPoint (3, 4), pointer_pos + QPoint (3, 12));
	    p.drawLine (pointer_pos + QPoint (-3, -3), pointer_pos + QPoint (-12, -3));
	    p.drawLine (pointer_pos + QPoint (-3, -4), pointer_pos + QPoint (-3, -12));
	} else {
	    p.setPen (QColor (0, 0, 0));
	    p.drawLine (pointer_pos + QPoint (-2, 2), pointer_pos + QPoint (-11, 2));
	    p.drawLine (pointer_pos + QPoint (-2, 3), pointer_pos + QPoint (-2, 11));
	    p.drawLine (pointer_pos + QPoint (-4, 4), pointer_pos + QPoint (-13, 4));
	    p.drawLine (pointer_pos + QPoint (-4, 5), pointer_pos + QPoint (-4, 13));
	    p.drawLine (pointer_pos + QPoint (2, -2), pointer_pos + QPoint (11, -2));
	    p.drawLine (pointer_pos + QPoint (2, -3), pointer_pos + QPoint (2, -11));
	    p.drawLine (pointer_pos + QPoint (4, -4), pointer_pos + QPoint (13, -4));
	    p.drawLine (pointer_pos + QPoint (4, -5), pointer_pos + QPoint (4, -13));
	    p.setPen (QColor (0xff, 0xff, 0xff));
	    p.drawLine (pointer_pos + QPoint (-3, 3), pointer_pos + QPoint (-12, 3));
	    p.drawLine (pointer_pos + QPoint (-3, 4), pointer_pos + QPoint (-3, 12));
	    p.drawLine (pointer_pos + QPoint (3, -3), pointer_pos + QPoint (12, -3));
	    p.drawLine (pointer_pos + QPoint (3, -4), pointer_pos + QPoint (3, -12));
	}
    }
    if (move_mode) {
	p.setPen (Qt::NoPen);
	p.setBrush (QColor (0x00, 0x00, 0x00, 0x88));
	const QPointF points[12] = {
	    QPointF (estimated_size.width ()*0.5, 20.0),
	    QPointF (estimated_size.width ()*0.45, 20.0 + estimated_size.width ()*0.05),
	    QPointF (estimated_size.width ()*0.55, 20.0 + estimated_size.width ()*0.05),

	    QPointF (estimated_size.width ()*0.5, 20.0 + estimated_size.width ()*0.06),
	    QPointF (estimated_size.width ()*0.45, 20.0 + estimated_size.width ()*0.11),
	    QPointF (estimated_size.width ()*0.55, 20.0 + estimated_size.width ()*0.11),

	    QPointF (estimated_size.width ()*0.5, estimated_size.height () - 20.0),
	    QPointF (estimated_size.width ()*0.45, estimated_size.height () - (20.0 + estimated_size.width ()*0.05)),
	    QPointF (estimated_size.width ()*0.55, estimated_size.height () - (20.0 + estimated_size.width ()*0.05)),

	    QPointF (estimated_size.width ()*0.5, estimated_size.height () - (20.0 + estimated_size.width ()*0.06)),
	    QPointF (estimated_size.width ()*0.45, estimated_size.height () - (20.0 + estimated_size.width ()*0.11)),
	    QPointF (estimated_size.width ()*0.55, estimated_size.height () - (20.0 + estimated_size.width ()*0.11)),
	};
	p.drawPolygon (points, 3);
	p.drawPolygon (points + 3, 3);
	p.drawPolygon (points + 6, 3);
	p.drawPolygon (points + 9, 3);
    }
    p.end ();
    QFrame::paintEvent (event);
}
