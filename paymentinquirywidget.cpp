#include "paymentinquirywidget.h"
#include "ui_paymentinquirywidget.h"
#include "databasemanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QRadioButton>
#include <QCoreApplication>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QInputDialog>
#include<QTableWidgetItem>
#include <QSqlRecord>
#include <QDateTime>
#include <QApplication>
#include <QInputDialog>
#include <QDateTime>

PaymentInquiryWidget::PaymentInquiryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PaymentInquiryWidget)
{
    ui->setupUi(this);
    ui->label->setStyleSheet ("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");

    ui->tabWidget->setStyleSheet(
            "QTabBar::tab {"
            "    min-width: 120px;"    // 标签的最小宽度
            "    min-height: 50px;"    // 标签的最小高度
            "}"
        );

    table=ui->table;
    table2=ui->table2;
    ui->tabWidget->setCurrentIndex(1);
    table->setColumnCount(11);
    table2->setColumnCount(11);
    table->setHorizontalHeaderLabels(
    QStringList() <<"订单编号"<<"缴费周期"<<"缴费类型"<< "应付" << "实付" << "缴费时间"
                <<"缴费人用户名"<<"缴费人姓名"<<"家庭地址"<<"联系电话"<<"缴费方式");
    table2->setHorizontalHeaderLabels(
    QStringList() <<"订单编号"<<"缴费周期"<<"缴费类型"<< "应付" << "实付" << "缴费时间"
                <<"缴费人用户名"<<"缴费人姓名"<<"家庭地址"<<"联系电话"<<"缴费方式");
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QHeaderView::section { font-weight: bold; } "
                         "QTableWidget::item:selected { background: #2d8cf0; }");
    setFixedRowHeight(table, 60);
    table2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table2->setSelectionBehavior(QAbstractItemView::SelectRows);
    table2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table2->setAlternatingRowColors(true);
    table2->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QHeaderView::section { font-weight: bold; } "
                         "QTableWidget::item:selected { background: #2d8cf0; }");
    setFixedRowHeight(table2, 60);

    connect(ui->pushButton,&QPushButton::clicked,this,&PaymentInquiryWidget::searchone);
    connect(ui->pushButton_1,&QPushButton::clicked,this,&PaymentInquiryWidget::setup);
    connect(ui->pushButton_2,&QPushButton::clicked,this,&PaymentInquiryWidget::refreshone);
    connect(ui->pushButton_3,&QPushButton::clicked,this,&PaymentInquiryWidget::searchtwo);
    connect(ui->pushButton_4,&QPushButton::clicked,this,&PaymentInquiryWidget::refreshtwo);

    refreshTable();

}

// 固定行高
void PaymentInquiryWidget::setFixedRowHeight(QTableWidget *table, int height)
{
    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHeight(i, height);
}

