#include <DApplication>
#include <DWidgetUtil>
#include <QApplication>
#include <QCommandLineParser>
#include "MainWindow.h"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication app(argc, argv);
    app.setApplicationName("sudoku-dtk");
    app.setApplicationDisplayName(QObject::tr("Sudoku"));
    app.setApplicationVersion("0.2.0");
    app.setOrganizationName("deepines");

    app.loadTranslator();

    MainWindow w;
    w.show();
    Dtk::Widget::moveToCenter(&w);

    return app.exec();
}
