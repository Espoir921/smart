#include "repairworkorderwidget.h"
#include "ui_repairworkorderwidget.h"
#include"databasemanager.h"
#include<QDebug>
#include<QTableWidget>
#include<QHeaderView>
#include<QPushButton>
#include<QMessageBox>
#include<QInputDialog>
#include<QSqlError>
#include<QMessageBox>
#include<QSqlQuery>

RepairWorkOrderWidget::RepairWorkOrderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RepairWorkOrderWidget)
{
    ui->setupUi(this);
//    qDebug() << "RepairWorkOrderWidget is running";
    ui->label->setStyleSheet ("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");

    ui->tableWidget->setColumnCount(10);
    ui->tableWidget->setHorizontalHeaderLabels(
    QStringList() <<"报修编号"<<"报修人用户名"<<"业主姓名"<< "业主地址" << "报修时间" << "报修描述"<<"报修状态"<<"维修日期"<<"处理人" <<"评价");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                                   "QHeaderView::section { font-weight: bold; } "
                                   "QTableWidget::item:selected { background: #2d8cf0; }");
    setFixedRowHeight(ui->tableWidget, 60);

    // 编辑按钮（绿色）
    QString editStyle = "QPushButton { background-color: #19be6b; color: white; border-radius: 6px; "
                            "font-family: 等线; font-weight: bold;}"
                            "QPushButton:hover { background-color: #13a456; }";
    ui->pushButton_2->setStyleSheet(editStyle);
    ui->pushButton_2->setFixedSize(60, 30);

    // 返回按钮（蓝色）
    QString returnStyle = "QPushButton { background-color: #2d8cf0; color: white; border-radius: 6px; "
                           "font-family: 等线; font-weight: bold;}"
                           "QPushButton:hover { background-color: #1a73e8; }";
    ui->pushButton_5->setStyleSheet(returnStyle);
    ui->pushButton_5->setFixedSize(60, 30);

    // 删除按钮（红色）
    QString deleteStyle = "QPushButton { background-color: #f56c6c; color: white; border-radius: 6px; "
                           "font-family: 等线; font-weight: bold;}"
                           "QPushButton:hover { background-color: #f56c6c; }";
    ui->pushButton_3->setStyleSheet(deleteStyle);
    ui->pushButton_3->setFixedSize(60, 30);



            connect(ui->pushButton_2,&QPushButton::clicked,this,&RepairWorkOrderWidget::onchange);
            connect(ui->pushButton_3,&QPushButton::clicked,this,&RepairWorkOrderWidget::ondelete);
            connect(ui->pushButton_5,&QPushButton::clicked,this,&RepairWorkOrderWidget::onReturn);
            refreshTable();

}

// 固定行高
void RepairWorkOrderWidget::setFixedRowHeight(QTableWidget *table, int height)
{
    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHeight(i, height);
}

