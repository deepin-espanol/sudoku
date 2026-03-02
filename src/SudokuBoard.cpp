#include "SudokuBoard.h"
#include "SudokuCell.h"

#include <QRandomGenerator>
#include <QGridLayout>

SudokuBoard::SudokuBoard(QWidget *parent)
    : QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(0);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);

    m_solucion.resize(TAMANIO, QVector<int>(TAMANIO, 0));
    m_esPista.resize(TAMANIO, QVector<bool>(TAMANIO, false));
    m_celdas.resize(TAMANIO, QVector<SudokuCell*>(TAMANIO, nullptr));

    for (int i = 0; i < TAMANIO; ++i) {
        for (int j = 0; j < TAMANIO; ++j) {
            auto *cell = new SudokuCell(i, j, this);

            connect(cell, &SudokuCell::cellClicked, this, &SudokuBoard::onCeldaClicked);
            connect(cell, &SudokuCell::numberEntered, this, &SudokuBoard::onNumeroIngresado);
            connect(cell, &SudokuCell::cellCleared, this, &SudokuBoard::onCeldaLimpiada);

            m_celdas[i][j] = cell;
            m_gridLayout->addWidget(cell, i, j);
        }
    }

    nuevoJuego();
}

void SudokuBoard::nuevoJuego()
{
    generarSolucion();
    colocarPistas(m_celdasVisibles);

    for (int i = 0; i < TAMANIO; ++i) {
        for (int j = 0; j < TAMANIO; ++j) {
            auto *cell = m_celdas[i][j];
            cell->setNumber(m_solucion[i][j]);
            cell->setReadOnly(m_esPista[i][j]);
            cell->setConflict(false);
            cell->clearNotes();
            actualizarEstadoCelda(i, j);
        }
    }
}

void SudokuBoard::setDificultad(int celdasVisibles)
{
    m_celdasVisibles = celdasVisibles;
    nuevoJuego();
}

void SudokuBoard::onCeldaClicked(int row, int col)
{
    limpiarResaltado();

    for (int i = 0; i < TAMANIO; ++i) {
        m_celdas[row][i]->setHighlighted(true);
        m_celdas[i][col]->setHighlighted(true);
    }
    int bRow = (row / 3) * 3, bCol = (col / 3) * 3;
    for (int i = bRow; i < bRow + 3; ++i)
        for (int j = bCol; j < bCol + 3; ++j)
            m_celdas[i][j]->setHighlighted(true);

    int num = m_celdas[row][col]->number();
    if (num != 0) {
        for (int i = 0; i < TAMANIO; ++i)
            for (int j = 0; j < TAMANIO; ++j)
                if (m_celdas[i][j]->number() == num)
                    m_celdas[i][j]->setHighlighted(true);
    }
}

void SudokuBoard::onNumeroIngresado(int row, int col, int num)
{
    if (m_esPista[row][col]) return;

    auto *cell = m_celdas[row][col];

    if (cell->number() == num) {
        cell->setNumber(0);
        actualizarEstadoCelda(row, col);
        return;
    }

    cell->setNumber(num);
    cell->clearNotes();

    bool conflicto = tieneConflicto(row, col);
    cell->setConflict(conflicto);
    if (conflicto) {
        emit conflictoDetectado(row, col, num);
    }

    actualizarEstadoCelda(row, col);
}

void SudokuBoard::onCeldaLimpiada(int row, int col)
{
    if (m_esPista[row][col]) return;

    auto *cell = m_celdas[row][col];

    cell->setNumber(0);
    cell->setConflict(false);

    actualizarEstadoCelda(row, col);
}

void SudokuBoard::generarSolucion()
{
    for (int i = 0; i < TAMANIO; ++i)
        for (int j = 0; j < TAMANIO; ++j)
            m_solucion[i][j] = 0;

    resolver(m_solucion);
}

bool SudokuBoard::resolver(QVector<QVector<int>> &tablero)
{
    for (int i = 0; i < TAMANIO; ++i) {
        for (int j = 0; j < TAMANIO; ++j) {
            if (tablero[i][j] == 0) {
                QVector<int> nums;
                for (int n = 1; n <= 9; ++n) nums.append(n);

                for (int k = nums.size() - 1; k > 0; --k) {
                    int r = QRandomGenerator::global()->bounded(k + 1);
                    nums.swapItemsAt(k, r);
                }

                for (int num : nums) {
                    if (esValidoEnTablero(tablero, i, j, num)) {
                        tablero[i][j] = num;
                        if (resolver(tablero)) return true;
                        tablero[i][j] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

bool SudokuBoard::esValidoEnTablero(const QVector<QVector<int>> &tablero, int fila, int col, int num) const
{
    for (int j = 0; j < TAMANIO; ++j)
        if (tablero[fila][j] == num) return false;

    for (int i = 0; i < TAMANIO; ++i)
        if (tablero[i][col] == num) return false;

    int bRow = (fila / 3) * 3;
    int bCol = (col / 3) * 3;
    for (int i = bRow; i < bRow + 3; ++i)
        for (int j = bCol; j < bCol + 3; ++j)
            if (tablero[i][j] == num) return false;

    return true;
}

bool SudokuBoard::tieneConflicto(int fila, int col) const
{
    int num = m_celdas[fila][col]->number();
    if (num == 0) return false;

    for (int j = 0; j < TAMANIO; ++j) {
        if (j == col) continue;
        if (m_celdas[fila][j]->number() == num) return true;
    }

    for (int i = 0; i < TAMANIO; ++i) {
        if (i == fila) continue;
        if (m_celdas[i][col]->number() == num) return true;
    }

    int bRow = (fila / 3) * 3, bCol = (col / 3) * 3;
    for (int i = bRow; i < bRow + 3; ++i) {
        for (int j = bCol; j < bCol + 3; ++j) {
            if (i == fila && j == col) continue;
            if (m_celdas[i][j]->number() == num) return true;
        }
    }

    return false;
}

void SudokuBoard::colocarPistas(int celdasVisibles)
{
    QVector<QPair<int,int>> todas;
    for (int i = 0; i < TAMANIO; ++i)
        for (int j = 0; j < TAMANIO; ++j)
            todas.append({i, j});

    for (int k = todas.size() - 1; k > 0; --k) {
        int r = QRandomGenerator::global()->bounded(k + 1);
        todas.swapItemsAt(k, r);
    }

    for (int i = 0; i < TAMANIO; ++i)
        for (int j = 0; j < TAMANIO; ++j)
            m_esPista[i][j] = false;

    for (int k = 0; k < celdasVisibles && k < todas.size(); ++k) {
        auto [f, c] = todas[k];
        m_esPista[f][c] = true;
    }
}

void SudokuBoard::actualizarEstadoCelda(int fila, int col)
{
    auto *cell = m_celdas[fila][col];

    if (!cell->isReadOnly() && cell->number() != 0) {
        bool conflicto = tieneConflicto(fila, col);
        cell->setConflict(conflicto);
    }
}

void SudokuBoard::limpiarResaltado()
{
    for (int i = 0; i < TAMANIO; ++i)
        for (int j = 0; j < TAMANIO; ++j)
            m_celdas[i][j]->setHighlighted(false);
}
