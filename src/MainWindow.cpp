#include "MainWindow.h"

#include <DLabel>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    setFixedSize(520, 600);
    setWindowTitle(tr("Sudoku"));

    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(new DLabel(tr("Sudoku - en construccion"), centralWidget), 0, Qt::AlignCenter);

    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() = default;
