#include "SudokuCell.h"

#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QFontMetrics>

SudokuCell::SudokuCell(int row, int col, QWidget *parent)
    : QWidget(parent), m_row(row), m_col(col)
{
    setFixedSize(48, 48);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(false);
    setCursor(Qt::ArrowCursor);
}

void SudokuCell::setNumber(int num)
{
    if (m_readOnly && m_number != 0) return;
    m_number = num;
    if (num != 0) m_notes.clear();
    update();
}

void SudokuCell::setNote(int num, bool active)
{
    if (m_number != 0) return;
    if (active)
        m_notes.insert(num);
    else
        m_notes.remove(num);
    update();
}

bool SudokuCell::hasNote(int num) const
{
    return m_notes.contains(num);
}

void SudokuCell::clearNotes()
{
    m_notes.clear();
    update();
}

void SudokuCell::setNotes(const QSet<int> &notes)
{
    m_notes = notes;
    update();
}

void SudokuCell::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
    update();
}

void SudokuCell::setConflict(bool conflict)
{
    m_conflict = conflict;
    update();
}

void SudokuCell::setSelected(bool selected)
{
    m_selected = selected;
    update();
}

void SudokuCell::setHighlighted(bool highlighted)
{
    m_highlighted = highlighted;
    update();
}

QColor SudokuCell::backgroundColor() const
{
    if (m_selected)
        return QColor("#b8d4f0");
    if (m_highlighted)
        return QColor("#ddeeff");
    if (m_readOnly) {
        int bRow = m_row / 3, bCol = m_col / 3;
        return ((bRow + bCol) % 2 == 0) ? QColor("#d6e4f0") : QColor("#c8d8e8");
    }
    int bRow = m_row / 3, bCol = m_col / 3;
    return ((bRow + bCol) % 2 == 0) ? QColor("#ffffff") : QColor("#e8e8e8");
}

void SudokuCell::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect r = rect();

    painter.setPen(Qt::NoPen);
    if (m_conflict && m_number != 0)
        painter.setBrush(QColor("#ffcccc"));
    else
        painter.setBrush(backgroundColor());
    painter.drawRect(r);

    if (m_selected) {
        painter.setPen(QPen(QColor("#0078d7"), 2.5));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(r.adjusted(1, 1, -2, -2));
    }

    if (m_number != 0) {
        QFont font = painter.font();
        font.setPointSize(18);
        font.setBold(m_readOnly);
        painter.setFont(font);

        if (m_conflict)
            painter.setPen(QColor("#cc0000"));
        else if (m_readOnly)
            painter.setPen(QColor("#333333"));
        else
            painter.setPen(QColor("#0055aa"));

        painter.drawText(r, Qt::AlignCenter, QString::number(m_number));
    } else if (!m_notes.isEmpty()) {
        drawNotes(painter, r);
    }
}

void SudokuCell::drawNotes(QPainter &painter, const QRect &rect)
{
    QFont font = painter.font();
    font.setPointSize(8);
    font.setBold(false);
    painter.setFont(font);
    painter.setPen(QColor("#666666"));

    int stepX = rect.width() / 3;
    int stepY = rect.height() / 3;

    for (int num = 1; num <= 9; ++num) {
        if (!m_notes.contains(num)) continue;
        int idx = num - 1;
        int x = rect.left() + (idx % 3) * stepX + stepX / 2 - 4;
        int y = rect.top() + (idx / 3) * stepY + stepY / 2 + 3;
        painter.drawText(x, y, QString::number(num));
    }
}

void SudokuCell::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (hasFocus())
            emit cellClicked(m_row, m_col);
        else
            setFocus(Qt::MouseFocusReason); // focusInEvent emite cellClicked
    }
    QWidget::mousePressEvent(event);
}

void SudokuCell::keyPressEvent(QKeyEvent *event)
{
    const bool esFlecha = event->key() == Qt::Key_Up || event->key() == Qt::Key_Down
                       || event->key() == Qt::Key_Left || event->key() == Qt::Key_Right;
    if (m_readOnly && !esFlecha) {
        QWidget::keyPressEvent(event);
        return;
    }

    if (!m_readOnly && event->key() >= Qt::Key_1 && event->key() <= Qt::Key_9) {
        int num = event->key() - Qt::Key_0;
        emit numberEntered(m_row, m_col, num);
        return;
    }

    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace
        || event->key() == Qt::Key_0) {
        // El tablero borra la celda y registra el movimiento para deshacer
        if (m_number != 0 || !m_notes.isEmpty())
            emit cellCleared(m_row, m_col);
        return;
    }

    int newRow = m_row, newCol = m_col;
    switch (event->key()) {
    case Qt::Key_Up:    newRow = qMax(0, m_row - 1); break;
    case Qt::Key_Down:  newRow = qMin(8, m_row + 1); break;
    case Qt::Key_Left:  newCol = qMax(0, m_col - 1); break;
    case Qt::Key_Right: newCol = qMin(8, m_col + 1); break;
    default: QWidget::keyPressEvent(event); return;
    }

    auto cells = parentWidget()->findChildren<SudokuCell *>(QString(), Qt::FindDirectChildrenOnly);
    for (auto *cell : cells) {
        if (cell->row() == newRow && cell->col() == newCol) {
            cell->setFocus();
            break;
        }
    }
}

void SudokuCell::focusInEvent(QFocusEvent *event)
{
    setSelected(true);
    emit cellClicked(m_row, m_col);
    QWidget::focusInEvent(event);
}

void SudokuCell::focusOutEvent(QFocusEvent *event)
{
    setSelected(false);
    QWidget::focusOutEvent(event);
}
