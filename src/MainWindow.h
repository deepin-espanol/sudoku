#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DLabel>
#include <DPushButton>

DWIDGET_USE_NAMESPACE

class SudokuBoard;
class SoundManager;

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onNuevoJuego();
    void onDificultadFacil();
    void onDificultadMedia();
    void onDificultadDificil();
    void onUndo();
    void onRedo();
    void onNotasToggle();
    void onJuegoCompletado(int tiempo);
    void onTiempoActualizado(int segundos);

private:
    void setupUI();
    void setupMenu();

    SudokuBoard *m_board = nullptr;
    DLabel *m_labelDificultad = nullptr;
    DLabel *m_labelTimer = nullptr;
    DPushButton *m_btnUndo = nullptr;
    DPushButton *m_btnRedo = nullptr;
    DPushButton *m_btnNotas = nullptr;
    DPushButton *m_btnNuevo = nullptr;
    SoundManager *m_soundManager = nullptr;
};

#endif
