#include "houserenterwidget.h"
#include "ui_houserenterwidget.h"
#include"databasemanager.h"
#include <QMessageBox>
#include <QDate>
#include <QSqlQuery>
#include <QDebug>
#include<QMainWindow>
#include<QPushButton>
#include<QTableWidget>
#include<QHeaderView>
#include<QInputDialog>
#include<QSqlError>
HouseRenterWidget::HouseRenterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HouseRenterWidget)
{
    ui->setupUi(this);
    ui->label->setStyleSheet ("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");


       //qDebug() << "HouseRenterWidget is running";
       ui->tableWidget->setColumnCount(7); // 联系电话、房屋地址、租借状态
       ui->tableWidget->setHorizontalHeaderLabels(
                      QStringList() <<"ID"<<"用户名"<<"出租人"<< "联系电话" << "房屋地址" << "房屋租借状态"<<"出租对象");
          // 让列宽自适应内容（可选，解决“拉不开”问题）
       ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

       ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
       ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
       ui->tableWidget->setAlternatingRowColors(true);
       ui->tableWidget->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                                   "QHeaderView::section { font-weight: bold; } "
                                   "QTableWidget::item:selected { background: #2d8cf0; }");
       setFixedRowHeight(ui->tableWidget, 60);


       connect(ui->pushButton,&QPushButton::clicked,this,&HouseRenterWidget::onmyrenter);
       connect(ui->pushButton_2,&QPushButton::clicked,this,&HouseRenterWidget::onmyunrenter);
       connect(ui->pushButton_3,&QPushButton::clicked,this,&HouseRenterWidget::onrenter);
       connect(ui->pushButton_4,&QPushButton::clicked,this,&HouseRenterWidget::onunrenter);
       connect(ui->pushButton_5,&QPushButton::clicked,this,&HouseRenterWidget::onReturn);
       refreshTable();

}

// 固定行高
void HouseRenterWidget::setFixedRowHeight(QTableWidget *table, int height)
{
    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHeight(i, height);
}


