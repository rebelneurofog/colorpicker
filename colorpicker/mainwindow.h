#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QFrame>


QT_BEGIN_NAMESPACE
class QSlider;
class QLabel;
class QButtonGroup;
class QTextEdit;
QT_END_NAMESPACE

class ColorButton;
class ColorPicker;

class MainWindow: public QFrame
{
    Q_OBJECT

public:
    MainWindow (const QString&);
    ~MainWindow ();

protected:
    void keyPressEvent (QKeyEvent*);
    void keyReleaseEvent (QKeyEvent*);

private slots:
    void recognizeInitialString ();
    void accept ();
    void decline ();
    void updateAlphaFlag (int);
    void updateTextFormat (int);
    void updateColor (const QColor&, double);
    void updateAlpha (int);
    void updateFloatPrecision (int);
    void updateFinalText ();

private:
    QString getFinalText ();

private:
    QFont monospace_font;
    QString initial_string;
    QString prefix;
    QString suffix;
    ColorButton *new_color_button;
    ColorButton *initial_color_button;
    QSlider *hue_slider;
    QSlider *saturation_slider;
    QSlider *value_slider;
    QSlider *alpha_slider;
    QTextEdit *final_text_edit;
    int text_format;
    int float_precision;
    QColor color;
    double hue;
    ColorPicker *color_picker;
    QButtonGroup *alpha_flag_button_group;
    QButtonGroup *mode_button_group;
};

#endif
