#include "../include/GridView.hpp"
#include <QPainter>
#include <QScrollBar>
#include <QPinchGesture>
#include <QTouchEvent>
#include <QLineF>

GridView::GridView(QWidget* parent) : QGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setViewportUpdateMode(FullViewportUpdate);
    grabGesture(Qt::PinchGesture);
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    QAbstractScrollArea::setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    QAbstractScrollArea::setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff ) ;
}

void GridView::setCellSize(int size) {
    cell_size_  = size;
    viewport()->update();
}

int GridView::cellSize() const {
    return cell_size_;
}

void GridView::wheelEvent(QWheelEvent* event) {
    constexpr qreal step = 1.05;
    constexpr qreal minScale = 0.25;
    constexpr qreal maxScale = 5.0;

    qreal factor = (event->angleDelta().y() > 0) ? step : 1.0 / step;
    qreal newScale = qBound(minScale, scale_ * factor, maxScale);
    qreal actualScale = newScale / scale_;
    scale(actualScale, actualScale);
    scale_ = newScale;
}

/* bool GridView::event(QEvent* event) {
    if (event->type() == QEvent::Gesture) {
        QGestureEvent* gest = static_cast<QGestureEvent*>(event);
        if (auto* pinch = static_cast<QPinchGesture*>(gest->gesture(Qt::PinchGesture))) {
            constexpr qreal minScale = 0.25;
            constexpr qreal maxScale = 5.0;
            qreal pinchFactor = pinch->scaleFactor();

            qreal newScale = scale_ * pinchFactor;
            newScale = qBound(minScale, newScale, maxScale);
            qreal actualFactor = newScale / scale_;
            scale(actualFactor, actualFactor);
            scale_ = newScale;
            gest->accept(pinch);
            return true;
        }
    }
    return QGraphicsView::event(event);
} */

bool GridView::event(QEvent* event) {
    return QGraphicsView::event(event);
}

void GridView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        is_panning_ = true;
        last_mouse_pos_ = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void GridView::mouseMoveEvent(QMouseEvent* event) {
    if (is_panning_) {
        QPoint delta = event->pos() - last_mouse_pos_;
        last_mouse_pos_ = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    }
    QGraphicsView::mouseMoveEvent(event);
}

void GridView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton && is_panning_) {
        is_panning_ = false;
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void GridView::drawBackground(QPainter* painter, const QRectF& rect) {
    painter->save();
    painter->setPen(QPen(Qt::lightGray, 0));

    QRectF viewRect = mapToScene(viewport()->geometry()).boundingRect();

    qreal left = std::floor(viewRect.left() / cell_size_) * cell_size_;
    qreal right = std::ceil(viewRect.right() / cell_size_) * cell_size_;
    qreal top = std::floor(viewRect.top() / cell_size_) * cell_size_;
    qreal bottom = std::ceil(viewRect.bottom() / cell_size_) * cell_size_;

    for (qreal x = left; x <= right; x += cell_size_) {
        painter->drawLine(QPointF(x, top), QPointF(x, bottom));
    }
    for (qreal y = top; y <= bottom; y += cell_size_) {
        painter->drawLine(QPointF(left, y), QPointF(right, y));
    }

    painter->restore();
}

void GridView::mouseDoubleClickEvent(QMouseEvent* event) {
    QPointF scenePt = mapToScene(event->pos());
    int col = static_cast<int>(scenePt.x()) / cell_size_;
    int row = static_cast<int>(scenePt.y()) / cell_size_;
    emit cellDoubleClicked(row, col);
    QGraphicsView::mouseDoubleClickEvent(event);
}

/* bool GridView::viewportEvent(QEvent* event) {
    if (event->type() == QEvent::TouchBegin ||
        event->type() == QEvent::TouchUpdate ||
        event->type() == QEvent::TouchEnd) {
        
            QTouchEvent* touch = static_cast<QTouchEvent*>(event);
            const auto &points = touch->points();

            if (points.count() == 2) {
                const auto &p1 = points[0];
                const auto &p2 = points[1];

                QPointF center = (p1.position() + p2.position()) / 2.0;
                QPointF last = (p1.lastPosition() + p2.lastPosition()) / 2.0;
                QPointF delta = center - last;

                horizontalScrollBar()->setValue(horizontalScrollBar()->value() - int(delta.x()));
                verticalScrollBar()->setValue(verticalScrollBar()->value() - int(delta.y()));
                return true;
            }
    }
    return QGraphicsView::viewportEvent(event);
} */

bool GridView::viewportEvent(QEvent* event) {
    if (event->type() == QEvent::TouchUpdate) {        
            QTouchEvent* touch = static_cast<QTouchEvent*>(event);
            const auto &points = touch->points();

            if (points.count() == 2) {
                const auto &p1 = points[0];
                const auto &p2 = points[1];

                qreal curDist = QLineF(p1.position(), p2.position()).length();
                qreal lastDist = QLineF(p1.lastPosition(), p2.lastPosition()).length();
                qreal distDelta = curDist - lastDist;

                QPointF center = (p1.position() + p2.position()) / 2.0;
                QPointF lastCenter = (p1.lastPosition() + p2.lastPosition()) / 2.0;
                QPointF panDelta = center - lastCenter;

                constexpr qreal zoomThreshold = 10.0;

                if (qAbs(distDelta) > zoomThreshold) {
                    constexpr qreal minScale = 0.25;
                    constexpr qreal maxScale = 5.0;
                    qreal factor = 1.0 + distDelta / 200.0;
                    qreal newScale = qBound(minScale, scale_ * factor, maxScale);
                    qreal actualFactor = newScale / scale_;
                    scale_ = newScale;
                    scale(actualFactor, actualFactor);
                } else {
                    horizontalScrollBar()->setValue(horizontalScrollBar()->value() - int(panDelta.x()));
                    verticalScrollBar()->setValue(verticalScrollBar()->value() - int(panDelta.y()));
                }
                return true;
            }
    }
    return QGraphicsView::viewportEvent(event);
}