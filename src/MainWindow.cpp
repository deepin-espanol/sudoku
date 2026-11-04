#include "MainWindow.h"
#include "SudokuBoard.h"
#include "SoundManager.h"
#include "SettingsDialog.h"

#include <DTitlebar>
#include <DWidgetUtil>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    m_soundManager = new SoundManager(this);
    setupUI();
    setupShortcuts();
    updateTitlebarMenu();
    m_labelDificultad->setText(tr("Dificultad: Media"));
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    setFixedSize(520, 660);
    setWindowTitle(tr("Sudoku"));

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

    m_comboDificultad = new DComboBox();
    m_comboDificultad->addItem(tr("Facil (55 celdas)"), 55);
    m_comboDificultad->addItem(tr("Media (45 celdas)"), 45);
    m_comboDificultad->addItem(tr("Dificil (35 celdas)"), 35);
    m_comboDificultad->setCurrentIndex(1);

    m_btnUndo->setEnabled(false);
    m_btnRedo->setEnabled(false);
    m_btnNotas->setCheckable(true);

    // Sin foco: asi pulsar un boton no quita la seleccion de la celda
    for (auto *btn : {m_btnNuevo, m_btnUndo, m_btnRedo, m_btnNotas})
        btn->setFocusPolicy(Qt::NoFocus);
    m_comboDificultad->setFocusPolicy(Qt::NoFocus);

    toolLayout->addWidget(m_btnNuevo);
    toolLayout->addWidget(m_btnUndo);
    toolLayout->addWidget(m_btnRedo);
    toolLayout->addWidget(m_btnNotas);
    toolLayout->addWidget(m_comboDificultad);
    toolLayout->addStretch();
    mainLayout->addLayout(toolLayout);

    m_board = new SudokuBoard(this);
    mainLayout->addWidget(m_board, 0, Qt::AlignCenter);

    // Teclado numerico en pantalla para jugar solo con el raton
    auto *padLayout = new QHBoxLayout();
    padLayout->setSpacing(4);
    padLayout->addStretch();
    for (int n = 1; n <= 9; ++n) {
        auto *btn = new DPushButton(QString::number(n));
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setFixedSize(40, 36);
        connect(btn, &DPushButton::clicked, this, [this, n]() {
            m_board->ingresarNumeroEnSeleccion(n);
        });
        padLayout->addWidget(btn);
    }
    auto *btnBorrar = new DPushButton(tr("Borrar"));
    btnBorrar->setFocusPolicy(Qt::NoFocus);
    btnBorrar->setFixedHeight(36);
    connect(btnBorrar, &DPushButton::clicked, this, [this]() { m_board->borrarSeleccion(); });
    padLayout->addWidget(btnBorrar);
    padLayout->addStretch();
    mainLayout->addLayout(padLayout);

    setCentralWidget(centralWidget);

    connect(m_btnNuevo, &DPushButton::clicked, this, &MainWindow::onNuevoJuego);
    connect(m_btnUndo, &DPushButton::clicked, this, &MainWindow::onUndo);
    connect(m_btnRedo, &DPushButton::clicked, this, &MainWindow::onRedo);
    connect(m_btnNotas, &DPushButton::clicked, this, &MainWindow::onNotasToggle);
    connect(m_comboDificultad, QOverload<int>::of(&DComboBox::currentIndexChanged),
            this, &MainWindow::onDificultadCambiada);

    connect(m_board, &SudokuBoard::movimientoHecho, this, [this]() {
        m_btnUndo->setEnabled(m_board->canUndo());
        m_btnRedo->setEnabled(m_board->canRedo());
    });
    connect(m_board, &SudokuBoard::juegoCompletado, this, &MainWindow::onJuegoCompletado);
    connect(m_board, &SudokuBoard::conflictoDetectado, this, [this]() {
        m_soundManager->playError();
    });
    connect(m_board, &SudokuBoard::tiempoActualizado, this, &MainWindow::onTiempoActualizado);
    connect(m_board, &SudokuBoard::notasModeChanged, this, [this](bool enabled) {
        m_btnNotas->setChecked(enabled);
    });
}

void MainWindow::setupShortcuts()
{
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_N), this, [this]() { onNuevoJuego(); });
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), this, [this]() { onUndo(); });
    new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z), this, [this]() { onRedo(); });
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), this, [this]() { close(); });
}

void MainWindow::updateTitlebarMenu()
{
    auto *tb = titlebar();
    auto *menu = tb->menu();

    if (!menu) {
        menu = new QMenu(this);
        tb->setMenu(menu);
    }

    auto *settingsAction = menu->addAction(tr("Preferencias..."));
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onAbrirSettings);
}

void MainWindow::onNuevoJuego()
{
    m_board->nuevoJuego();
    m_btnUndo->setEnabled(false);
    m_btnRedo->setEnabled(false);
    m_soundManager->playPlace();
}

void MainWindow::onDificultadCambiada(int index)
{
    QString dificultad;
    int celdas = 45;
    switch (index) {
    case 0: dificultad = tr("Facil"); celdas = 55; break;
    case 2: dificultad = tr("Dificil"); celdas = 35; break;
    default: dificultad = tr("Media"); celdas = 45; break;
    }
    m_labelDificultad->setText(tr("Dificultad: %1").arg(dificultad));
    m_board->setDificultad(celdas);
    m_btnUndo->setEnabled(false);
    m_btnRedo->setEnabled(false);
    m_soundManager->playPlace();
}

void MainWindow::onUndo()
{
    if (m_board->undo())
        m_soundManager->playPlace();
}

void MainWindow::onRedo()
{
    if (m_board->redo())
        m_soundManager->playPlace();
}

void MainWindow::onNotasToggle()
{
    m_board->setNotasMode(!m_board->notasMode());
    m_soundManager->playPlace();
}

void MainWindow::onJuegoCompletado(int tiempo)
{
    m_soundManager->playVictory();

    int mins = tiempo / 60;
    int segs = tiempo % 60;
    QString timeStr = QString("%1:%2")
        .arg(mins, 2, 10, QChar('0'))
        .arg(segs, 2, 10, QChar('0'));

    QMessageBox::information(this, tr("Sudoku Completado"),
        tr("Felicidades! Has completado el Sudoku!\n\nTiempo: %1").arg(timeStr));
}

void MainWindow::onTiempoActualizado(int segundos)
{
    int mins = segundos / 60;
    int secs = segundos % 60;
    m_labelTimer->setText(QString("%1:%2")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0')));
}

void MainWindow::onAbrirSettings()
{
    if (!m_settingsDialog) {
        m_settingsDialog = new SettingsDialog(m_soundManager, this);
    }
    m_settingsDialog->show();
    m_settingsDialog->raise();
    m_settingsDialog->activateWindow();

    Dtk::Widget::moveToCenter(m_settingsDialog);
}
