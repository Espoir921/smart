#include "maintenance.h"
#include "ui_maintenance.h"
#include "databasemanager.h"
#include <QtDebug>
#include <QStyle>
#include <QPalette>
#include <QFontDatabase>
#include <QBrush>
#include <QColor>

Maintenance::Maintenance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Maintenance)
{
    ui->setupUi(this);
    // 1. 基础样式设置
    setStyleBase();

    // 2. 标题区域美化
    styleTitleArea();

    // 3. 表单区域美化
    styleFormArea();

    // 4. 按钮区域美化
    styleButtonArea();

    // 5. 布局微调
    adjustLayout();

    initDatabase();

    // 获取当前日期
    QDateTime currentDate = QDateTime::currentDateTime();
    // 设置当前日期到 reportTimeEdit
    ui->reportTimeEdit->setDateTime(currentDate);
    connect(ui->submitRepairBtn, &QPushButton::clicked, this, &Maintenance::onSubmitRepairClicked);
}

// 基础样式设置
void Maintenance::setStyleBase() {
    // 设置窗口背景
    setStyleSheet("QWidget#Maintenance {"
                  "background-color: #f5f7fa;"
                  "}");

    // 全局字体设置
    QFont globalFont("Microsoft YaHei", 10);
    this->setFont(globalFont);

    // 设置所有标签样式
    QString labelStyle = "QLabel {"
                         "color: #333333;"
                         "font-weight: 500;"
                         "}";
    ui->label->setStyleSheet(labelStyle);
    ui->label_2->setStyleSheet(labelStyle);
    ui->label_4->setStyleSheet(labelStyle);
    ui->label_5->setStyleSheet(labelStyle);
    ui->label_6->setStyleSheet(labelStyle);
    ui->label_7->setStyleSheet(labelStyle);
}

// 标题区域美化
void Maintenance::styleTitleArea() {
    // 主标题样式
    ui->label->setStyleSheet("QLabel {"
                            "color: #2c3e50;"
                            "font-family: 'Microsoft YaHei';"
                            "font-size: 24px;"
                            "font-weight: bold;"
                            "padding: 10px 0;"
                            "}");

    // 返回按钮样式
    ui->back->setStyleSheet("QPushButton {"
                           "background-color: #3498db;"
                           "color: white;"
                           "border-radius: 4px;"
                           "padding: 6px 12px;"
                           "font-family: 'Microsoft YaHei';"
                           "font-size: 11px;"
                           "}"
                           "QPushButton:hover {"
                           "background-color: #2980b9;"
                           "}"
                           "QPushButton:pressed {"
                           "background-color: #1f6dad;"
                           "}");
}

// 表单区域美化
void Maintenance::styleFormArea() {
    // 输入框通用样式
    QString lineEditStyle = "QLineEdit, QDateEdit {"
                           "border: 1px solid #bdc3c7;"
                           "border-radius: 4px;"
                           "padding: 6px 8px;"
                           "background-color: white;"
                           "font-family: 'Microsoft YaHei';"
                           "font-size: 12px;"
                           "}"
                           "QLineEdit:focus, QDateEdit:focus {"
                           "border-color: #3498db;"
                           "outline: none;"
                           "}";

    // 应用到所有输入框
    ui->usernameEdit->setStyleSheet(lineEditStyle);
    ui->locationEdit->setStyleSheet(lineEditStyle);
    ui->phonenumberEdit->setStyleSheet(lineEditStyle);
    ui->reportTimeEdit->setStyleSheet(lineEditStyle);

    // 多行文本框样式
    ui->descriptionEdit->setStyleSheet("QTextEdit {"
                                      "border: 1px solid #bdc3c7;"
                                      "border-radius: 4px;"
                                      "padding: 6px 8px;"
                                      "background-color: white;"
                                      "font-family: 'Microsoft YaHei';"
                                      "font-size: 12px;"
                                      "}"
                                      "QTextEdit:focus {"
                                      "border-color: #3498db;"
                                      "outline: none;"
                                      "}");

    // 调整日期编辑框样式
    ui->reportTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
//    ui->reportTimeEdit->setCalendarPopup(true);
}

