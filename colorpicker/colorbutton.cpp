#include "colorbutton.h"

#include <QPainter>

ColorButton::ColorButton (QWidget *parent)
    : QLabel (parent), color (0xff000000), recognized (false)
{
    setFrameStyle (QFrame::Panel | QFrame::Raised);
    setAlignment (Qt::AlignCenter);
    setText ("<b style=\"color: #ff0000;\">Unrecognized</b>");
    setMinimumSize (QSize (40, 40));
}
void ColorButton::setColor (const QColor &new_color)
{
    color = new_color;
    recognized = true;
    setText ("");
    setFrameStyle (QFrame::Panel | QFrame::Sunken);
    update ();
}
void ColorButton::paintEvent (QPaintEvent *event)
{
    if (recognized) {
	QPainter p;
	p.begin (this);
	p.setPen (Qt::NoPen);
	p.setBrush (color);
	p.drawRect (rect ());
	p.end ();
	QFrame::paintEvent (event);
    } else {
	QLabel::paintEvent (event);
    }
}
