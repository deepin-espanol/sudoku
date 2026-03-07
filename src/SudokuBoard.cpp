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

    m_timerTick = new QTimer(this);
    connect(m_timerTick, &QTimer::timeout, this, &SudokuBoard::onTimerTick);

    nuevoJuego();
}

void SudokuBoard::nuevoJuego()
{
    m_timerTick->stop();
    m_tiempoSegundos = 0;
    m_ganado = false;
    m_undoStack.clear();
    m_redoStack.clear();

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

    if (!m_ganado) {
        m_timer.start();
        m_timerTick->start(1000);
    }

    emit tiempoActualizado(0);
}

void SudokuBoard::setDificultad(int celdasVisibles)
{
    m_celdasVisibles = celdasVisibles;
    nuevoJuego();
}

void SudokuBoard::setNotasMode(bool enabled)
{
    m_notasMode = enabled;
    emit notasModeChanged(enabled);
}

bool SudokuBoard::undo()
{
    if (m_undoStack.isEmpty() || m_ganado) return false;

    SudokuMove move = m_undoStack.pop();
    m_redoStack.push(move);

    auto *cell = m_celdas[move.row][move.col];
    if (move.oldValue != 0) {
        cell->setNumber(move.oldValue);
    } else {
        cell->setNumber(0);
        cell->setNotes(move.oldNotes);
    }
    actualizarEstadoCelda(move.row, move.col);
    m_ganado = false;
    emit movimientoHecho();
    return true;
}

bool SudokuBoard::redo()
{
    if (m_redoStack.isEmpty() || m_ganado) return false;

    SudokuMove move = m_redoStack.pop();
    m_undoStack.push(move);

    auto *cell = m_celdas[move.row][move.col];
    if (move.newValue != 0) {
        cell->setNumber(move.newValue);
    } else {
        cell->setNumber(0);
    }
    actualizarEstadoCelda(move.row, move.col);
    m_ganado = false;
    emit movimientoHecho();
    return true;
}

int SudokuBoard::tiempoTranscurrido() const
{
    return m_tiempoSegundos;
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
    if (m_ganado || m_esPista[row][col]) return;

    auto *cell = m_celdas[row][col];

    if (m_notasMode) {
        if (cell->number() != 0) return;
        SudokuMove move;
        move.row = row;
        move.col = col;
        move.oldValue = 0;
        move.newValue = 0;
        move.oldNotes = cell->notes();

        if (cell->hasNote(num))
            cell->setNote(num, false);
        else
            cell->setNote(num, true);

        m_undoStack.push(move);
        m_redoStack.clear();
        if (m_undoStack.size() > MAX_UNDO) m_undoStack.remove(0);
        emit movimientoHecho();
        return;
    }

    if (cell->number() == num) {
        cell->setNumber(0);
        actualizarEstadoCelda(row, col);
        m_redoStack.clear();
        emit movimientoHecho();
        return;
    }

    SudokuMove move;
    move.row = row;
    move.col = col;
    move.oldValue = cell->number();
    move.newValue = num;
    move.oldNotes = cell->notes();

    cell->setNumber(num);
    cell->clearNotes();

    m_undoStack.push(move);
    m_redoStack.clear();
    if (m_undoStack.size() > MAX_UNDO) m_undoStack.remove(0);

    bool conflicto = tieneConflicto(row, col);
    cell->setConflict(conflicto);
    if (conflicto) {
        emit conflictoDetectado(row, col, num);
    }

    actualizarEstadoCelda(row, col);
    emit movimientoHecho();

    if (!conflicto) verificarVictoria();
}

void SudokuBoard::onCeldaLimpiada(int row, int col)
{
    if (m_ganado || m_esPista[row][col]) return;

    auto *cell = m_celdas[row][col];

    SudokuMove move;
    move.row = row;
    move.col = col;
    move.oldValue = cell->number();
    move.newValue = 0;
    move.oldNotes = cell->notes();

    cell->setNumber(0);
    cell->setConflict(false);

    m_undoStack.push(move);
    m_redoStack.clear();
    if (m_undoStack.size() > MAX_UNDO) m_undoStack.remove(0);

    actualizarEstadoCelda(row, col);
    emit movimientoHecho();
}

void SudokuBoard::onTimerTick()
{
    m_tiempoSegundos++;
    emit tiempoActualizado(m_tiempoSegundos);
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

void SudokuBoard::verificarVictoria()
{
    for (int i = 0; i < TAMANIO; ++i) {
        for (int j = 0; j < TAMANIO; ++j) {
            auto *cell = m_celdas[i][j];
            if (cell->number() == 0 || tieneConflicto(i, j))
                return;
        }
    }

    m_ganado = true;
    m_timerTick->stop();
    emit juegoCompletado(m_tiempoSegundos);
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

void SudokuBoard::limpiarConflicto(int fila, int col, int num)
{
    auto *cell = m_celdas[fila][col];
    cell->setConflict(false);
    actualizarEstadoCelda(fila, col);
}
