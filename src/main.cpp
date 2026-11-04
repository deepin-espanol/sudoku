#include <DApplication>
#include <DWidgetUtil>
#include <DAboutDialog>
#include <QApplication>
#include <QIcon>
#include <QLocale>
#include <QTranslator>
#include "MainWindow.h"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication app(argc, argv);
    app.setApplicationName("sudoku-dtk");
    app.setApplicationVersion(QStringLiteral("0.2.2"));
    app.setOrganizationName("deepines");

    // Traducciones de DTK y, encima, las del juego embebidas en :/translations
    app.loadTranslator();
    QTranslator translator;
    if (translator.load(QLocale(), QStringLiteral("sudoku"), QStringLiteral("_"),
                        QStringLiteral(":/translations")))
        app.installTranslator(&translator);

    app.setApplicationDisplayName(QObject::tr("Sudoku"));
    app.setProductIcon(QIcon(":/icons/sudoku.svg"));
    app.setProductName(QObject::tr("Sudoku"));

    DAboutDialog about;
    about.setProductName(QObject::tr("Sudoku"));
    about.setProductIcon(QIcon(":/icons/sudoku.svg"));
    about.setVersion(DApplication::buildVersion(QStringLiteral("0.2.2")));
    about.setDescription(QObject::tr("Un juego de Sudoku para Deepin.\n\nGenera puzzles con solucion unica\ny tres niveles de dificultad."));
    about.setWebsiteName(QStringLiteral("github.com/deepin-espanol/sudoku"));
    about.setWebsiteLink(QStringLiteral("https://github.com/deepin-espanol/sudoku"));
    app.setAboutDialog(&about);

    MainWindow w;
    w.show();
    Dtk::Widget::moveToCenter(&w);

    return app.exec();
}
