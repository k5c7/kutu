#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "computer.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    m_computer = std::make_unique<Computer>(ui->editor_console);
    QObject::connect(ui->button_run, SIGNAL(clicked()), this, SLOT(run()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::run()
{
    spdlog::debug("Run clicked");
    m_computer->reset();
    QStringList code_qstr_list = ui->editor_code->toPlainText().split(QRegExp("\n|\r\n|\r"));
    std::vector<std::string> code_str_vec(code_qstr_list.size());

    for(size_t idx = 0; idx < code_str_vec.size(); idx++)
    {
        code_str_vec[idx] = code_qstr_list[idx].toStdString();
    }

    m_computer->get_code(code_str_vec);
    m_computer->start();
}

