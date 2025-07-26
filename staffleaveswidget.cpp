#include "staffleaveswidget.h"
#include "ui_staffleaveswidget.h"
#include "databasemanager.h"
#include <QSqlError>
#include <QHeaderView>
#include <QMessageBox>
#include<QFormLayout>

StaffLeavesWidget::StaffLeavesWidget(QWidget *parent)
    : QWidget(parent)
   , ui(new Ui::StaffLeavesWidget)
{
    ui->setupUi(this);
    setWindowTitle("员工请假管理");
    initDatabase();
    initTabWidget();
    initLeaveForm();

}

StaffLeavesWidget::~StaffLeavesWidget()
{
    db.close();
    delete ui;
}

//获取当前用户名
void StaffLeavesWidget::setUserId(const int userId)
{
    m_userId = userId;
    loadUserInfo(); // 加载用户信息
    loadPendingData();
    loadApprovedData();
}

void StaffLeavesWidget::initDatabase()
{
    // 以 SQLite 为例，其他数据库需调整驱动名和连接参数
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("try.db"); // 替换实际数据库文件名
    if (!db.open()) {
        QMessageBox::critical(this, "数据库错误", "打开数据库失败：" + db.lastError().text());
    }
}

void StaffLeavesWidget::initTabWidget()
{
    tabWidget = new QTabWidget(this);

    // 待审批页面
    pendingTab = new QWidget();
    pendingTable = new QTableWidget(pendingTab);
    pendingTable->setColumnCount(8); // 对应 leaves 表字段：staffid、staffname、start_date、end_date、leavereason、approvalprogress、approvalresult、approverid、approver、approvaltime（根据实际需求调整列数）
    pendingTable->setHorizontalHeaderLabels(QStringList() << "员工ID" << "姓名" << "开始日期" << "结束日期" << "请假原因" << "审核进度" << "审核结果" << "审核人ID");
    pendingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QVBoxLayout *pendingLayout = new QVBoxLayout(pendingTab);
    QPushButton *refreshPendingBtn = new QPushButton("刷新待审批列表", pendingTab);
    connect(refreshPendingBtn, &QPushButton::clicked, this, &StaffLeavesWidget::onRefreshPendingClicked);
    pendingLayout->addWidget(refreshPendingBtn);
    pendingLayout->addWidget(pendingTable);

    // 已审批页面
    approvedTab = new QWidget();
    approvedTable = new QTableWidget(approvedTab);
    approvedTable->setColumnCount(10); // 按需调整列数，覆盖 leaves 表所有字段
    approvedTable->setHorizontalHeaderLabels(QStringList() << "员工ID" << "姓名" << "开始日期" << "结束日期" << "请假原因" << "审核进度" << "审核结果" << "审核人ID" << "审核人姓名" << "审核时间");
    approvedTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QVBoxLayout *approvedLayout = new QVBoxLayout(approvedTab);
    QPushButton *refreshApprovedBtn = new QPushButton("刷新已审批列表", approvedTab);
    connect(refreshApprovedBtn, &QPushButton::clicked, this, &StaffLeavesWidget::onRefreshApprovedClicked);
    approvedLayout->addWidget(refreshApprovedBtn);
    approvedLayout->addWidget(approvedTable);

    // 将页面加入 TabWidget
    tabWidget->addTab(pendingTab, "待审批");
    tabWidget->addTab(approvedTab, "已审批");

    // 布局：TabWidget 占主窗口
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);

    loadPendingData();
    loadApprovedData();
}

void StaffLeavesWidget::initLeaveForm()
{
    // 表单组件
    staffIdLabel = new QLabel(this);
    staffNameLabel = new QLabel(this);
    startDateEdit = new QDateEdit(this);
    startDateEdit->setDisplayFormat("yyyy-MM-dd");
    endDateEdit = new QDateEdit(this);
    endDateEdit->setDisplayFormat("yyyy-MM-dd");
    reasonEdit = new QTextEdit(this);
    reasonEdit->setPlaceholderText("输入请假原因");
    submitBtn = new QPushButton("提交请假申请", this);
    connect(submitBtn, &QPushButton::clicked, this, &StaffLeavesWidget::onSubmitLeaveClicked);

    // 表单布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("员工 ID", staffIdLabel);
    formLayout->addRow("姓名", staffNameLabel);
    formLayout->addRow("开始日期", startDateEdit);
    formLayout->addRow("结束日期", endDateEdit);
    formLayout->addRow("请假原因", reasonEdit);

    QVBoxLayout *formMainLayout = new QVBoxLayout();
    formMainLayout->addLayout(formLayout);
    formMainLayout->addWidget(submitBtn);

    // 将表单加入主布局（可根据需求调整位置，比如放在 TabWidget 上方等）
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->insertLayout(0, formMainLayout);
    }
    loadPendingData();
    loadApprovedData();
}