// 按钮区域美化
void Maintenance::styleButtonArea() {
    // 提交按钮样式
    ui->submitRepairBtn->setStyleSheet("QPushButton {"
                                      "background-color: #2ecc71;"
                                      "color: white;"
                                      "border-radius: 4px;"
                                      "padding: 8px 24px;"
                                      "font-family: 'Microsoft YaHei';"
                                      "font-size: 14px;"
                                      "font-weight: 500;"
                                      "}"
                                      "QPushButton:hover {"
                                      "background-color: #27ae60;"
                                      "}"
                                      "QPushButton:pressed {"
                                      "background-color: #219653;"
                                      "}"
                                      "QPushButton:disabled {"
                                      "background-color: #bdc3c7;"
                                      "}");
}

// 布局微调
void Maintenance::adjustLayout() {
    // 调整整体布局间距
    ui->gridLayout->setContentsMargins(20, 20, 20, 20);
    ui->gridLayout->setSpacing(15);

    // 调整输入框大小
    ui->usernameEdit->setMinimumHeight(32);
    ui->locationEdit->setMinimumHeight(32);
    ui->phonenumberEdit->setMinimumHeight(32);
    ui->reportTimeEdit->setMinimumHeight(32);
    ui->descriptionEdit->setMinimumHeight(120);
    ui->descriptionEdit->setMinimumWidth(250);

    // 调整按钮大小
    ui->back->setMinimumHeight(30);
    ui->submitRepairBtn->setMinimumHeight(36);

    // 设置窗口最小大小
    setMinimumSize(1200, 600);
}


void Maintenance::initDatabase()
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

bool Maintenance::validateInput()
{
    // 必填
    if (ui->usernameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "报修人姓名不能为空！");
        return false;
    }

    if (ui->locationEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "业主地址不能为空！");
        return false;
    }

    if (ui->phonenumberEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "电话号码不能为空！");
        return false;
    }

    if (ui->descriptionEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "报修描述不能为空！");
        return false;
    }


    return true;
}

void Maintenance::onSubmitRepairClicked()
{
    if (!validateInput()) {
        return;
    }


    QString username = ui->usernameEdit->text().trimmed();
    QString location = ui->locationEdit->text().trimmed();
    QString phonenumber = ui->phonenumberEdit->text().trimmed();
//    QDate reportTime = ui->reportTimeEdit->date();
    QDateTime currentDate = QDateTime::currentDateTime();

    QString description = ui->descriptionEdit->toPlainText().trimmed();


    DatabaseManager db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query;
    query.prepare("INSERT INTO maintenance ("
                  "username, "
                  "phonenumber, "
                  "location, "
                  "reporttime, "
                  "description, "
                  "status, "
                  "maintenancedate, "
                  "handler"
                  ") VALUES ("
                  ":username, "
                  ":phonenumber, "
                  ":location, "
                  ":reporttime, "
                  ":description, "
                  ":status, "
                  ":maintenancedate, "
                  ":handler"
                  ")");


    query.bindValue(":username", username);
    query.bindValue(":phonenumber", phonenumber);
    query.bindValue(":location", location);
    query.bindValue(":reporttime", currentDate.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":description", description);
    query.bindValue(":status", 0);
    query.bindValue(":maintenancedate", QDate());
    query.bindValue(":handler", "");

    if (query.exec()) {
        QMessageBox::information(this, "提交成功", "报修信息已提交，我们会尽快处理！");
        ui->usernameEdit->clear();
        ui->locationEdit->clear();
        ui->phonenumberEdit->clear();
        ui->reportTimeEdit->setDate(QDate::currentDate());
        ui->descriptionEdit->clear();
    } else {
        qCritical() << "SQL 执行失败：" << query.lastError().text();
        QMessageBox::critical(this, "提交失败", "数据库错误：" + query.lastError().text());
    }
}

Maintenance::~Maintenance()
{
    delete ui;
}



void Maintenance::on_back_clicked()
{
    emit maintenancegoBack();
}