void HouseRenterWidget::refreshTable(){
    DatabaseManager db;
    db.openDB("try.db");

    QString sql = "SELECT houserenter.id, houserenter.username, houserenter.lessor,houserenter.phonenumber,houserenter.address,houserenter.housestatus,houserenter.tenant FROM houserenter ";

    QSqlQuery query=db.query(sql,QVariantList());
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0); //清空表格原有内容
    int row=0;
    while(query.next()){
        ui->tableWidget->insertRow(row);

        QTableWidgetItem *item0 = new QTableWidgetItem(query.value(0).toString());//id
        ui->tableWidget->setItem(row, 0, item0);

        QTableWidgetItem *item1 = new QTableWidgetItem(query.value(1).toString());// 第1列username
        ui->tableWidget->setItem(row, 1, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem(query.value(2).toString()); // 第2列lessor
        ui->tableWidget->setItem(row, 2, item2);

        QTableWidgetItem *item3 = new QTableWidgetItem(query.value(3).toString());// 第3列phonenumber
        ui->tableWidget->setItem(row, 3, item3);

        QTableWidgetItem *item4 = new QTableWidgetItem(query.value(4).toString());//第4列address
        ui->tableWidget->setItem(row, 4, item4);
        QString val=query.value(5).toString();
        if(val=="1"){
            val="已出租";
        }
        else if(val=="0"){
            val="空闲";
        }
        QTableWidgetItem *item5 = new QTableWidgetItem(val);//房屋出租状态
        ui->tableWidget->setItem(row, 5, item5);

        QTableWidgetItem *item6 = new QTableWidgetItem(query.value(6).toString());//第4列address
        ui->tableWidget->setItem(row, 6, item6);

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
void HouseRenterWidget::onmyrenter(){
    bool ok;
           QString username = QInputDialog::getText(this, "新增出租消息", "用户名：", QLineEdit::Normal, "", &ok);
           if (!ok  ) return;
           QString lessor = QInputDialog::getText(this, "新增出租消息", "出租人：", QLineEdit::Normal, "", &ok);
           if (!ok ) return;
           QString phonenumber = QInputDialog::getText(this, "新增出租消息", "联系电话：", QLineEdit::Normal, "", &ok);
                  if (!ok) return;
           QString address = QInputDialog::getText(this, "新增出租消息", "出租地址：", QLineEdit::Normal, "", &ok);
           if (!ok) return;


           DatabaseManager db;
           if (!db.openDB("try.db")) {
                  QMessageBox::critical(this, "数据库错误", "无法打开数据库文件");
                  return;
              }

           QSqlQuery q;
           q.prepare("INSERT INTO houserenter(username, lessor,phonenumber,   address,housestatus) VALUES (?, ?,?, ?,?)");
           q.addBindValue(username);
           q.addBindValue(lessor);
           q.addBindValue(phonenumber);
           q.addBindValue(address);
           q.addBindValue(0);  // 原代码中 status 字段值未获取，这里假设补 1，实际需按逻辑调整
           if (!q.exec()) {
               qDebug() << "SQL 执行失败：" << q.lastError().text(); // 打印详细错误
               QMessageBox::warning(this, "失败", "添加账号失败，用户名可能已存在！");
               return;
           }
           else{
              refreshTable();

           }
}
void HouseRenterWidget::onmyunrenter(){
    int row =ui->tableWidget->currentRow();
               if (row < 0) return;
               int id = ui->tableWidget->item(row, 0)->text().toInt();
               if (QMessageBox::question(this, "确认", "确定要删除该条租房消息？") != QMessageBox::Yes) return;

               DatabaseManager db;
               db.openDB("try.db");
               QSqlQuery q1;
               q1.prepare("DELETE FROM houserenter WHERE id = ?");
               q1.addBindValue(id);
               q1.exec();

               refreshTable();
}
void HouseRenterWidget::onrenter(){

       int row = ui->tableWidget->currentRow();// 获取表格当前选中行
       if (row < 0) {
           QMessageBox::warning(this, "提示", "请先选中一行出租信息！");
           return;
       }
       QTableWidgetItem *idItem = ui->tableWidget->item(row, 0);// 获取选中行的 ID
       if (!idItem || idItem->text().isEmpty()) {
           QMessageBox::warning(this, "错误", "未找到有效的房屋！");
           return;
       }
       QString houseId = idItem->text();
       bool ok;
           QString tenantl = QInputDialog::getText(this, "输入租户信息","请输入租户姓名或相关信息：",QLineEdit::Normal,"", &ok);
           DatabaseManager db;

           if (!db.openDB("try.db")) {
               QMessageBox::critical(this, "数据库错误", "无法打开数据库文件");
               return;
           }
           if(ok){QSqlQuery q1;// SQL 语
                  q1.prepare("UPDATE houserenter SET  tenant =? WHERE id =? ");
                  q1.addBindValue(tenantl);
                  q1.addBindValue(houseId);
                  if (!q1.exec()) { //必须调用 exec() 才会真正执行更新
                              qDebug() << "SQL 执行失败：" << q1.lastError().text();
                              QMessageBox::warning(this, "失败", "更新租户信息失败！");
                              return;
                          }

          }
           if (!ok) {  // 用户点击取消
                   QMessageBox::information(this, "提示", "已取消出租操作");
                   return;
               }
             else  if (tenantl.isEmpty()) {  // 输入为空
                   QMessageBox::warning(this, "错误", "租户信息不能为空，请重新操作");
                   return;
               }



       QSqlQuery q;// SQL 语句：更新 houserenter 表，把 housestatus 字段从 '0' 改为 '1'，通过 id 定位
       q.prepare("UPDATE houserenter SET housestatus = '1' WHERE id = ? AND housestatus = '0'");
       q.addBindValue(houseId);
      //refreshTable();
       // 调试输出，方便排查问题
//       qDebug() << "执行 SQL:" << q.lastQuery();
//       qDebug() << "绑定参数:" << q.boundValues();

       if (!q.exec()) {
           qDebug() << "SQL 执行失败：" << q.lastError().text();
           QMessageBox::warning(this, "失败", "更新出租信息状态失败！");
       } else {
           // 判断是否有记录被更新
           if (q.numRowsAffected() > 0) {
               QMessageBox::information(this, "成功", "该房屋已被出租");
               refreshTable(); // 刷新表格，显示最新数据
           } else {
               QMessageBox::information(this, "提示", "该房屋已被出租");
           }
       }
}
void HouseRenterWidget::onunrenter(){
    // 获取表格当前选中行
           int row = ui->tableWidget->currentRow();
           if (row < 0) {
               QMessageBox::warning(this, "提示", "请先选中一行出租信息！");
               return;
           }

           // 获取选中行的 ID
           QTableWidgetItem *idItem = ui->tableWidget->item(row, 0);
           if (!idItem || idItem->text().isEmpty()) {
               QMessageBox::warning(this, "错误", "未找到有效的房屋 ID！");
               return;
           }
           QString houseId = idItem->text();

           DatabaseManager db;
           if (!db.openDB("try.db")) {
               QMessageBox::critical(this, "数据库错误", "无法打开数据库文件");
               return;
           }


           QSqlQuery q1;// SQL 语句
           q1.prepare("UPDATE houserenter SET tenant = NULL WHERE id =?");
           q1.addBindValue(houseId);
           if (!q1.exec()) {
               qDebug() << "清空 tenant 失败:" << q1.lastError().text();
           }
           QSqlQuery q;
           q.prepare("UPDATE houserenter SET housestatus = '0' WHERE id = ? AND housestatus = '1' ");
           q.addBindValue(houseId);

           // 调试输出，方便排查问题
           qDebug() << "执行 SQL:" << q.lastQuery();
           qDebug() << "绑定参数:" << q.boundValues();

           if (!q.exec()) {
               qDebug() << "SQL 执行失败：" << q.lastError().text();
               QMessageBox::warning(this, "失败", "更新出租信息状态失败！");
           } else {
               // 判断是否有记录被更新
               if (q.numRowsAffected() > 0) {
                   QMessageBox::information(this, "成功", "该房屋恢复空闲");
                   refreshTable(); // 刷新表格，显示最新数据
               } else {
                   QMessageBox::information(this, "提示", "未找到状态为 '1' 的对应记录，无需更新");
               }
           }
}
void HouseRenterWidget::onReturn(){
    refreshTable();
}
HouseRenterWidget::~HouseRenterWidget()
{
    delete ui;
}
