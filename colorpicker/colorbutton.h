// -*- mode: c++ -*-

#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QLabel>

class ColorButton: public QLabel
{
    Q_OBJECT

public:
    ColorButton (QWidget*);
    void setColor (const QColor&);

protected:
    void paintEvent (QPaintEvent*);

private:
    QColor color;
    bool recognized;
};

#endif
