#pragma once

#include <QMainWindow>

#include "GridView.hpp"

class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onCellActivated(int row, int col);

private:
    GridView* grid_ = nullptr;
};