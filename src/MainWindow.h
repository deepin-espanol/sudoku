#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DLabel>
#include <DPushButton>
#include <DComboBox>

DWIDGET_USE_NAMESPACE

class SudokuBoard;
class SoundManager;
class SettingsDialog;

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onNuevoJuego();
    void onUndo();
    void onRedo();
    void onNotasToggle();
    void onDificultadCambiada(int index);
    void onJuegoCompletado(int tiempo);
    void onTiempoActualizado(int segundos);
    void onAbrirSettings();

private:
    void setupUI();
    void setupShortcuts();
    void updateTitlebarMenu();

    SudokuBoard *m_board = nullptr;
    DLabel *m_labelDificultad = nullptr;
    DLabel *m_labelTimer = nullptr;
    DPushButton *m_btnUndo = nullptr;
    DPushButton *m_btnRedo = nullptr;
    DPushButton *m_btnNotas = nullptr;
    DPushButton *m_btnNuevo = nullptr;
    DComboBox *m_comboDificultad = nullptr;
    SoundManager *m_soundManager = nullptr;
    SettingsDialog *m_settingsDialog = nullptr;
};

#endif
