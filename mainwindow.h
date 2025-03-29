#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "computer.h"
#include "highlighter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void run();

private:
    Ui::MainWindow* ui;
    std::unique_ptr<Computer> m_computer;
    std::unique_ptr<Highlighter> m_highlighter;
};
#endif // MAINWINDOW_H
