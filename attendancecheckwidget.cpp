#include "attendancecheckwidget.h"
#include "ui_attendancecheckwidget.h"
#include "databasemanager.h"
#include <QSpacerItem>
#include <QStyle>
#include <QApplication>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QVariantList>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QSqlError>
#include <QDate>

void AttendanceCheckWidget::refreshTable(){

    table->setRowCount(0);//清空表格便于数据导入
    DatabaseManager db;
    db.openDB("try.db");
    QString sql = "SELECT id, staffid, name, department, position, date, status FROM attendance WHERE staffid = ?";
//    qDebug()<<"sql是"<<sql;
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(m_userId);
    query.exec();

//    qDebug()<<"thisis a test";
    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        for (int col = 0; col < 7; ++col) {

            QString val= query.value(col).toString();
            if (col == 6) { // 状态
                if (val == "1") val = "已打卡";
                else if (val == "0") val = "未打卡";
                else val = "未知";
            }
            table->setItem(row, col, new QTableWidgetItem(val));
        }
        ++row;
    }
    // 设置单元格居中
    for (int r = 0; r < table->rowCount(); ++r) {
        for (int c = 0; c < table->columnCount(); ++c) {
            if (QTableWidgetItem* item = table->item(r, c)) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    setFixedRowHeight(table,60);
}

AttendanceCheckWidget::AttendanceCheckWidget(QWidget *parent) :
    QWidget(parent){
    table = new QTableWidget(this);//创建表格
       table->setColumnCount(7);//表格8列
       table->setHorizontalHeaderLabels({"考勤单编号", "ID", "姓名", "部门", "岗位", "日期", "打卡状态"});
       table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
              //                   设置列的调整模式为QHeaderView::Stretch，即表格自动拉伸列宽
       table->setSelectionBehavior(QAbstractItemView::SelectRows);
                                   //选中整行
       table->setAlternatingRowColors(true);
       table->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                                "QHeaderView::section { font-weight: bold; } "
                                "QTableWidget::item:selected { background: #2d8cf0; }");


       //设置表格外观样式
       btnAdd = new QPushButton("打卡", this);//QPushButton增加按钮
       btnEdit = new QPushButton("查询", this);
       btnDelete = new QPushButton("删除", this);
       btnReturn = new QPushButton("返回",this);
       //按钮美化
       // 新增按钮（蓝色）
           QString addStyle = "QPushButton { background-color: #2d8cf0; color: white; border-radius: 6px; "
                              "font-family: 等线; font-weight: bold;}"
                              "QPushButton:hover { background-color: #1a73e8; }";
           btnAdd->setStyleSheet(addStyle);
           btnAdd->setFixedSize(60, 30);
           // 编辑按钮（绿色）
           QString editStyle = "QPushButton { background-color: #19be6b; color: white; border-radius: 6px; "
                               "font-family: 等线; font-weight: bold;}"
                               "QPushButton:hover { background-color: #13a456; }";
           btnEdit->setStyleSheet(editStyle);
           btnEdit->setFixedSize(60, 30);
           // 删除按钮（红色）
           QString deleteStyle = "QPushButton { background-color: #f56c6c; color: white; border-radius: 6px; "
                                   "font-family: 等线; font-weight: bold;}"
                                   "QPushButton:hover { background-color: #d93025; }";
           btnDelete->setStyleSheet(deleteStyle);
           btnDelete->setFixedSize(60, 30);
           btnReturn->setStyleSheet(editStyle);
           btnReturn->setFixedSize(60,30);

           auto* btnLayout = new QHBoxLayout();//auto：btnLayout是指向QHBoxLayout的指针
           btnLayout->addStretch();//表格伸缩
           btnLayout->addWidget(btnAdd);
           btnLayout->addSpacing(10);
           btnLayout->addWidget(btnEdit);//实现按钮的水平排列和间距控制
           btnLayout->addSpacing(10);
           btnLayout->addWidget(btnDelete);
           btnLayout->addSpacing(10);
           btnLayout->addWidget(btnReturn);
           btnLayout->addStretch();
           // 标题
               QLabel* title = new QLabel("汤臣二品工作人员出勤打卡", this);//QLabel显示文本或图像
               title->setAlignment(Qt::AlignCenter);//对齐方式：居中
//               QFont font = title->font();
//               title->setFont(font);
               title->setStyleSheet ("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");
                      //样式表
               auto* mainLayout = new QVBoxLayout(this);
               mainLayout->addWidget(title);
               mainLayout->addLayout(btnLayout);
               mainLayout->addWidget(table);

               connect(btnAdd, &QPushButton::clicked, this, &AttendanceCheckWidget::onAdd);
               connect(btnEdit, &QPushButton::clicked, this, &AttendanceCheckWidget::onEdit);
               connect(btnDelete, &QPushButton::clicked, this, &AttendanceCheckWidget::onDelete);
               connect(btnReturn, &QPushButton::clicked, this, &AttendanceCheckWidget::onReturn);

               refreshTable();
               setFixedRowHeight(table, 60);
               onReturn();
           }




//获取当前用户名
void AttendanceCheckWidget::setUserId(const int userId)
{
    m_userId = userId;
    refreshTable();
}

//检查是否已完成打卡
bool AttendanceCheckWidget::onCheck(QString date){
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return false;
    }
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM attendance WHERE date = ? AND staffid = ?");
    query.addBindValue(date);
    query.addBindValue(m_userId);
    if(query.exec()&&query.next()) {
        if(query.value(0).toInt() == 0){
            return true;
        }
        return false;
    }
    return false;

}


