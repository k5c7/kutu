#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "parser.h"
#include "computer.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Computer c;
    c.get_code({{"PRINT $0"},
        {"MOV $0 5"},
        {"MOV $1 7"},
        {"ADD $0 $0 $1"},
        {"PRINT $0"}});
    c.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

