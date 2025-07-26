#include "staffscheduling.h"
#include "ui_staffscheduling.h"
#include "databasemanager.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

StaffScheduling::StaffScheduling(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StaffScheduling),
    m_userId(-1) // 初始化用户ID为无效值
{
    ui->setupUi(this);

    label = ui->label_2;
    label->setStyleSheet ("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");

    // 初始化表格
    table = ui->tableWidget;
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QHeaderView::section { font-weight: bold; } "
                         "QTableWidget::item:selected { background: #2d8cf0; }");

    // 初始化按钮和下拉框
    btnAdd = ui->ButtonNew;
    btnEdit = ui->ButtonAlter;
    btnDelete = ui->ButtonDelete;
    comboBox = ui->comboBox;
    setupButtonsStyle();
    setupComboBox();

    // 连接信号槽
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StaffScheduling::onComboBoxChanged);
    connect(btnAdd, &QPushButton::clicked, this, &StaffScheduling::onAdd);
    connect(btnEdit, &QPushButton::clicked, this, &StaffScheduling::onEdit);
    connect(btnDelete, &QPushButton::clicked, this, &StaffScheduling::onDelete);
}

StaffScheduling::~StaffScheduling()
{
    delete ui;
}

// 设置当前用户ID（用于排班人信息）
void StaffScheduling::setUserId(const int userId)
{
    m_userId = userId;
}

// 按钮样式设置
void StaffScheduling::setupButtonsStyle()
{
    QString btnStyle = "QPushButton { background-color: #2d8cf0; color: white; border-radius: 6px; font-family: 等线; font-weight: bold;}"
                       "QPushButton:hover { background-color: #1a73e8; }";
    btnAdd->setStyleSheet(btnStyle);
    btnAdd->setFixedSize(60,30);
    btnEdit->setStyleSheet(btnStyle.replace("#2d8cf0", "#19be6b").replace("#1a73e8", "#13a456"));
    btnEdit->setFixedSize(60,30);
    btnDelete->setStyleSheet(btnStyle.replace("#19be6b", "#f56c6c").replace("#13a456", "#f56c6c"));
    btnDelete->setFixedSize(60,30);
}

// 初始化时间下拉框（2025-2028年季度）
void StaffScheduling::setupComboBox()
{
    comboBox->addItem(""); // 初始空白
    for (int year = 2025; year <= 2028; ++year) {
        for (int quarter = 1; quarter <= 4; ++quarter) {
            comboBox->addItem(QString("%1年第%2季度").arg(year).arg(quarter));
        }
    }
}

// 下拉框变化时刷新表格
void StaffScheduling::onComboBoxChanged()
{
    refreshTable();
}