void AttendanceCheckWidget::onAdd(){
    DatabaseManager db;
    if (!db.openDB("try.db")) {
        QMessageBox::critical(this, "数据库错误", "无法打开数据库文件");
        return;
    }

     // 获取当前日期
     QDate currentDate = QDate::currentDate();
     QString dateStr = currentDate.toString("yyyy-MM-dd");

     if(onCheck(dateStr)){

        // 根据m_userId查找users表
        QString selectSql = "SELECT id, name, department, position FROM users WHERE id = ?";
        QSqlQuery selectQuery;
        selectQuery.prepare(selectSql);
        selectQuery.addBindValue(m_userId);

        if (selectQuery.exec() && selectQuery.next()) {
            int staffId = selectQuery.value(0).toInt();
            QString name = selectQuery.value(1).toString();
            QString department = selectQuery.value(2).toString();
            QString position = selectQuery.value(3).toString();

            // 将数据插入到attendance表中，同时插入日期
            QString insertSql = "INSERT INTO attendance (staffid, name, department, position, date, status, role) VALUES (?, ?, ?, ?, ?, 1, 0)";
            QSqlQuery insertQuery;
            insertQuery.prepare(insertSql);
            insertQuery.addBindValue(staffId);
            insertQuery.addBindValue(name);
            insertQuery.addBindValue(department);
            insertQuery.addBindValue(position);
            insertQuery.addBindValue(dateStr);

            if (insertQuery.exec()) {
                QMessageBox::information(this, "成功", "打卡成功");
            } else {
                QMessageBox::critical(this, "插入失败", QString("SQL执行错误:\n%1").arg(insertQuery.lastError().text()));
            }
        } else {
            QMessageBox::information(this, "未找到记录", QString("未找到ID为 '%1' 的用户").arg(m_userId));
        }

        refreshTable();
    }
     else {
         QMessageBox::information(this, "提示", "今日已完成打卡！");
     }
}

void AttendanceCheckWidget::onEdit()
{
    // 直接弹出输入框，无需预先选择行
    QString date = QInputDialog::getText(
            this, "查询日期", "请输入要查询的日期（格式：2025-01-01）：",
            QLineEdit::Normal
        );

    // 若用户取消输入，则返回
    if (date.isEmpty()) return;

    DatabaseManager db;
    if (!db.openDB("try.db")) {
        QMessageBox::critical(this, "数据库错误", "无法打开数据库文件");
        return;
    }

    QSqlQuery selectQuery;
    selectQuery.prepare("SELECT id, staffid, name, department, position, date, status FROM attendance WHERE date = ? ");
    selectQuery.addBindValue(date);//将用户输入的日期绑定到占位符

    if (selectQuery.exec()) {
        // 清空表格
        table->setRowCount(0);

        // 填充查询结果
        while (selectQuery.next()) {
            int newRow = table->rowCount();
            table->insertRow(newRow);

            // 设置各列数据（根据实际表结构调整）
            table->setItem(newRow, 0, new QTableWidgetItem(selectQuery.value("id").toString()));
            table->setItem(newRow, 1, new QTableWidgetItem(selectQuery.value("staffid").toString()));
            table->setItem(newRow, 2, new QTableWidgetItem(selectQuery.value("name").toString()));
            table->setItem(newRow, 3, new QTableWidgetItem(selectQuery.value("department").toString()));
            table->setItem(newRow, 4, new QTableWidgetItem(selectQuery.value("position").toString()));
            table->setItem(newRow, 5, new QTableWidgetItem(selectQuery.value("phonenumber").toString()));
            table->setItem(newRow, 6, new QTableWidgetItem(selectQuery.value("date").toString()));
            table->setItem(newRow, 7, new QTableWidgetItem(selectQuery.value("status").toString()));


            // 设置单元格居中
            for (int r = 0; r < table->rowCount(); ++r) {
                for (int c = 0; c < table->columnCount(); ++c) {
                    if (QTableWidgetItem* item = table->item(r, c)) {
                        item->setTextAlignment(Qt::AlignCenter);
                    }
                }
            }
            setFixedRowHeight(table,6);

        }

        // 若没有查询到结果，提示用户
        if (table->rowCount() == 0) {
            QMessageBox::information(this, "提示", "未查询到结果！");
        }
    } else {
        QMessageBox::critical(this, "查询失败",
            QString("SQL执行错误:\n%1").arg(selectQuery.lastError().text()));
    }

    // 关闭数据库连接（如果Database类需要显式关闭）
    // db.closeDB(); // 根据实际情况添加
}
    void AttendanceCheckWidget::onDelete(){
        int row = table->currentRow();
           if (row < 0) return;
           int user_id = table->item(row, 0)->text().toInt();
           if (QMessageBox::question(this, "确认", "确定要删除该工作人员吗？") != QMessageBox::Yes) return;

           DatabaseManager db;
           db.openDB("try.db");
           QSqlQuery q1;
           q1.prepare("DELETE FROM attendance WHERE id=?");
           q1.addBindValue(user_id);
           q1.exec();


           refreshTable();
    };

    void AttendanceCheckWidget::onReturn(){
        refreshTable();
    }
/*
QSqlQuery DatabaseManager::query(const QString& sql, const QVariantList& args)
                              //参数            参数列表
    {   //用SQL语句查询
        QSqlQuery query;
        query.prepare(sql);//预处理
        for (const auto& v : args) query.addBindValue(v);//遍历args中的每个元素并将其添加到query对象的绑定值列表中
        query.exec();
        return query;
    }


*/

// 固定行高
void AttendanceCheckWidget::setFixedRowHeight(QTableWidget *table, int height)
{
        for (int i = 0; i < table->rowCount(); ++i)
            table->setRowHeight(i, height);
}

AttendanceCheckWidget::~AttendanceCheckWidget()
{
    delete ui;
}

