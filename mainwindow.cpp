#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::on_pushButton_2_clicked);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug()<<"this is a slot onloginclicked";  //调试程序
    QString username = ui->lename->text().trimmed();  //读取用户名
    QString userpass = ui->lecode->text().trimmed();  //读取密码
    if(username == "abc" && userpass == "123456"){
        qDebug()<<"Login Sucessfully";
    }
    else {
        qDebug()<<"Login Failed";
    }
}