// 刷新表格（根据时间查询排班数据）
void StaffScheduling::refreshTable()
{
    QString timeText = comboBox->currentText();
    if (timeText.isEmpty()) {
        table->setRowCount(0);
        return;
    }

    // 从shifts表查询对应时间的数据
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM shifts WHERE time = ?");
    query.addBindValue(timeText);
    if (!query.exec()) {
        qDebug() << "查询失败：" << query.lastError().text();
        return;
    }

    // 填充表格
    table->setRowCount(0);
    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        // 按shifts表字段顺序填充（shiftid, staffid, staffname, department, position, time, shift, phonenumber, approverid, approver）
        for (int col = 0; col < 10; ++col) {
            QString val = query.value(col).toString();
            // 第6列（索引6）是shift：0=夜班，1=早班
            if (col == 6) {
                if (val == "0") val = "早班";
                else if (val == "1") val = "中班";
                else val = "夜班";
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
    setFixedRowHeight(60);
}

// 新增排班
void StaffScheduling::onAdd()
{

    bool ok;
    // 1. 输入staffid（员工ID）
    int staffid = QInputDialog::getInt(this, "输入员工ID", "请输入员工ID：", 0, 0, 10000, 1, &ok);
    if (!ok) return;

    // 2. 获取时间（从下拉框）
    QString timeText = comboBox->currentText();
    if (timeText.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择时间！");
        return;
    }

    // 3. 输入shift
    int shift = QInputDialog::getInt(this, "输入排班类型", "请输入排班类型（早班为0，中班为1，夜班为2）：", 0, 0, 2, 1, &ok);
    if (!ok) return;

    // 4. 从users表查询员工信息（根据staffid）
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }

    QSqlQuery userQuery;
    userQuery.prepare("SELECT name, department, position, phonenumber FROM users WHERE id = ?");
    userQuery.addBindValue(staffid);
    if (!userQuery.exec() || !userQuery.next()) {
        QMessageBox::warning(this, "错误", "员工ID不存在！");
        return;
    }
    // 提取员工信息
    QString staffname = userQuery.value("name").toString();
    QString department = userQuery.value("department").toString();
    QString position = userQuery.value("position").toString();
    QString phonenumber = userQuery.value("phonenumber").toString();

    // 5. 获取排班人信息（approverid和approver，当前登录用户）
    QSqlQuery approverQuery;
    approverQuery.prepare("SELECT username FROM users WHERE id = ?");
    approverQuery.addBindValue(m_userId);
    if (!approverQuery.exec() || !approverQuery.next()) {
        QMessageBox::warning(this, "错误", "排班人信息获取失败！");
        return;
    }
    QString approver = approverQuery.value("username").toString();
    int approverid = m_userId;

    // 6. 插入shifts表
    QSqlQuery insertQuery;
    insertQuery.prepare(
        "INSERT INTO shifts ("
        "staffid, staffname, department, position, time, shift, "
        "phonenumber, approverid, approver"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );
    insertQuery.addBindValue(staffid);
    insertQuery.addBindValue(staffname);
    insertQuery.addBindValue(department);
    insertQuery.addBindValue(position);
    insertQuery.addBindValue(timeText);
    insertQuery.addBindValue(shift);
    insertQuery.addBindValue(phonenumber);
    insertQuery.addBindValue(approverid);
    insertQuery.addBindValue(approver);

    if (insertQuery.exec()) {
        QMessageBox::information(this, "成功", "排班添加成功！");
        refreshTable();
    } else {
        qDebug() << "插入失败：" << insertQuery.lastError().text();
        QMessageBox::warning(this, "失败", "排班添加失败！");
    }
}

// 编辑排班（仅编辑time和shift，onEdit()）
void StaffScheduling::onEdit()
{
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请选择一行进行编辑！");
        return;
    }

    // 获取当前排班ID（第0列）
    int shiftid = table->item(row, 0)->text().toInt();

    // 1. 编辑time（从下拉框选择新时间）
    QString newTime = comboBox->currentText();
    if (newTime.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择时间！");
        return;
    }

    // 2. 编辑shift（新排班类型）
    bool ok;
    int newShift = QInputDialog::getInt(this, "编辑排班类型", "请输入新排班类型（早班为0，中班为1，夜班为2）：", 0, 0, 2, 1, &ok); //默认值，最小值，最大值，步长
    if (!ok) return;

    // 更新数据库
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }

    QSqlQuery query;
    query.prepare("UPDATE shifts SET time = ?, shift = ? WHERE shiftid = ?");
    query.addBindValue(newTime);
    query.addBindValue(newShift);
    query.addBindValue(shiftid);

    if (query.exec()) {
        QMessageBox::information(this, "成功", "排班编辑成功！");
        refreshTable();
    } else {
        qDebug() << "更新失败：" << query.lastError().text();
        QMessageBox::warning(this, "失败", "排班编辑失败！");
    }
}

// 删除排班
void StaffScheduling::onDelete()
{
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请选择一行进行删除！");
        return;
    }

    // 获取排班ID（第0列）
    int shiftid = table->item(row, 0)->text().toInt();

    if (QMessageBox::question(this, "确认", "确定要删除该排班吗？") != QMessageBox::Yes) {
        return;
    }

    // 删除数据库记录
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM shifts WHERE shiftid = ?");
    query.addBindValue(shiftid);

    if (query.exec()) {
        QMessageBox::information(this, "成功", "排班删除成功！");
        refreshTable();
    } else {
        qDebug() << "删除失败：" << query.lastError().text();
        QMessageBox::warning(this, "失败", "排班删除失败！");
    }
}

void StaffScheduling::setFixedRowHeight(int height){
    for (int row = 0; row < table->rowCount(); ++row) {
        table->setRowHeight(row, height);
    }
}