void StaffLeavesWidget::loadPendingData()
{
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    QSqlQuery query;
    // 查询待审批数据（approvalprogress = 0）
    query.prepare("SELECT staffid, staffname, start_date, end_date, leavereason, approvalprogress, approvalresult, approverid FROM leaves WHERE approvalprogress = 0 AND staffid = ?" );
    query.addBindValue(m_userId);
    query.exec();
    pendingTable->setRowCount(0);
    int row = 0;
    while (query.next()) {
        pendingTable->insertRow(row);
        for (int col = 0; col < 8; ++col) {
            QString content = query.value(col).toString();
            if (col == 5){
                if (content == "1") content = "已审批";
                else content = "未审批";
            }
            QTableWidgetItem *item = new QTableWidgetItem(content);
            pendingTable->setItem(row, col, item);
        }
        row++;
    }
    // 设置单元格居中
    for (int r = 0; r < pendingTable->rowCount(); ++r) {
        for (int c = 0; c < pendingTable->columnCount(); ++c) {
            if (QTableWidgetItem* item = pendingTable->item(r, c)) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    setFixedRowHeight(pendingTable, 60);
}

void StaffLeavesWidget::loadApprovedData()
{
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    QSqlQuery query;
    // 查询已审批数据（approvalprogress = 1）
    query.prepare("SELECT staffid, staffname, start_date, end_date, leavereason, approvalprogress, approvalresult, approverid, approver, approvaltime FROM leaves WHERE approvalprogress = 1 AND staffid = ?");
    query.addBindValue(m_userId);
    query.exec();
    approvedTable->setRowCount(0);
    int row = 0;
    while (query.next()) {
        approvedTable->insertRow(row);
        for (int col = 0; col < 10; ++col) {
            QString content =query.value(col).toString();
            if (col == 5) {
                if (content == "1") content = "已审批";
                else content = "未审批";
            }
            if (col == 6) {
                if (content == "1") content = "批准";
                else content = "驳回";
            }
            QTableWidgetItem *item = new QTableWidgetItem(content);

            approvedTable->setItem(row, col, item);
        }
        row++;
    }
    // 设置单元格居中
    for (int r = 0; r < approvedTable->rowCount(); ++r) {
        for (int c = 0; c < approvedTable->columnCount(); ++c) {
            if (QTableWidgetItem* item = approvedTable->item(r, c)) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    setFixedRowHeight(approvedTable, 60);
}

// 固定行高
void StaffLeavesWidget::setFixedRowHeight(QTableWidget *table, int height)
{
    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHeight(i, height);
}

void StaffLeavesWidget::onSubmitLeaveClicked()
{
    // 获取表单数据
    int staffId = m_userId; // 使用 m_userId 作为员工 ID
    QString staffName = staffNameLabel->text(); // 从标签获取员工姓名
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();
    QString reason = reasonEdit->toPlainText();

    // 简单校验（可扩展）
    if (staffId <= 0 || staffName.isEmpty() || startDate > endDate || reason.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请正确填写所有字段，开始日期不能晚于结束日期！");
        return;
    }

    // 插入数据库，默认 approvalprogress = 0（待审批）
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO leaves (staffid, staffname, start_date, end_date, leavereason, approvalprogress) "
                  "VALUES (:staffid, :staffname, :start_date, :end_date, :leavereason, 0)");
    query.bindValue(":staffid", staffId);
    query.bindValue(":staffname", staffName);
    query.bindValue(":start_date", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":end_date", endDate.toString("yyyy-MM-dd"));
    query.bindValue(":leavereason", reason);

    if (query.exec()) {
        QMessageBox::information(this, "提交成功", "请假申请已提交，等待审核！");
        // 提交后刷新待审批列表
        loadPendingData();
        // 清空表单
        startDateEdit->setDate(QDate::currentDate());
        endDateEdit->setDate(QDate::currentDate());
        reasonEdit->clear();
    } else {
        QMessageBox::critical(this, "提交失败", "数据库操作错误：" + query.lastError().text());
    }
}

void StaffLeavesWidget::onRefreshPendingClicked()
{
    loadPendingData();
}

void StaffLeavesWidget::onRefreshApprovedClicked()
{
    loadApprovedData();
}

void StaffLeavesWidget::loadUserInfo()
{
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    QSqlQuery query;
    query.prepare("SELECT id, name FROM users WHERE id = :id");
    query.bindValue(":id", m_userId);
    if (query.exec() && query.next()) {
        staffIdLabel->setText(query.value(0).toString());
        staffNameLabel->setText(query.value(1).toString());
    } else {
        QMessageBox::critical(this, "错误", "获取用户信息失败：" + query.lastError().text());
    }
}
