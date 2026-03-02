#ifndef SUDOKUBOARD_H
#define SUDOKUBOARD_H

#include <QWidget>
#include <QGridLayout>
#include <QVector>

class SudokuCell;

class SudokuBoard : public QWidget
{
    Q_OBJECT

public:
    explicit SudokuBoard(QWidget *parent = nullptr);

    void nuevoJuego();
    void setDificultad(int celdasVisibles);

signals:
    void conflictoDetectado(int fila, int col, int num);

private slots:
    void onCeldaClicked(int row, int col);
    void onNumeroIngresado(int row, int col, int num);
    void onCeldaLimpiada(int row, int col);

private:
    void generarSolucion();
    void colocarPistas(int celdasVisibles);
    bool esValidoEnTablero(const QVector<QVector<int>> &tablero, int fila, int col, int num) const;
    bool resolver(QVector<QVector<int>> &tablero);
    bool tieneConflicto(int fila, int col) const;
    void actualizarEstadoCelda(int fila, int col);
    void limpiarResaltado();

    static const int TAMANIO = 9;

    QVector<QVector<int>> m_solucion;
    QVector<QVector<bool>> m_esPista;
    QVector<QVector<SudokuCell*>> m_celdas;
    QGridLayout *m_gridLayout = nullptr;
    int m_celdasVisibles = 45;
};

#endif
