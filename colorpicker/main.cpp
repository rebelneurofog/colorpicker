#include "mainwindow.h"

#include <QApplication>
#include <QtDebug>

static void silencedMessageOutput (QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit ();
    switch (type) {
    case QtDebugMsg:
        // fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData (), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        // fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData (), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        // fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData (), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf (stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData (), context.file, context.line, context.function);
        abort ();
    }
}


int main (int argc, char **argv)
{
    qInstallMessageHandler (silencedMessageOutput);
    QString initial_string ("");
    if (argc > 1)
	initial_string = argv[1];
    QApplication a (argc, argv);
    MainWindow window (initial_string);
    window.show ();
    return a.exec ();
}
