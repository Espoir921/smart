#include "login.h"
#include "ui_login.h"
#include <QDebug>
#include "databasemanager.h"
#include <QSqlQuery>
#include <QMessageBox>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login),db(DatabaseManager::getInstance())
{
    ui->setupUi(this);
    usernameEdit = ui->lename;
    passwordEdit = ui->lecode;
    loginBtn = ui->pushButton_2;
    registerBtn = ui->pushButton;

    //打开数据库
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this,"错误","数据库打开失败！");
    }
    connect(loginBtn, &QPushButton::clicked, this, &Login::onLoginClicked);
    connect(registerBtn, &QPushButton::clicked, this, &Login::onExitClicked);
}

Login::~Login()
{
    delete ui;
}

//读取用户名和密码
void Login::onLoginClicked(){
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this,"提示","请输入用户和密码！");
        return;
    }

    //查询数据库，使用QSqlQuery数据集
    QSqlQuery query = db.getUserByUserName(username);
    if (query.next()){
        QString dbPassword = query.value("password").toString();
        if (dbPassword != password){
            QMessageBox::warning(this, "登录失败","密码错误！");
            return;
        }
        int userID = query.value("id").toInt();
        int role = query.value("role").toInt();
        emit loginSuccess(userID,role);
        accept();
    }
    else {
        QMessageBox::warning(this, "登录失败","用户不存在，请先注册！");
        return;
    }
}

void Login::onExitClicked(){
    reject();
}
