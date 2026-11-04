#ifndef SUDOKUBOARD_H
#define SUDOKUBOARD_H

#include <QWidget>
#include <QGridLayout>
#include <QVector>
#include <QStack>
#include <QSet>
#include <QElapsedTimer>
#include <QTimer>

class SudokuCell;

struct SudokuMove {
    int row;
    int col;
    int oldValue;
    int newValue;
    QSet<int> oldNotes;
    QSet<int> newNotes;
};

class SudokuBoard : public QWidget
{
    Q_OBJECT

public:
    explicit SudokuBoard(QWidget *parent = nullptr);

    void nuevoJuego();
    void setDificultad(int celdasVisibles);
    void setNotasMode(bool enabled);
    bool notasMode() const { return m_notasMode; }

    bool undo();
    bool redo();
    bool canUndo() const { return !m_undoStack.isEmpty(); }
    bool canRedo() const { return !m_redoStack.isEmpty(); }

    int tiempoTranscurrido() const;
    bool juegoGanado() const { return m_ganado; }

    void limpiarConflicto(int fila, int col, int num);

    // Entrada desde el teclado numerico en pantalla (celda seleccionada)
    void ingresarNumeroEnSeleccion(int num);
    void borrarSeleccion();

signals:
    void juegoCompletado(int tiempo);
    void movimientoHecho();
    void conflictoDetectado(int fila, int col, int num);
    void notasModeChanged(bool enabled);
    void tiempoActualizado(int segundos);

private slots:
    void onCeldaClicked(int row, int col);
    void onNumeroIngresado(int row, int col, int num);
    void onCeldaLimpiada(int row, int col);
    void onTimerTick();

private:
    void generarSolucion();
    void colocarPistas(int celdasVisibles);
    int contarSoluciones(QVector<QVector<int>> &tablero, int limite) const;
    void registrarMovimiento(const SudokuMove &move);
    void aplicarEstado(int fila, int col, int valor, const QSet<int> &notas);
    void recalcularConflictos();
    bool esValidoEnTablero(const QVector<QVector<int>> &tablero, int fila, int col, int num) const;
    bool resolver(QVector<QVector<int>> &tablero);
    bool tieneConflicto(int fila, int col) const;
    void verificarVictoria();
    void actualizarEstadoCelda(int fila, int col);
    void limpiarResaltado();

    static const int TAMANIO = 9;

    QVector<QVector<int>> m_solucion;
    QVector<QVector<bool>> m_esPista;
    QVector<QVector<SudokuCell*>> m_celdas;
    QGridLayout *m_gridLayout = nullptr;
    int m_celdasVisibles = 45;
    bool m_notasMode = false;
    bool m_ganado = false;

    QStack<SudokuMove> m_undoStack;
    QStack<SudokuMove> m_redoStack;
    static const int MAX_UNDO = 100;

    QElapsedTimer m_timer;
    QTimer *m_timerTick = nullptr;
    int m_tiempoSegundos = 0;
    int m_filaSel = -1;
    int m_colSel = -1;
};

#endif