void PaymentInquiryWidget::refreshTable(){
    DatabaseManager db;
    if (!db.openDB("try.db")) {
            qCritical() << "数据库打开失败";
            return;
        }
    QSqlQuery query;

    // 查询已支付的记录
            QString sqlPaid = "SELECT payment.paymentid, payment.paymentperiod, payment.type, payment.payable, payment.paid, "
                              "payment.paymenttime, payment.username, payment.name, payment.location, payment.phonenumber, "
                              "payment.method FROM payment WHERE phonenumber IS NOT NULL AND phonenumber != ''";
            query = db.query(sqlPaid, QVariantList());
            table2->clearContents();
            table2->setRowCount(0);
            int row = 0;
            while (query.next()) {
                table2->insertRow(row);
                for (int col = 0; col < query.record().count(); col++) {
                    QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                    table2->setItem(row, col, item);
                }
                QString val2 = query.value(2).toString();
                if(val2=="0"){
                    val2="维修费";
                }
                else if(val2=="1"){
                    val2="物业费";
                }
                else if(val2=="2"){
                    val2="车位租赁";
                }
                else{
                    val2="车位购买";
                }
                QTableWidgetItem *item2 = new QTableWidgetItem(val2);
                            table2->setItem(row, 2, item2);
                QString val = query.value(10).toString();
                if (val == "1") {
                    val = "线上缴费";
                } else if (val == "0") {
                    val = "线下缴费";
                }
                QTableWidgetItem *item10 = new QTableWidgetItem(val);
                table2->setItem(row, 10, item10);


                row++;
            }

        // 设置单元格居中
                for (int r = 0; r < table2->rowCount(); ++r) {
                    for (int c = 0; c < table2->columnCount(); ++c) {
                        if (QTableWidgetItem* item = table2->item(r, c)) {
                            item->setTextAlignment(Qt::AlignCenter);
                        }
                    }
                }
                setFixedRowHeight(table2, 60);

                // 查询未支付的记录
                       QString sqlUnpaid = "SELECT payment.paymentid, payment.paymentperiod, payment.type, payment.payable, payment.paid, "
                                           "payment.paymenttime, payment.username, payment.name, payment.location, payment.phonenumber, "
                                           "payment.method FROM payment WHERE phonenumber IS NULL OR phonenumber = ''";
                       query = db.query(sqlUnpaid, QVariantList());
                       table->clearContents();
                       table->setRowCount(0);
                       row = 0;
                       while (query.next()) {
                           table->insertRow(row);
                           for (int col = 0; col < query.record().count(); col++) {
                               QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                               table->setItem(row, col, item);
                           }
                           QString val2 = query.value(2).toString();
                                       if(val2=="0"){
                                           val2="维修费";
                                       }
                                       else if(val2=="1"){
                                           val2="物业费";
                                       }
                                       else if(val2=="2"){
                                           val2="车位租赁";
                                       }
                                       else{
                                           val2="车位购买";
                                       }
                                       QTableWidgetItem *item2 = new QTableWidgetItem(val2);
                                                   table->setItem(row, 2, item2);
                                       QString val = query.value(10).toString();
                                       if (val == "1") {
                                           val = "线上缴费";
                                       } else if (val == "0") {
                                           val = "线下缴费";
                                       }
                                       QTableWidgetItem *item10 = new QTableWidgetItem(val);
                                       table->setItem(row, 10, item10);
                           row++;
                       }

        // 设置单元格居中
                for (int r = 0; r < table->rowCount(); ++r) {
                    for (int c = 0; c < table->columnCount(); ++c) {
                        if (QTableWidgetItem* item = table->item(r, c)) {
                            item->setTextAlignment(Qt::AlignCenter);
                        }
                    }
                }
                setFixedRowHeight(table, 60);
   }

