#include "../include/MainWindow.hpp"
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent) {
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 2000, 2000);

    grid_ = new GridView(this);
    grid_->setScene(scene);
    setCentralWidget(grid_);

    connect(grid_, &GridView::cellDoubleClicked, 
            this,  &MainWindow::onCellActivated);

    grid_->setCellSize(50);

    resize(800, 600);
    show();
}

void MainWindow::onCellActivated(int row, int col) {
    int size = grid_->cellSize();
    qreal x = size * col;
    qreal y = size * row;

    qreal r = size / 3.0;

    QGraphicsEllipseItem* e = new QGraphicsEllipseItem(x + (size - r * 2) / 2, y + (size - r * 2) / 2, r * 2, r * 2);

    e->setBrush(Qt::red);
    e->setZValue(1);
    grid_->scene()->addItem(e);
}