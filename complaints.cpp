#include "complaints.h"
#include "ui_complaints.h"
#include "databasemanager.h"
#include <QDateTime>
#include <QDebug>

Complaints::Complaints(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Complaints)
{
    ui->setupUi(this);
    initDatabase();
    ui->dateEdit->setDate(QDate::currentDate());
    connect(ui->submitButton, &QPushButton::clicked, this, &Complaints::onSubmitClicked);

}

Complaints::~Complaints()
{
    delete ui;
}

void Complaints::initDatabase()
{

    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("try.db");
    }


    if (!db.open()) {
        qCritical() << "数据库打开失败：" << db.lastError().text();
        QMessageBox::critical(this, "错误", "无法连接数据库，请检查路径或权限！");
    }
}


bool Complaints::validateInput()
{
    bool isUsernameInt = false;
    ui->usernameEdit->text().toInt(&isUsernameInt);
    if (!isUsernameInt || ui->usernameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "投诉人用户名格式错误！");
        return false;
    }

    if (ui->descriptionEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "问题描述不能为空！");
        return false;
    }

    return true;
}
void Complaints::onSubmitClicked()
{
    if (!validateInput()) {
        return;
    }

    int username = ui->usernameEdit->text().toInt();
    QString name = ui->nameEdit->text().trimmed();
    QString description = ui->descriptionEdit->toPlainText().trimmed();
    QDate complaintDate = ui->dateEdit->date();
    int complainedId = ui->complainedIdEdit->text().toInt();
    QString complainedName = ui->complainedNameEdit->text().trimmed();
    QString phoneNumber = ui->phoneEdit->text().trimmed();
    QString location = ui->locationEdit->text().trimmed();
    QString expect = ui->expectEdit->toPlainText().trimmed();

    DatabaseManager db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query;
    query.prepare("INSERT INTO complaints (""username, ""name, ""description, ""phonenumber, ""location, " "complainedid, "
                  "complainedname, "
                  "date, "
                  "expect"
                  ") VALUES ("
                  ":username, "
                  ":name, "
                  ":description, "
                  ":phonenumber, "
                  ":location, "
                  ":complainedid, "
                  ":complainedname, "
                  ":date, "
                  ":expect"
                  ")");

    query.bindValue(":username", username);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.bindValue(":phonenumber", phoneNumber);
    query.bindValue(":location", location);
    query.bindValue(":complainedid", complainedId);
    query.bindValue(":complainedname", complainedName);
    query.bindValue(":date", complaintDate.toString("yyyy-MM-dd"));
    query.bindValue(":expect", expect);


    if (query.exec()) {
        QMessageBox::information(this, "提交成功", "投诉建议已提交，我们会尽快处理！");

        ui->usernameEdit->clear();
        ui->nameEdit->clear();
        ui->descriptionEdit->clear();
        ui->dateEdit->setDate(QDate::currentDate());
        ui->complainedIdEdit->clear();
        ui->complainedNameEdit->clear();
        ui->phoneEdit->clear();
        ui->locationEdit->clear();
        ui->expectEdit->clear();
    } else {
        qCritical() << "SQL 执行失败：" << query.lastError().text();
        QMessageBox::critical(this, "提交失败", "数据库错误：" + query.lastError().text());
    }
}


void Complaints::on_back_clicked()
{
    emit complaintgoBack();
}