void PaymentInquiryWidget::searchone(){
    bool ok;
    QString username = QInputDialog::getText(this, "用户查找", "您要查找的用户的用户名为：", QLineEdit::Normal, "", &ok);
    if (!ok  ) return;

    // 打开数据库连接
    DatabaseManager db;
    db.openDB("try.db");

    QSqlQuery query;
    query.prepare("SELECT paymentid, paymentperiod, type, payable, paid, paymenttime, username, name, location, phonenumber, method FROM payment WHERE username = ?");
    query.addBindValue(username);
    if (!query.exec()) {
        qCritical() << "查询失败：" << query.lastError().text();
        QMessageBox::critical(this,"失败", "查询错误");
        return;
    }

    // 调试信息：输出查询结果的行数
    int rowCount = 0;
    while(query.next()) {
        rowCount++;
    }
//    qDebug() << "查询结果的行数: " << rowCount;

    // 将查询指针重置到开头
    query.seek(-1);

    table->clearContents();
    table->setRowCount(0);
    int row = 0;
    while(query.next()){
        table->insertRow(row);
        for (int col = 0; col < query.record().count(); col++) {
            QTableWidgetItem* item = new QTableWidgetItem(
                query.value(col).toString()
            );
            table->setItem(row, col, item);
        }
        row++;
    }
    // 设置单元格居中
            for (int r = 0; r < table->rowCount(); ++r) {
                for (int c = 0; c < table->columnCount(); ++c) {
                    if (QTableWidgetItem* item = table->item(r, c)) {
                        item->setTextAlignment(Qt::AlignCenter);
                    }
                }
            }
            setFixedRowHeight(table, 60);
}
void PaymentInquiryWidget::setup(){
    bool ok;
    QStringList types = {"维修费","车位购买"};
        QString paymentType = QInputDialog::getItem(this, "新增缴费消息", "缴费类型：", types, 0, false, &ok);
        if (!ok) return;
           QString time = QInputDialog::getText(this, "新增缴费消息", "缴费周期：", QLineEdit::Normal, "", &ok);
           if (!ok  ) return;
           QString money = QInputDialog::getText(this, "新增缴费消息", "应付：", QLineEdit::Normal, "", &ok);
           if (!ok) return;
           QString username = QInputDialog::getText(this, "新增缴费消息", "缴费人用户名：", QLineEdit::Normal, "", &ok);
           if (!ok) return;
           QString name = QInputDialog::getText(this, "新增缴费消息", "缴费人姓名：", QLineEdit::Normal, "", &ok);
           if (!ok) return;
           QString location = QInputDialog::getText(this, "新增缴费消息", "缴费人地址：", QLineEdit::Normal, "", &ok);
           if (!ok) return;
           QString paymenttime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
           QString phonenumber = QInputDialog::getText(this, "新增缴费消息", "缴费人联系电话：", QLineEdit::Normal, "", &ok);
           if (!ok) return;

           DatabaseManager db;
           if (!db.openDB("try.db")) {
            QMessageBox::critical(this, "数据库错误", "无法打开数据库文件");
                             return;
           }
          if(paymentType=="维修费"){
           QSqlQuery q;
           q.prepare("INSERT INTO payment(paymentperiod, type, payable, username,name, location, paymenttime, phonenumber, method, paid) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?,?)");
            q.addBindValue(time);
           q.addBindValue(0);
                      q.addBindValue(money);
                      q.addBindValue(username);
                      q.addBindValue(name);
                      q.addBindValue(location);
                      q.addBindValue(paymenttime);
                      q.addBindValue(phonenumber);
                      q.addBindValue(0);
                      q.addBindValue(money);
                                            if (!q.exec()) {
                                                qDebug() << "SQL 执行失败：" << q.lastError().text(); // 打印详细错误
                                                QMessageBox::warning(this, "失败", "添加账号失败，用户名可能已存在！");
                                                return;
                                            }
                                            else{
                                               refreshTable();

                                            }}
                                else{
                                    QSqlQuery q;
                                               q.prepare("INSERT INTO payment(paymentperiod, type, payable, username,name, location, paymenttime, phonenumber, method, paid) "
                                                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?,?)");
                                                          q.addBindValue(time);
                                                          q.addBindValue(3);
                                                          q.addBindValue(money);
                                                          q.addBindValue(username);
                                                          q.addBindValue(name);
                                                          q.addBindValue(location);
                                                          q.addBindValue(paymenttime);
                                                          q.addBindValue(phonenumber);
                                                          q.addBindValue(0);
                                                          q.addBindValue(money);
                                                          if (!q.exec()) {
                                                              qDebug() << "SQL 执行失败：" << q.lastError().text(); // 打印详细错误
                                                              QMessageBox::warning(this, "失败", "添加账号失败，用户名可能已存在！");
                                                              return;
                                                          }
                                                          else{
                                                             refreshTable();

                                                          }
                                }
                      }


void PaymentInquiryWidget::refreshone(){
refreshTable();
}
void PaymentInquiryWidget::searchtwo(){
    QString searchText = ui->lineEdit_2->text();
    QString condition;
    if (ui->radioButton_4->isChecked ()){
    condition = "paymentperiod";
    } else if (ui->radioButton_5->isChecked ()){
    condition = "paymenttime";
    } else if (ui->radioButton_6->isChecked ()){
    condition = "name";
    } else {
    QMessageBox::warning (this,"提示","请选择查询条件");
    return;
    }
    QSqlQuery query;
    query.prepare(QString("SELECT paymentid, paymentperiod, type, payable, paid, paymenttime, username, name, location, phonenumber, method FROM payment WHERE %1 LIKE ?").arg(condition));
    query.addBindValue("%" + searchText + "%");
    if (!query.exec ()){
    qCritical () << "查询失败" << query.lastError ().text ();
    QMessageBox::critical (this,"错误","查询失败");
    return;
    }
    table2->clearContents();
    table2->setRowCount(0);
    int row = 0;
    while(query.next()){
    table2->insertRow(row);
    for(int col = 0; col < 10; col++){
    QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
    table2->setItem(row, col, item);
    }
    QString val = query.value (10).toString ();
    val = (val == "1") ? "线上缴费" : "线下缴费";
    QTableWidgetItem *item10 = new QTableWidgetItem (val);
    table2->setItem (row, 10, item10);
    row++;
    }
    // 设置单元格居中
            for (int r = 0; r < table2->rowCount(); ++r) {
                for (int c = 0; c < table2->columnCount(); ++c) {
                    if (QTableWidgetItem* item = table2->item(r, c)) {
                        item->setTextAlignment(Qt::AlignCenter);
                    }
                }
            }
            setFixedRowHeight(table2, 60);
}
void PaymentInquiryWidget::refreshtwo(){
refreshTable();
}
PaymentInquiryWidget::~PaymentInquiryWidget()
{
    delete ui;
}
