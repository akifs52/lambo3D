#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , lambo3D(new LamboWidget)
{
    ui->setupUi(this);

    ui->verticalLayout_2->addWidget(lambo3D);

}

MainWindow::~MainWindow()
{
    delete ui;
}
