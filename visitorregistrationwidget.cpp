#include "visitorregistrationwidget.h"
#include "ui_visitorregistrationwidget.h"
#include"databasemanager.h"
#include <QMessageBox>
#include <QInputDialog>
#include<QTableWidget>
#include<QComboBox>
#include<QPushButton>
#include<QDebug>
#include<QSqlError>
#include<QSqlDatabase>
#include<QDateEdit>
VisitorRegistrationWidget::VisitorRegistrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisitorRegistrationWidget)
{
    ui->setupUi(this);

    ui->label->setStyleSheet ("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");

//  qDebug() << "RepairWorkOrderWidget is running";
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() <<"访客编号"<<"访客姓名"<<"来访日期"<< "访问地址" << "访客联系电话" << "登记人");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                                   "QHeaderView::section { font-weight: bold; } "
                                   "QTableWidget::item:selected { background: #2d8cf0; }");
    setFixedRowHeight(ui->tableWidget, 60);
    refreshTable();

    connect(ui->pushButton, &QPushButton::clicked, this, &VisitorRegistrationWidget::onadd);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &VisitorRegistrationWidget::onEditButtonClicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &VisitorRegistrationWidget::onSearchButtonClicked);
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &VisitorRegistrationWidget::onComboBoxCurrentIndexChanged);

    connect(ui->pushButton_4, &QPushButton::clicked, this, &VisitorRegistrationWidget::refreshTable);
}

// 固定行高
void VisitorRegistrationWidget::setFixedRowHeight(QTableWidget *table, int height)
{
    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHeight(i, height);
}


void VisitorRegistrationWidget::refreshTable() {
    DatabaseManager db;
          db.openDB("try.db");

          QString sql= "SELECT visiter.visiterid, visiter.visiterusername, visiter.visittime, "
                  "visiter.address, visiter.visitphone, visiter.handler FROM visiter";

          QSqlQuery query=db.query(sql,QVariantList());
          ui->tableWidget->clearContents();
          ui->tableWidget->setRowCount(0); //清空表格原有内容
      int row=0;


      //调试
//     QSqlError error = query.lastError();
//     qDebug()<<"last error is："<< error.text();


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

VisitorRegistrationWidget::~VisitorRegistrationWidget()
{
    delete ui;
}

void VisitorRegistrationWidget::onSearchButtonClicked()
{
    QString condition = getSearchCondition();
    if (!condition.isEmpty()) {
        queryAndUpdateTable(condition);
    }
}

void VisitorRegistrationWidget::onEditButtonClicked()
{
    QTableWidgetItem *selectedItem = ui->tableWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "提示", "请先选中要修改的行！");
        return;
    }
    int row = selectedItem->row();
    int column = selectedItem->column();

    QString columnName = ui->tableWidget->horizontalHeaderItem(column)->text();

    if (columnName == "访客姓名") columnName = "visiterusername";
    else if (columnName == "来访日期") columnName = "visittime";
    else if (columnName == "访问地址") columnName = "address";
    else if (columnName == "访客联系电话") columnName = "visitphone";
    else if (columnName == "登记人") columnName = "handler";
    else if (columnName == "访客编号"){
       QMessageBox::information(this, "提示", "访客编号不可更改");
       return;
    }
    QString originalValue = selectedItem->text();
    QString editValue = getEditContent(originalValue);
    if (!editValue.isEmpty()) {
        ui->tableWidget->setItem(row, column, new QTableWidgetItem(editValue));
        DatabaseManager db = DatabaseManager::getInstance();
        db.DatabaseManager::openDB(databasePath);
        QSqlQuery query;

        query.prepare(QString("UPDATE visiter SET %1 = :value WHERE visiterid = :visiterid").arg(columnName));
        query.bindValue(":value", editValue);
        query.bindValue(":visiterid", ui->tableWidget->item(row, 0)->text());

        if (!query.exec()) {
            QMessageBox::critical(this, "数据库错误", "修改数据失败！");
        }

        refreshTable();
    }
}

void VisitorRegistrationWidget::onComboBoxCurrentIndexChanged(int index)
{
    currentSearchType = index;
}

QString VisitorRegistrationWidget::getSearchCondition()
{
    QString prompt = "";
    if (currentSearchType == 0) {
        prompt = "请输入要查询的访客姓名：";
    } else if (currentSearchType == 1) {
        prompt = "请输入要查询的访问地址：";
    } else if (currentSearchType == 2) {
        prompt = "请输入要查询的访问日期：";
    }
    return QInputDialog::getText(this, "输入查询条件", prompt);
}

QString VisitorRegistrationWidget::getEditContent(const QString &originalValue)
{
    return QInputDialog::getText(this, "修改内容", "请输入修改后的值（原内容：" + originalValue + "）：");
}

void VisitorRegistrationWidget::queryAndUpdateTable(const QString &condition)
{
    QString sql = "";
    if (currentSearchType == 0) {
        sql = QString("SELECT * FROM visiter WHERE visiterusername = '%1'").arg(condition);
//        qDebug()<<"sql语句是"<<sql;
    } else if (currentSearchType == 1) {
        sql = QString("SELECT * FROM visiter WHERE address = '%1'").arg(condition);
    } else if (currentSearchType == 2) {
        sql = QString("SELECT * FROM visiter WHERE visittime = '%1'").arg(condition);
    }

    DatabaseManager db = DatabaseManager::getInstance();
    db.DatabaseManager::openDB(databasePath);
    QSqlQuery query(sql);
    ui->tableWidget->setRowCount(0);
    while (query.next()) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        for (int col = 0; col < ui->tableWidget->columnCount(); col++) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui->tableWidget->setItem(row, col, item);
        }
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

void VisitorRegistrationWidget::onadd(){

    DatabaseManager db ;
        if (!db.openDB("try.db")) {
            QMessageBox::critical(this, "数据库错误", "无法连接数据库！");
            return;
        }
    QString name = QInputDialog::getText(this, "添加访客", "来访者姓名：");
    // 替换原 dateEdit 相关代码
    QDateTimeEdit *dateEdit = new QDateTimeEdit(this);
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    // 直接获取时间，无需 exec
    QDateTime selectedTime = dateEdit->dateTime();
    QString time = selectedTime.toString("yyyy-MM-dd");

    // 后续判空逻辑前，确保 time 已初始化
    // 比如前面加默认值：QString time = "";
    QString place = QInputDialog::getText(this, "添加访客", "访问地点：");
    QString phone = QInputDialog::getText(this, "添加访客", "访问者联系电话：");
    QString approver = QInputDialog::getText(this, "添加访客", "登记人：");
    if ( name.isEmpty() || time.isEmpty() || place.isEmpty() || phone.isEmpty() || approver.isEmpty()) {
            QMessageBox::warning(this, "警告", "请填写完整的访客信息！");
            return;
        }
    QSqlQuery query;
    query.prepare("INSERT INTO visiter ( visiterusername, visittime, address, visitphone, handler) VALUES ( :visiterusername, :visittime, :address, :visitphone, :handler)");
    query.bindValue(":visiterusername", name);
    query.bindValue(":visittime", time);
    query.bindValue(":address", place);
    query.bindValue(":visitphone", phone);
    query.bindValue(":handler", approver);
    if (query.exec()) {
        QMessageBox::information(this, "成功", "添加访客信息成功！");

    } else {
        QMessageBox::critical(this, "失败", "添加访客信息失败！");
    }
    refreshTable();
}
