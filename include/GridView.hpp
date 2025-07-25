#pragma once

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGestureEvent>
#include <QTouchEvent>

class GridView: public QGraphicsView {
    Q_OBJECT

public:
    explicit GridView(QWidget* parent = nullptr);

    void setCellSize(int size);
    int cellSize() const;

signals:
    void cellDoubleClicked(int row, int col);

protected:
    bool viewportEvent(QEvent* event) override;
    bool event(QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void drawBackground(QPainter*painter, const QRectF& rect) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    bool is_panning_ = false;
    QPoint last_mouse_pos_;
    int cell_size_ = 40;
    qreal scale_ = 1.0;
};