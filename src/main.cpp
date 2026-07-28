#include <DApplication>
#include <DWidgetUtil>
#include <DAboutDialog>
#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
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

    app.setProductIcon(QIcon(":/icons/sudoku.svg"));
    app.setProductName(QObject::tr("Sudoku"));

    DAboutDialog about;
    about.setProductName(QObject::tr("Sudoku"));
    about.setProductIcon(QIcon(":/icons/sudoku.svg"));
    about.setVersion(DApplication::buildVersion("Version 0.2.0"));
    about.setDescription(QObject::tr("Un juego de Sudoku para Deepin.\n\nGenera puzzles con solucion unica\ny tres niveles de dificultad."));
    about.setWebsiteName("github.com/deepines/sudoku");
    app.setAboutDialog(&about);

    MainWindow w;
    w.show();
    Dtk::Widget::moveToCenter(&w);

    return app.exec();
}