void RepairWorkOrderWidget::refreshTable(){
    DatabaseManager db;
       db.openDB("try.db");

       QString sql= "SELECT maintenance.requestid,maintenance.username,maintenance.phonenumber,maintenance.location,maintenance.reporttime,maintenance.description,maintenance.status,maintenance.maintenancedate,maintenance.handler,maintenance.evaluation FROM maintenance ";

       QSqlQuery query = db.query(sql,QVariantList());
       ui->tableWidget->clearContents();
       ui->tableWidget->setRowCount(0); //清空表格原有内容
   int row=0;
   while(query.next()){
           ui->tableWidget->insertRow(row);

           QTableWidgetItem*item0 = new QTableWidgetItem(query.value(0).toString());//requestid
           ui->tableWidget->setItem(row, 0, item0);

           QTableWidgetItem*item1 = new QTableWidgetItem(query.value(1).toString());// 第1列username
           ui->tableWidget->setItem(row, 1, item1);

           QTableWidgetItem*item2 = new QTableWidgetItem(query.value(2).toString()); // 第2列phonenumber
           ui->tableWidget->setItem(row, 2, item2);

           QTableWidgetItem *item3 = new QTableWidgetItem(query.value(3).toString());// 第3列location
           ui->tableWidget->setItem(row, 3, item3);

           QTableWidgetItem *item4 = new QTableWidgetItem(query.value(4).toString());// 第4列reporttime
           ui->tableWidget->setItem(row, 4, item4);

           QTableWidgetItem*item5 = new QTableWidgetItem(query.value(5).toString());//第5列description
           ui->tableWidget->setItem(row, 5, item5);
           QString val=query.value(6).toString();
   if(val=="1"){
               val="已维修";
   }
   else if(val=="0"){
               val="未维修";
   }
           QTableWidgetItem*item6 = new QTableWidgetItem(val);//status
           ui->tableWidget->setItem(row, 6, item6);

           QTableWidgetItem *item7 = new QTableWidgetItem(query.value(7).toString());//第7列maintenancedate
           ui->tableWidget->setItem(row, 7, item7);

           QTableWidgetItem *item8 = new QTableWidgetItem(query.value(8).toString());//第8列handler
           ui->tableWidget->setItem(row, 8, item8);

           QTableWidgetItem *item9 = new QTableWidgetItem(query.value(9).toString());//第9列evaluation
           ui->tableWidget->setItem(row, 9, item9);

           row++;
   }
   // 设置单元格居中
           for (int r = 0; r < ui->tableWidget->rowCount(); ++r) {
               for (int c = 0; c < ui->tableWidget->columnCount(); ++c) {
                   if (QTableWidgetItem* item = ui->tableWidget->item(r, c)) {
                       item->setTextAlignment(Qt::AlignCenter);
                   }
               }
           }
           setFixedRowHeight(ui->tableWidget, 60);

}
void RepairWorkOrderWidget::onchange(){
    int row = ui->tableWidget->currentRow();// 获取表格当前选中行
          if (row < 0) {
              QMessageBox::warning(this, "提示", "请先选中一行出租信息！");
              return;
          }
          QTableWidgetItem*idItem = ui->tableWidget->item(row, 0);
          QString questId= idItem->text();
    bool ok;
    QMessageBox::StandardButton ret = QMessageBox::question(this, "搜索", "该设备是否已完成维修？", QMessageBox::Yes | QMessageBox::No);
    bool isCompleted = (ret == QMessageBox::Yes); // 根据返回值判断用户选择
    QString content = isCompleted ? "是" : "否";
    QString content2 = QInputDialog::getMultiLineText(this,"搜索","处理人的名字是？","",&ok);
    QString day = QInputDialog::getMultiLineText(this,"搜索","维修日期是？","",&ok);
    DatabaseManager db;
    if (!db.openDB("try.db")) {
                   QMessageBox::critical(this, "数据库错误", "无法打开数据库文件");
                   return;
               }
    if(content=="是"){
        QSqlQuery q;// SQL 语句
               q.prepare("UPDATE maintenance SET status = '1' WHERE requestid = ? AND status = '0'");
               q.addBindValue(questId);
               if(!q.exec()){ // 补充执行及错误处理
                       qDebug() << "SQL 执行失败: " << q.lastError().text();
                   }
               QSqlQuery q1;
                              q1.prepare("UPDATE maintenance SET handler =?, maintenancedate=? WHERE requestid=?");
                              q1.addBindValue(content2);
                              q1.addBindValue(day);

                              q1.addBindValue(questId);
                              if(!q1.exec()){ // 补充执行及错误处理
                                                     qDebug()<< q.lastError().text();
                                                 }
    }
    else{
        QSqlQuery q;// SQL 语句
                       q.prepare("UPDATE maintenance SET status = '0' WHERE requestid = ? AND status = '1'");
                       q.addBindValue(questId);
                       if(!q.exec()){ // 补充执行及错误处理
                              qDebug() << "SQL 执行失败: " << q.lastError().text();
                          }
    }
    refreshTable();
}
void RepairWorkOrderWidget::ondelete(){
    int row =ui->tableWidget->currentRow();
    if (row < 0) return;
    int id = ui->tableWidget->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "确认", "确定要删除该条维修日期？") != QMessageBox::Yes) return;

    DatabaseManager db;
    db.openDB("try.db");
    QSqlQuery q1;
    q1.prepare("DELETE FROM maintenance WHERE requestid=?");
    q1.addBindValue(id);
    q1.exec();

    refreshTable();
}

void RepairWorkOrderWidget::onReturn(){
refreshTable();
}


RepairWorkOrderWidget::~RepairWorkOrderWidget()
{
    delete ui;
}
