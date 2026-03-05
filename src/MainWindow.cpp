#include "MainWindow.h"
#include "SudokuBoard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    setupUI();
    setupMenu();
    onDificultadMedia();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    resize(520, 600);
    setWindowTitle(tr("Sudoku"));
    setMinimumSize(480, 540);

    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 8, 12, 12);

    auto *topLayout = new QHBoxLayout();
    m_labelDificultad = new DLabel(tr("Dificultad: Media"));
    m_labelTimer = new DLabel("00:00");
    m_labelTimer->setStyleSheet("font-size: 16px; font-weight: bold; color: palette(button-text);");

    topLayout->addWidget(m_labelDificultad);
    topLayout->addStretch();
    topLayout->addWidget(m_labelTimer);
    mainLayout->addLayout(topLayout);

    auto *toolLayout = new QHBoxLayout();
    toolLayout->setSpacing(6);

    m_btnNuevo = new DPushButton(tr("Nuevo"));
    m_btnUndo = new DPushButton(tr("Deshacer"));
    m_btnRedo = new DPushButton(tr("Rehacer"));
    m_btnNotas = new DPushButton(tr("Notas"));

    m_btnUndo->setEnabled(false);
    m_btnRedo->setEnabled(false);
    m_btnNotas->setCheckable(true);

    toolLayout->addWidget(m_btnNuevo);
    toolLayout->addWidget(m_btnUndo);
    toolLayout->addWidget(m_btnRedo);
    toolLayout->addWidget(m_btnNotas);
    toolLayout->addStretch();
    mainLayout->addLayout(toolLayout);

    m_board = new SudokuBoard(this);
    mainLayout->addWidget(m_board, 0, Qt::AlignCenter);

    setCentralWidget(centralWidget);

    connect(m_btnNuevo, &DPushButton::clicked, this, &MainWindow::onNuevoJuego);
    connect(m_btnUndo, &DPushButton::clicked, this, &MainWindow::onUndo);
    connect(m_btnRedo, &DPushButton::clicked, this, &MainWindow::onRedo);
    connect(m_btnNotas, &DPushButton::clicked, this, &MainWindow::onNotasToggle);

    connect(m_board, &SudokuBoard::movimientoHecho, this, [this]() {
        m_btnUndo->setEnabled(m_board->canUndo());
        m_btnRedo->setEnabled(m_board->canRedo());
    });
    connect(m_board, &SudokuBoard::notasModeChanged, this, [this](bool enabled) {
        m_btnNotas->setChecked(enabled);
    });
}

void MainWindow::setupMenu()
{
    auto *menuBar = this->menuBar();
    auto *juegoMenu = menuBar->addMenu(tr("&Juego"));

    auto *nuevoAction = juegoMenu->addAction(tr("&Nuevo juego"));
    nuevoAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    connect(nuevoAction, &QAction::triggered, this, &MainWindow::onNuevoJuego);

    juegoMenu->addSeparator();

    auto *facil = juegoMenu->addAction(tr("&Facil (35 celdas)"));
    connect(facil, &QAction::triggered, this, &MainWindow::onDificultadFacil);

    auto *media = juegoMenu->addAction(tr("&Media (45 celdas)"));
    connect(media, &QAction::triggered, this, &MainWindow::onDificultadMedia);

    auto *dificil = juegoMenu->addAction(tr("&Dificil (55 celdas)"));
    connect(dificil, &QAction::triggered, this, &MainWindow::onDificultadDificil);

    juegoMenu->addSeparator();

    auto *deshacer = juegoMenu->addAction(tr("&Deshacer"));
    deshacer->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z));
    connect(deshacer, &QAction::triggered, this, &MainWindow::onUndo);

    auto *rehacer = juegoMenu->addAction(tr("&Rehacer"));
    rehacer->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z));
    connect(rehacer, &QAction::triggered, this, &MainWindow::onRedo);

    juegoMenu->addSeparator();

    auto *salir = juegoMenu->addAction(tr("&Salir"));
    salir->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    connect(salir, &QAction::triggered, this, &QWidget::close);
}

void MainWindow::onNuevoJuego()
{
    m_board->nuevoJuego();
    m_btnUndo->setEnabled(false);
    m_btnRedo->setEnabled(false);
}

void MainWindow::onDificultadFacil()
{
    m_labelDificultad->setText(tr("Dificultad: Facil"));
    m_board->setDificultad(35);
    m_btnUndo->setEnabled(false);
    m_btnRedo->setEnabled(false);
}

void MainWindow::onDificultadMedia()
{
    m_labelDificultad->setText(tr("Dificultad: Media"));
    m_board->setDificultad(45);
    m_btnUndo->setEnabled(false);
    m_btnRedo->setEnabled(false);
}

void MainWindow::onDificultadDificil()
{
    m_labelDificultad->setText(tr("Dificultad: Dificil"));
    m_board->setDificultad(55);
    m_btnUndo->setEnabled(false);
    m_btnRedo->setEnabled(false);
}

void MainWindow::onUndo()
{
    m_board->undo();
}

void MainWindow::onRedo()
{
    m_board->redo();
}

void MainWindow::onNotasToggle()
{
    m_board->setNotasMode(!m_board->notasMode());
}
