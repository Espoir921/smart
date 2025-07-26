#include "rosterquerywidget.h"
#include <QSqlError>
#include <QHeaderView>
#include <QMessageBox>
#include<QDebug>
#include"databasemanager.h"
RosterQueryWidget::RosterQueryWidget(QWidget *parent)
    : QWidget(parent)
{
    initDatabase();
    setupUI();
    loadAllData();
}

RosterQueryWidget::~RosterQueryWidget()
{
    if (database.isOpen()) {
        database.close();
    }
}

void RosterQueryWidget::initDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("try.db");

    if (!database.open()) {
        QMessageBox::critical(this, "数据库错误", "无法连接到数据库: " + database.lastError().text());
        return;
    }
}

void RosterQueryWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *searchLayout = new QHBoxLayout;

    staffIdEdit = new QLineEdit(this);
    staffIdEdit->setPlaceholderText("请输入员工ID");
    staffIdEdit->setMinimumWidth(200);

    queryButton = new QPushButton("查询", this);


    searchLayout->addWidget(staffIdEdit);
    searchLayout->addWidget(queryButton);

    searchLayout->addStretch();

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(10);
    tableWidget->setHorizontalHeaderLabels({"排班ID", "员工ID", "员工姓名", "部门", "职位","日期", "班次", "电话号码", "排班人ID", "排班人"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(tableWidget);

    connect(queryButton, &QPushButton::clicked, this, &RosterQueryWidget::onQueryButtonClicked);
}

void RosterQueryWidget::loadAllData()
{
    if (!database.isOpen()) {
        return;
    }

    QSqlQuery query("SELECT * FROM shifts", database);

    tableWidget->setRowCount(0);
    int row = 0;

    while (query.next()) {
        tableWidget->insertRow(row);

        for (int col = 0; col < 10; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            tableWidget->setItem(row, col, item);
        }

        row++;
    }
}
void RosterQueryWidget::queryByStaffId(int staffId) {
    DatabaseManager db;
    if (!db.openDB("try.db")) {
        QMessageBox::critical(this, "数据库错误", "无法打开数据库文件");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM shifts WHERE staffid = :staffid");
    query.bindValue(":staffid", staffId);

    if (query.exec() && query.first()) {
        tableWidget->setRowCount(0);
        tableWidget->insertRow(0);

        tableWidget->setItem(0, 0, new QTableWidgetItem(query.value("staffid").toString()));
        tableWidget->setItem(0, 1, new QTableWidgetItem(query.value("staffname").toString()));
        tableWidget->setItem(0, 2, new QTableWidgetItem(query.value("department").toString()));
        tableWidget->setItem(0, 3, new QTableWidgetItem(query.value("position").toString()));
        tableWidget->setItem(0, 4, new QTableWidgetItem(query.value("time").toString()));
        tableWidget->setItem(0, 5, new QTableWidgetItem(query.value("shift").toString()));
        tableWidget->setItem(0,6, new QTableWidgetItem(query.value("phonenumber").toString()));
        tableWidget->setItem(0, 7, new QTableWidgetItem(query.value("approverid").toString()));
        tableWidget->setItem(0, 8, new QTableWidgetItem(query.value("approver").toString()));
        // 设置单元格居中
                for (int r = 0; r < tableWidget->rowCount(); ++r) {
                    for (int c = 0; c < tableWidget->columnCount(); ++c) {
                        if (QTableWidgetItem* item = tableWidget->item(r, c)) {
                            item->setTextAlignment(Qt::AlignCenter);
                        }
                    }
                }
                setFixedRowHeight(tableWidget, 60);
    } else {
        QString errorMsg = query.lastError().isValid()? QString("查询失败: %1").arg(query.lastError().text())
            :QString("未找到用户名ID为 '%1' 的用户").arg(staffId);

        QMessageBox::information(this, "查询结果", errorMsg);
    }
}

// 固定行高
void RosterQueryWidget::setFixedRowHeight(QTableWidget *table, int height)
{
    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHeight(i, height);
}

void RosterQueryWidget::onQueryButtonClicked()
{
    bool ok;
    int staffId = staffIdEdit->text().toInt(&ok);

    if (ok) {
        queryByStaffId(staffId);
    } else {
        QMessageBox::warning(this, "输入错误", "请输入有效的员工ID");
    }
}

void RosterQueryWidget::onRefreshButtonClicked()
{
//    qDebug() << "进入刷新逻辑";
        loadAllData();
    staffIdEdit->clear();
}
