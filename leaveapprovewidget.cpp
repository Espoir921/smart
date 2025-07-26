#include "leaveapprovewidget.h"
#include "ui_leaveapprovewidget.h"
#include "databasemanager.h"
#include "login.h"

#include <QLabel>
#include <QStyle>
#include <QApplication>
#include <QSpacerItem>
#include <QTableWidget>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSqlQuery>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QTableWidgetItem>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDateTime>

LeaveApproveWidget::LeaveApproveWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LeaveApproveWidget)
{
    ui->setupUi(this);
    choose = 1;

    label = ui->label;
    label->setStyleSheet ("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");
    tabwidget = ui->tabWidget;
    tabwidget->tabBar()->setStyleSheet("QTabBar::tab{min-height: 50px; min-width: 80px;}");


    //已处理界面
    //表格美化
    table1 = ui->tableWidget;
    table1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table1->setSelectionBehavior(QAbstractItemView::SelectRows);
    table1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table1->setAlternatingRowColors(true); // 斑马线
    table1->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QTableWidget::item:selected { background: #2d8cf0;  }");

    //查询筛选按键
    radioID = ui->radioId;
    radioId = ui->radioid;
    radioName = ui->radioName;
    radioHandler = ui->radioHandler;
    radioDate = ui->radioDate;
    btnSearch = ui->searchButton;
    QString btnStyle = "QPushButton { background-color: #19be6b; color: white; border-radius: 6px; font-family: 等线; font-weight: bold;}"
                       "QPushButton:hover { background-color: #13a456; }";
    btnSearch->setStyleSheet(btnStyle);
    btnSearch->setFixedSize(80,30);

    searchGroup = new QButtonGroup(this);
    searchGroup->addButton(radioID,1);
    searchGroup->addButton(radioId, 2);
    searchGroup->addButton(radioName, 3);
    searchGroup->addButton(radioDate, 4);
    searchGroup->addButton(radioHandler, 5);

    radioID->setChecked(true);

    //未处理界面
    table0 = ui->tableWidget_2;
    table0->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table0->setSelectionBehavior(QAbstractItemView::SelectRows);
    table0->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table0->setAlternatingRowColors(true); // 斑马线
    table0->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QTableWidget::item:selected { background: #2d8cf0;  }");

    //按钮美化
    btnHandle = ui->pushButton;
    btnHandle->setStyleSheet(btnStyle);
    btnHandle->setFixedSize(80,35);

    //连接信号和槽
    connect(searchGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [&](int id) {
        choose = id;
    });
    connect(btnSearch, &QPushButton::clicked, this, &LeaveApproveWidget::onSearch);
    connect(btnHandle, &QPushButton::clicked, this, &LeaveApproveWidget::onHandle);


    refreshTable();

}

LeaveApproveWidget::~LeaveApproveWidget()
{
    delete ui;
}

//已处理界面搜索键
void LeaveApproveWidget::onSearch(){
    QString searchText = ui->lineEdit->text().trimmed();
    if (searchText.isEmpty()) {
        refreshTable();
        return;
    }

    table1->setRowCount(0);
    DatabaseManager& db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query;
    int rowCount = 0;
    QString queryStr;
    switch (choose) {
    case 1: //按订单编号搜索
        queryStr = "SELECT leaveid, staffid, staffname, start_date, end_date, leavereason, approvalresult, approverid, approver, approvaltime "
                           "FROM leaves WHERE approvalprogress = 1 AND leaveid = ?";
        break;
    case 2: // 按 ID 搜索
        queryStr = "SELECT leaveid, staffid, staffname, start_date, end_date, leavereason, approvalresult, approverid, approver, approvaltime "
                           "FROM leaves WHERE approvalprogress = 1 AND staffid = ?";
        break;
    case 3: // 按姓名搜索
        queryStr = "SELECT leaveid, staffid, staffname, start_date, end_date, leavereason, approvalresult, approverid, approver, approvaltime "
                           "FROM leaves WHERE approvalprogress = 1 AND staffname = ?";
        break;
    case 4: // 按日期搜索
    {
        // 检查是否请假
        QSqlQuery leaveQuery;
        leaveQuery.prepare("SELECT start_date, end_date, leaveid FROM leaves");
        if (leaveQuery.exec()) {
            QDate current = QDate::fromString(searchText, "yyyy-MM-dd");
            while (leaveQuery.next()) {
                QDate start = QDate::fromString(leaveQuery.value(0).toString(), "yyyy-MM-dd");
                QDate end = QDate::fromString(leaveQuery.value(1).toString(), "yyyy-MM-dd");
                int id = leaveQuery.value(2).toInt();
//                qDebug()<<"id是"<<id;
                if (current >= start && current <= end ) {
                    queryStr = "SELECT leaveid, staffid, staffname, start_date, end_date, leavereason, approvalresult, approverid, approver, approvaltime "
                               "FROM leaves WHERE approvalprogress = 1 AND leaveid = ?";
                    query.prepare(queryStr);
                    query.addBindValue(id);
                    query.exec();
                    while (query.next()) {
                        table1->insertRow(rowCount);
                        for (int col = 0; col < 10; ++col) {
                            QString val = query.value(col).toString();
                            if (col == 6) {
                                int status = query.value(col).toInt();
                                if (status == 0) val = "已驳回";
                                else if (status == 1) val = "已批准";
                            }
                            table1->setItem(rowCount, col, new QTableWidgetItem(val));
                        }
                        ++rowCount;
                    }
                }
            }
        }
    }
        break;
    case 5: // 按处理人搜索
        queryStr = "SELECT leaveid, staffid, staffname, start_date, end_date, leavereason, approvalresult, approverid, approver, approvaltime "
                           "FROM leaves WHERE approvalprogress = 1 AND approver = ?";
        break;
    default:
        refreshTable();
        return;
    }


    if (choose != 4)
    {
        query.prepare(queryStr);
        query.addBindValue(searchText);
        query.exec();


    while (query.next()) {
        table1->insertRow(rowCount);
        for (int col = 0; col < 10; ++col) {
            QString val = query.value(col).toString();
            if (col == 6) {
                int status = query.value(col).toInt();
                if (status == 0) val = "已驳回";
                else if (status == 1) val = "已批准";
            }
            table1->setItem(rowCount, col, new QTableWidgetItem(val));
        }
        ++rowCount;
    }


    }
    for (int row = 0; row < table1->rowCount(); ++row) {
        for (int col = 0; col < table1->columnCount(); ++col) {
            QTableWidgetItem *item = table1->item(row, col);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    table1->resizeRowsToContents();
    if (rowCount == 0) {
        QMessageBox::information(this, "提示", "未找到符合条件的记录!");
    }
}

// 设置用户名的函数实现
void LeaveApproveWidget::setUserId(const int userId)
{
    m_userId = userId;

}


//未处理界面审核键
void LeaveApproveWidget::onHandle(){

    //选择
    int row = table0->currentRow();
    if(row<0) return;
    int id = table0->item(row,0)->text().toInt();

    // 初始化 result 为 -1
    int result = -1;

    // 弹出消息框，显示“同意”和“驳回”按钮
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "审核", "是否批准该条申请？",QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    // 根据用户选择设置 result
    if (reply == QMessageBox::Yes) {    // 用户点击了“同意”
        result = 1;
    } else {   // 用户点击了“驳回”
        result = 0;
    }


    // 审核员签名比对
    QString auditorSignature = QInputDialog::getText(this, "输入签名", "审核员签名：");
    // 若用户点击取消，getText 返回空字符串，可根据需求处理，比如直接 return
    if (auditorSignature.isEmpty()) {
        return;
    }
    DatabaseManager& db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query;
    query.prepare("SELECT name FROM users WHERE id = ?");
    query.addBindValue(m_userId);
    QString currentAuditor;
    if (query.exec() && query.next()){
        currentAuditor = query.value("name").toString();

        QSqlError error = query.lastError();
        qDebug() << "Insert Error:" << error.text();

        if (auditorSignature != currentAuditor) {
            // 签名错误，审核失败
            QMessageBox::critical(this, "错误", "签名错误，审核失败！");
            return;
        }
        else {
            QMessageBox::information(this, "提示", "审核完成！");


            // 插入审核结果到数据库
            QString insertQuery = QString("UPDATE leaves SET approvalprogress = 1, approvalresult = '%1', approverid = '%2', approver = '%3', approvaltime = '%4' WHERE leaveid = '%5' ")
                    .arg(result)
                    .arg(m_userId)
                    .arg(currentAuditor)
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                         .arg(id);
            query.exec(insertQuery);
            refreshTable();
        }
    }


}


 //刷新界面
void LeaveApproveWidget::refreshTable() {
    table1->setRowCount(0);
    table0->setRowCount(0);
    DatabaseManager& db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query1 = db.query("SELECT leaveid, staffid, staffname, start_date, end_date, leavereason, approvalresult, approverid, approver, approvaltime FROM leaves WHERE approvalprogress = 1");
    int row = 0;
    while (query1.next()) {
        table1->insertRow(row);
        for (int col = 0; col < 10; ++col) {
            QString val = query1.value(col).toString();
            if (col == 6) {
                int status = query1.value(col).toInt();
                if (status == 0) val = "已驳回";
                else if (status == 1) val = "已批准";
            }
            table1->setItem(row, col, new QTableWidgetItem(val));
        }
        ++row;
    }
    int rowcount = 0;
    QSqlQuery query0 = db.query("SELECT leaveid, staffid, staffname, start_date, end_date, leavereason, approvalresult, approver FROM leaves WHERE approvalprogress = 0");
    while (query0.next()) {
        table0->insertRow(rowcount);
        for (int col = 0; col < 8; ++col) {
            QString val = query0.value(col).toString();
            table0->setItem(rowcount, col, new QTableWidgetItem(val));
        }
        ++rowcount;
    }
    setFixedRowHeight(60);

    //设置居中
    for (int row = 0; row < table1->rowCount(); ++row) {
        for (int col = 0; col < table1->columnCount(); ++col) {
            QTableWidgetItem *item = table1->item(row, col);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    for (int row = 0; row < table0->rowCount(); ++row) {
        for (int col = 0; col < table0->columnCount(); ++col) {
            QTableWidgetItem *item = table0->item(row, col);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

}



// 遍历所有行，设置行高
void LeaveApproveWidget::setFixedRowHeight(int height) {
    for (int row = 0; row < table1->rowCount(); ++row) {
        table1->setRowHeight(row, height);
    }
    for (int row = 0; row < table0->rowCount(); ++row) {
        table0->setRowHeight(row, height);
    }
}
