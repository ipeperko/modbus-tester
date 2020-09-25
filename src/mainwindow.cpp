#include "mainwindow.h"
#include "client_tab.h"
#include "server_tab.h"
#include "ui_mainwindow.h"
#include <QDebug>

mainwindow::mainwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Modbus tester");

    ui->tabWidget->addTab(new client_tab, "Client");
    ui->tabWidget->addTab(new server_tab, "Server");
}

mainwindow::~mainwindow()
{
    delete ui;
}
