#include "mainwindow.h"
#include "colorpicker.h"
#include "colorbutton.h"

#include <stdio.h>

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QButtonGroup>
#include <QCheckBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QSlider>
#include <QComboBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QTextStream>


MainWindow::MainWindow (const QString &initial_string)
    : monospace_font ("Courier"), initial_string (initial_string), text_format (0), float_precision (1)
{
    setWindowFlags (Qt::FramelessWindowHint);
    setFrameStyle (QFrame::Panel | QFrame::Raised);

    QHBoxLayout *hlayout;
    QRadioButton *radio_button;
    QCheckBox *check_box;
    QSpinBox *spin_box;

    QVBoxLayout *layout = new QVBoxLayout (this);

    hlayout = new QHBoxLayout ();

    {
	color_picker = new ColorPicker (this);
	connect (color_picker, SIGNAL (colorChanged (const QColor&, double)), this, SLOT (updateColor (const QColor&, double)));
	hlayout->addWidget (color_picker, 1);
    }

    {
	QVBoxLayout *vsublayout = new QVBoxLayout ();
	vsublayout->addWidget (new QLabel ("A:", this));
	vsublayout->addWidget (alpha_slider = new QSlider (this), 1);
	alpha_slider->setRange (0, 65535);
	alpha_slider->setValue (alpha_slider->maximum ()*color.alpha ()/255.0);
	connect (alpha_slider, SIGNAL (valueChanged (int)), this, SLOT (updateAlpha (int)));
	hlayout->addLayout (vsublayout);
    }

    {
	QVBoxLayout *vsublayout = new QVBoxLayout ();

	{
	    QHBoxLayout *hsublayout = new QHBoxLayout ();
	    hsublayout->addWidget (new_color_button = new ColorButton (this), 1);
	    QPushButton *button = new QPushButton ("<-", this);
	    connect (button, SIGNAL (clicked ()), this, SLOT (recognizeInitialString ()));
	    hsublayout->addWidget (button);
	    hsublayout->addWidget (initial_color_button = new ColorButton (this), 1);
	    vsublayout->addLayout (hsublayout);
	}

	{
	    QGridLayout *grid_layout = new QGridLayout ();
	    grid_layout->addWidget (new QLabel ("H:", this), 0, 0);
	    grid_layout->addWidget (new QLabel ("S:", this), 1, 0);
	    grid_layout->addWidget (new QLabel ("V:", this), 2, 0);
	    grid_layout->addWidget (hue_slider = new QSlider (Qt::Horizontal, this), 0, 1);
	    hue_slider->setRange (0, 65535);
	    grid_layout->addWidget (saturation_slider = new QSlider (Qt::Horizontal, this), 1, 1);
	    saturation_slider->setRange (0, 65535);
	    grid_layout->addWidget (value_slider = new QSlider (Qt::Horizontal, this), 2, 1);
	    value_slider->setRange (0, 65535);
	    vsublayout->addLayout (grid_layout);
	}

	{
	    alpha_flag_button_group = new QButtonGroup (this);
	    mode_button_group = new QButtonGroup (this);
	    QGroupBox *mode_group_box = new QGroupBox ("Mode", this);
	    QGridLayout *mode_group_box_grid_layout = new QGridLayout (mode_group_box);
	    int row = 0;
	    mode_group_box_grid_layout->addWidget (check_box = new QCheckBox ("No alpha", this), row, 0);
	    alpha_flag_button_group->addButton (check_box, 0x00);
	    mode_group_box_grid_layout->addWidget (check_box = new QCheckBox ("Use alpha", this), row, 1);
	    alpha_flag_button_group->addButton (check_box, 0x100);
	    ++row;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("0xff007fff", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x00);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("0xff007f33", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x100);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("0xFF007FFF", this), row + 1, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x01);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("0xFF007F33", this), row + 1, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x101);
	    QComboBox *component_order_combo_box = new QComboBox (this);
	    component_order_combo_box->addItem ("0xRRGGBBAA");
	    component_order_combo_box->addItem ("0xAARRGGBB");
	    component_order_combo_box->addItem ("0xBBGGRRAA");
	    component_order_combo_box->addItem ("0xAABBGGRR");
	    mode_group_box_grid_layout->addWidget (component_order_combo_box, row, 2, 2, 1);
	    row += 2;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("(0xff, 0x00, 0x7f)", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x02);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("(0xff, 0x00, 0x7f, 0x33)", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x102);
	    ++row;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("(0xFF, 0x00, 0x7F)", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x03);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("(0xFF, 0x00, 0x7F, 0x33)", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x103);
	    ++row;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("(255, 0, 127)", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x04);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("(255, 0, 127, 51)", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x104);
	    ++row;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("(1.00, 0.00, 0.49)", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x05);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("(1.00, 0.00, 0.49, 0.20)", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x105);
	    mode_group_box_grid_layout->addWidget (spin_box = new QSpinBox (this), row, 2);
	    spin_box->setRange (1, 14);
	    spin_box->setValue (float_precision);
	    connect (spin_box, SIGNAL (valueChanged (int)), this, SLOT (updateFloatPrecision (int)));
	    spin_box->setToolTip ("Precision (number of digits after decimal point)");
	    ++row;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("#f08", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x06);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("#f083", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x106);
	    ++row;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("#F08", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x07);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("#F083", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x107);
	    ++row;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("#ff007f", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x08);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("[20]#ff007f", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x108);
	    ++row;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("#FF007F", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x09);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("[20]#FF007F", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x109);
	    ++row;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("ffff/0000/7fc2", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x0A);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("ffff/0000/7fc2/3333", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x10A);
	    ++row;
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("FFFF/0000/7FC2", this), row, 0);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x0B);
	    mode_group_box_grid_layout->addWidget (radio_button = new QRadioButton ("FFFF/0000/7FC2/3333", this), row, 1);
	    radio_button->setFont (monospace_font);
	    mode_button_group->addButton (radio_button, 0x10B);
	    ++row;
	    mode_group_box_grid_layout->setRowStretch (row, 1);
	    vsublayout->addWidget (mode_group_box);

	    alpha_flag_button_group->button (text_format & 0x100)->setChecked (true); // TODO: Check for NULL
	    mode_button_group->button (text_format)->setChecked (true); // TODO: Check for NULL

	    connect (alpha_flag_button_group, SIGNAL (buttonClicked (int)), this, SLOT (updateAlphaFlag (int)));
	    connect (mode_button_group, SIGNAL (buttonClicked (int)), this, SLOT (updateTextFormat (int)));
	}

	// vsublayout->addStretch (1);

	{
	    QFrame *frame = new QFrame (this);
	    frame->setFrameStyle (QFrame::Box | QFrame::Sunken);
	    QVBoxLayout *sublayout = new QVBoxLayout (frame);
	    final_text_edit = new QTextEdit (this);
	    final_text_edit->setFont (monospace_font);
	    final_text_edit->setReadOnly (true);
	    sublayout->addWidget (final_text_edit);
	    vsublayout->addWidget (frame, 1);
	}

	hlayout->addLayout (vsublayout);
    }

    layout->addLayout (hlayout);


    hlayout = new QHBoxLayout ();
    QPushButton *decline_button = new QPushButton ("Decline", this);
    connect (decline_button, SIGNAL (clicked ()), this, SLOT (decline ()));
    hlayout->addWidget (decline_button);
    hlayout->addStretch (1);
    QPushButton *accept_button = new QPushButton ("Accept", this);
    connect (accept_button, SIGNAL (clicked ()), this, SLOT (accept ()));
    hlayout->addWidget (accept_button);
    layout->addLayout (hlayout);

    recognizeInitialString ();
}
MainWindow::~MainWindow ()
{
}
void MainWindow::recognizeInitialString ()
{
    QRegExp re;
    QColor color (0, 0, 0);
    if ((re = QRegExp ("(.*)\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*\\)(.*)")).exactMatch (initial_string)) {
	prefix = re.cap (1);
	color = QColor (qMax (qMin (re.cap (2).toInt (NULL, 10), 255), 0),
			qMax (qMin (re.cap (3).toInt (NULL, 10), 255), 0),
			qMax (qMin (re.cap (4).toInt (NULL, 10), 255), 0));
	suffix = re.cap (5);
	text_format = 0x4;
    } else if ((re = QRegExp ("(.*)\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*\\)(.*)")).exactMatch (initial_string)) {
	prefix = re.cap (1);
	color = QColor (qMax (qMin (re.cap (2).toInt (NULL, 10), 255), 0),
			qMax (qMin (re.cap (3).toInt (NULL, 10), 255), 0),
			qMax (qMin (re.cap (4).toInt (NULL, 10), 255), 0),
			qMax (qMin (re.cap (5).toInt (NULL, 10), 255), 0));
	suffix = re.cap (6);
	text_format = 0x104;
    } else if ((re = QRegExp ("(.*)\\(\\s*0x([0-9a-f]{1,2})\\s*,\\s*0x([0-9a-f]{1,2})\\s*,\\s*0x([0-9a-f]{1,2})\\s*,\\s*0x([0-9a-f]{1,2})\\s*\\)(.*)")).exactMatch (initial_string)) {
	prefix = re.cap (1);
	color = QColor (qMax (qMin (re.cap (2).toInt (NULL, 16), 255), 0),
			qMax (qMin (re.cap (3).toInt (NULL, 16), 255), 0),
			qMax (qMin (re.cap (4).toInt (NULL, 16), 255), 0),
			qMax (qMin (re.cap (5).toInt (NULL, 16), 255), 0));
	suffix = re.cap (6);
	text_format = 0x102;
    } else if ((re = QRegExp ("(.*)\\(\\s*0x([0-9a-f]{1,2})\\s*,\\s*0x([0-9a-f]{1,2})\\s*,\\s*0x([0-9a-f]{1,2})\\s*\\)(.*)")).exactMatch (initial_string)) {
	prefix = re.cap (1);
	color = QColor (qMax (qMin (re.cap (2).toInt (NULL, 16), 255), 0),
			qMax (qMin (re.cap (3).toInt (NULL, 16), 255), 0),
			qMax (qMin (re.cap (4).toInt (NULL, 16), 255), 0));
	suffix = re.cap (5);
	text_format = 2;
    } else if ((re = QRegExp ("(.*)0x([0-9a-f]{8})(.*)")).exactMatch (initial_string)) {
	prefix = re.cap (1);
	quint32 color_int = re.cap (2).toUInt (NULL, 16);
	color = QColor (int ((color_int >> 24) & 0xff),
			int ((color_int >> 16) & 0xff),
			int ((color_int >> 8) & 0xff),
			int (color_int  & 0xff));
	suffix = re.cap (3);
	text_format = 0x100;
    } else {
	prefix = initial_string;
	suffix = "";
	updateFinalText ();
	return;
    }
    initial_color_button->setColor (color);
    new_color_button->setColor (color);
    color_picker->setColor (color.red ()/255.0, color.green ()/255.0, color.blue ()/255.0);
    QAbstractButton *button = mode_button_group->button (text_format);
    if (button)
	button->setChecked (true);
    button = alpha_flag_button_group->button (text_format & 0x100);
    if (button)
	button->setChecked (true);
    updateFinalText ();
}
void MainWindow::accept ()
{
    QTextStream (stdout) << getFinalText ();
    qApp->exit (0);
}
void MainWindow::decline ()
{
    qApp->exit (1);
}
void MainWindow::updateAlphaFlag (int new_alpha_flag)
{
    if ((text_format & 0x100) != new_alpha_flag) {
	text_format = (text_format & 0xff) | new_alpha_flag;
	mode_button_group->button (text_format)->setChecked (true); // TODO: Check for NULL
	updateFinalText ();
    }
}
void MainWindow::updateTextFormat (int new_text_format)
{
    if (text_format != new_text_format) {
	text_format = new_text_format;
	if (text_format & 0x100)
	    alpha_flag_button_group->button (0x100)->setChecked (true); // TODO: Check for NULL
	else
	    alpha_flag_button_group->button (0)->setChecked (true); // TODO: Check for NULL
	updateFinalText ();
    }
}
void MainWindow::updateFinalText ()
{
    new_color_button->setColor (color);
    int saturation = color.saturation ();
    int value = color.value ();
    hue_slider->setValue (hue_slider->maximum ()*hue);
    saturation_slider->setValue (saturation_slider->maximum ()*saturation/255.0);
    value_slider->setValue (value_slider->maximum ()*value/255.0);
    final_text_edit->setText (getFinalText ());
}
QString MainWindow::getFinalText ()
{
    QString final_value = "";
    int base_format = text_format & 0xff;
    int alpha_flag = text_format & 0x100;
    switch (base_format) {
    case 0: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("0x%02x%02x%02x%02x", color.red (), color.green (), color.blue (), color.alpha ());
	else
	    final_value = QString ().sprintf ("0x%02x%02x%02xff", color.red (), color.green (), color.blue ());
    } break;
    case 1: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("0x%02X%02X%02X%02X", color.red (), color.green (), color.blue (), color.alpha ());
	else
	    final_value = QString ().sprintf ("0x%02X%02X%02XFF", color.red (), color.green (), color.blue ());
    } break;
    case 2: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("(0x%02x, 0x%02x, 0x%02x, 0x%02x)", color.red (), color.green (), color.blue (), color.alpha ());
	else
	    final_value = QString ().sprintf ("(0x%02x, 0x%02x, 0x%02x)", color.red (), color.green (), color.blue ());
    } break;
    case 3: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("(0x%02X, 0x%02X, 0x%02X, 0x%02X)", color.red (), color.green (), color.blue (), color.alpha ());
	else
	    final_value = QString ().sprintf ("(0x%02X, 0x%02X, 0x%02X)", color.red (), color.green (), color.blue ());
    } break;
    case 4: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("(%d, %d, %d, %d)", color.red (), color.green (), color.blue (), color.alpha ());
	else
	    final_value = QString ().sprintf ("(%d, %d, %d)", color.red (), color.green (), color.blue ());
    } break;
    case 5: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("(%.*lf, %.*lf, %.*lf, %.*lf)",
	    float_precision, color.red ()/255.0,
	    float_precision, color.green ()/255.0,
	    float_precision, color.blue ()/255.0,
	    float_precision, color.alpha ()/255.0);
	else
	    final_value = QString ().sprintf ("(%.*lf, %.*lf, %.*lf)",
	    float_precision, color.red ()/255.0,
	    float_precision, color.green ()/255.0,
	    float_precision, color.blue ()/255.0);
    } break;
    case 6: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("#%01x%01x%01x%01x",
					      qMin ((color.red () + 8)/16, 15),
					      qMin ((color.green () + 8)/16, 15),
					      qMin ((color.blue () + 8)/16, 15),
					      qMin ((color.alpha () + 8)/16, 15));
	else
	    final_value = QString ().sprintf ("#%01x%01x%01x",
					      qMin ((color.red () + 8)/16, 15),
					      qMin ((color.green () + 8)/16, 15),
					      qMin ((color.blue () + 8)/16, 15));
    } break;
    case 7: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("#%01X%01X%01X%01X",
					      qMin ((color.red () + 8)/16, 15),
					      qMin ((color.green () + 8)/16, 15),
					      qMin ((color.blue () + 8)/16, 15),
					      qMin ((color.alpha () + 8)/16, 15));
	else
	    final_value = QString ().sprintf ("#%01X%01X%01X",
					      qMin ((color.red () + 8)/16, 15),
					      qMin ((color.green () + 8)/16, 15),
					      qMin ((color.blue () + 8)/16, 15));
    } break;
    case 8: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("#%02x%02x%02x%02x", color.red (), color.green (), color.blue (), color.alpha ());
	else
	    final_value = QString ().sprintf ("#%02x%02x%02x", color.red (), color.green (), color.blue ());
    } break;
    case 9: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("#%02X%02X%02X%02X", color.red (), color.green (), color.blue (), color.alpha ());
	else
	    final_value = QString ().sprintf ("#%02X%02X%02X", color.red (), color.green (), color.blue ());
    } break;
    case 10: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("%02x%02x/%02x%02x/%02x%02x/%02x%02x",
					      color.red (), color.red (),
					      color.green (), color.green (),
					      color.blue (), color.blue (),
					      color.alpha (), color.alpha ());
	else
	    final_value = QString ().sprintf ("%02x%02x/%02x%02x/%02x%02x",
					      color.red (), color.red (),
					      color.green (), color.green (),
					      color.blue (), color.blue ());
    } break;
    case 11: {
	if (alpha_flag)
	    final_value = QString ().sprintf ("%02X%02X/%02X%02X/%02X%02X/%02X%02X",
					      color.red (), color.red (),
					      color.green (), color.green (),
					      color.blue (), color.blue (),
					      color.alpha (), color.alpha ());
	else
	    final_value = QString ().sprintf ("%02X%02X/%02X%02X/%02X%02X",
					      color.red (), color.red (),
					      color.green (), color.green (),
					      color.blue (), color.blue ());
    } break;
    }
    return prefix + final_value + suffix;
}
void MainWindow::updateColor (const QColor &new_color, double new_hue)
{
    color = new_color;
    hue = new_hue;
    updateFinalText ();
}
void MainWindow::updateAlpha (int new_alpha)
{
    color.setAlpha (new_alpha*255.0/65535.0);
    updateFinalText ();
}
void MainWindow::updateFloatPrecision (int new_float_precision)
{
    float_precision = new_float_precision;
    updateFinalText ();
}
void MainWindow::keyPressEvent (QKeyEvent *event)
{
    int key = event->key ();
    if (key == Qt::Key_Escape) {
	decline ();
    } else if ((event->modifiers () & Qt::ControlModifier) &&
	       ((key == Qt::Key_Return) || (key == Qt::Key_S))) {
	accept ();
    } else if (key == Qt::Key_Alt) {
	color_picker->setMarkupMode (true);
    }
}
void MainWindow::keyReleaseEvent (QKeyEvent *event)
{
    int key = event->key ();
    if (key == Qt::Key_Alt) {
	color_picker->setMarkupMode (false);
    }
}
