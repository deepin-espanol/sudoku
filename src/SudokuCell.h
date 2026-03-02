#ifndef SUDOKUCELL_H
#define SUDOKUCELL_H

#include <QWidget>
#include <QSet>

class SudokuCell : public QWidget
{
    Q_OBJECT

public:
    explicit SudokuCell(int row, int col, QWidget *parent = nullptr);

    int row() const { return m_row; }
    int col() const { return m_col; }

    void setNumber(int num);
    int number() const { return m_number; }

    void setNote(int num, bool active);
    bool hasNote(int num) const;
    void clearNotes();
    QSet<int> notes() const { return m_notes; }
    void setNotes(const QSet<int> &notes);

    void setReadOnly(bool readOnly);
    bool isReadOnly() const { return m_readOnly; }

    void setConflict(bool conflict);
    bool hasConflict() const { return m_conflict; }

    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

    void setHighlighted(bool highlighted);

signals:
    void cellClicked(int row, int col);
    void numberEntered(int row, int col, int num);
    void cellCleared(int row, int col);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    QSize sizeHint() const override { return QSize(48, 48); }

private:
    void drawNotes(QPainter &painter, const QRect &rect);
    QColor backgroundColor() const;

    int m_row;
    int m_col;
    int m_number = 0;
    QSet<int> m_notes;
    bool m_readOnly = false;
    bool m_conflict = false;
    bool m_selected = false;
    bool m_highlighted = false;
};

#endif
