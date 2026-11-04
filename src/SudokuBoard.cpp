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
            // Quitar primero el modo solo-lectura: setNumber() ignora las
            // celdas bloqueadas y dejaria las pistas de la partida anterior.
            cell->setReadOnly(false);
            cell->clearNotes();
            cell->setNumber(m_esPista[i][j] ? m_solucion[i][j] : 0);
            cell->setConflict(false);
            cell->setReadOnly(m_esPista[i][j]);
        }
    }

    limpiarResaltado();
    m_timer.start();
    m_timerTick->start(1000);

    emit tiempoActualizado(0);
    emit movimientoHecho();
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

void SudokuBoard::aplicarEstado(int fila, int col, int valor, const QSet<int> &notas)
{
    auto *cell = m_celdas[fila][col];
    cell->setNumber(valor);
    if (valor == 0)
        cell->setNotes(notas);
    else
        cell->clearNotes();
}

void SudokuBoard::recalcularConflictos()
{
    for (int i = 0; i < TAMANIO; ++i)
        for (int j = 0; j < TAMANIO; ++j)
            actualizarEstadoCelda(i, j);
}

void SudokuBoard::registrarMovimiento(const SudokuMove &move)
{
    m_undoStack.push(move);
    m_redoStack.clear();
    if (m_undoStack.size() > MAX_UNDO)
        m_undoStack.remove(0);
}

bool SudokuBoard::undo()
{
    if (m_undoStack.isEmpty() || m_ganado) return false;

    SudokuMove move = m_undoStack.pop();
    m_redoStack.push(move);

    aplicarEstado(move.row, move.col, move.oldValue, move.oldNotes);
    recalcularConflictos();
    emit movimientoHecho();
    return true;
}

bool SudokuBoard::redo()
{
    if (m_redoStack.isEmpty() || m_ganado) return false;

    SudokuMove move = m_redoStack.pop();
    m_undoStack.push(move);

    aplicarEstado(move.row, move.col, move.newValue, move.newNotes);
    recalcularConflictos();
    emit movimientoHecho();
    verificarVictoria();
    return true;
}

int SudokuBoard::tiempoTranscurrido() const
{
    return m_tiempoSegundos;
}

void SudokuBoard::onCeldaClicked(int row, int col)
{
    m_filaSel = row;
    m_colSel = col;
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

void SudokuBoard::ingresarNumeroEnSeleccion(int num)
{
    if (m_filaSel < 0 || m_colSel < 0) return;
    onNumeroIngresado(m_filaSel, m_colSel, num);
    m_celdas[m_filaSel][m_colSel]->setFocus();
}

void SudokuBoard::borrarSeleccion()
{
    if (m_filaSel < 0 || m_colSel < 0) return;
    onCeldaLimpiada(m_filaSel, m_colSel);
    m_celdas[m_filaSel][m_colSel]->setFocus();
}

void SudokuBoard::onNumeroIngresado(int row, int col, int num)
{
    if (m_ganado || m_esPista[row][col]) return;
    if (num < 1 || num > 9) return;

    auto *cell = m_celdas[row][col];

    SudokuMove move;
    move.row = row;
    move.col = col;
    move.oldValue = cell->number();
    move.oldNotes = cell->notes();

    if (m_notasMode) {
        if (cell->number() != 0) return;
        cell->setNote(num, !cell->hasNote(num));
        move.newValue = 0;
        move.newNotes = cell->notes();
        registrarMovimiento(move);
        emit movimientoHecho();
        return;
    }

    // Repetir el mismo numero lo borra (y queda registrado en el historial)
    move.newValue = (cell->number() == num) ? 0 : num;
    aplicarEstado(row, col, move.newValue, move.newValue == 0 ? move.oldNotes : QSet<int>());
    move.newNotes = cell->notes();
    registrarMovimiento(move);

    recalcularConflictos();
    bool conflicto = move.newValue != 0 && tieneConflicto(row, col);
    if (conflicto)
        emit conflictoDetectado(row, col, num);

    onCeldaClicked(row, col);
    emit movimientoHecho();

    if (!conflicto && move.newValue != 0)
        verificarVictoria();
}

void SudokuBoard::onCeldaLimpiada(int row, int col)
{
    if (m_ganado || m_esPista[row][col]) return;

    auto *cell = m_celdas[row][col];
    if (cell->number() == 0 && cell->notes().isEmpty()) return;

    SudokuMove move;
    move.row = row;
    move.col = col;
    move.oldValue = cell->number();
    move.oldNotes = cell->notes();
    move.newValue = 0;

    aplicarEstado(row, col, 0, QSet<int>());
    cell->setConflict(false);
    registrarMovimiento(move);

    recalcularConflictos();
    onCeldaClicked(row, col);
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

// Cuenta soluciones hasta 'limite' eligiendo siempre la celda con menos
// candidatos (MRV), lo que mantiene la busqueda rapida en tableros 9x9.
int SudokuBoard::contarSoluciones(QVector<QVector<int>> &tablero, int limite) const
{
    int mejorFila = -1, mejorCol = -1, mejorCount = 10;
    for (int i = 0; i < TAMANIO && mejorCount > 1; ++i) {
        for (int j = 0; j < TAMANIO; ++j) {
            if (tablero[i][j] != 0) continue;
            int count = 0;
            for (int n = 1; n <= 9; ++n)
                if (esValidoEnTablero(tablero, i, j, n)) ++count;
            if (count == 0) return 0;
            if (count < mejorCount) {
                mejorCount = count;
                mejorFila = i;
                mejorCol = j;
                if (count == 1) break;
            }
        }
    }

    if (mejorFila < 0) return 1; // tablero completo

    int total = 0;
    for (int n = 1; n <= 9 && total < limite; ++n) {
        if (!esValidoEnTablero(tablero, mejorFila, mejorCol, n)) continue;
        tablero[mejorFila][mejorCol] = n;
        total += contarSoluciones(tablero, limite - total);
        tablero[mejorFila][mejorCol] = 0;
    }
    return total;
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

// Quita pistas en orden aleatorio solo si el puzzle sigue teniendo una
// unica solucion. Si no se puede bajar hasta 'celdasVisibles' se queda
// con algunas pistas de mas, pero siempre con solucion unica.
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

    QVector<QVector<int>> puzzle = m_solucion;
    int visibles = TAMANIO * TAMANIO;

    for (const auto &pos : todas) {
        if (visibles <= celdasVisibles) break;
        const int f = pos.first, c = pos.second;
        const int valor = puzzle[f][c];
        puzzle[f][c] = 0;

        QVector<QVector<int>> copia = puzzle;
        if (contarSoluciones(copia, 2) == 1)
            --visibles;
        else
            puzzle[f][c] = valor;
    }

    for (int i = 0; i < TAMANIO; ++i)
        for (int j = 0; j < TAMANIO; ++j)
            m_esPista[i][j] = puzzle[i][j] != 0;
}

void SudokuBoard::verificarVictoria()
{
    if (m_ganado) return;

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

    if (cell->isReadOnly() || cell->number() == 0)
        cell->setConflict(false);
    else
        cell->setConflict(tieneConflicto(fila, col));
}

void SudokuBoard::limpiarResaltado()
{
    for (int i = 0; i < TAMANIO; ++i)
        for (int j = 0; j < TAMANIO; ++j)
            m_celdas[i][j]->setHighlighted(false);
}

void SudokuBoard::limpiarConflicto(int fila, int col, int num)
{
    Q_UNUSED(num);
    auto *cell = m_celdas[fila][col];
    cell->setConflict(false);
    actualizarEstadoCelda(fila, col);
}
