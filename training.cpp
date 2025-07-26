#include "training.h"
#include "ui_training.h"
#include "databasemanager.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QHeaderView>
#include <QAbstractItemView>

Training::Training(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Training)
{
    ui->setupUi(this);


    label = ui->label;
    label->setStyleSheet ("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");
    // 初始化表格
    table = ui->tableWidget;
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true); // 斑马线
    table->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QHeaderView::section {  font-weight: bold; } "
                         "QTableWidget::item:selected { background: #2d8cf0;  }");
    // 初始化按钮
    btnAdd = ui->ButtonNew;
    btnEdit = ui->ButtonEdit;
    btnDelete = ui->ButtonDelete;
    btnSearch = ui->ButtonSearch;
    // 初始化搜索控件
    lineEditSearch = ui->lineEdit;
    radioStaffId = ui->radioid;
    radioStaffName = ui->radioname;
    radioEvalution = ui->radioevalution;
    searchGroup = new QButtonGroup(this);
    searchGroup->addButton(radioStaffId, 1);
    searchGroup->addButton(radioStaffName, 2);
    searchGroup->addButton(radioEvalution, 3);
    radioStaffId->setChecked(true);  // 默认按staffid搜索

    // 设置样式
    setupButtonsStyle();

    // 连接信号槽
    connect(btnAdd, &QPushButton::clicked, this, &Training::onAdd);
    connect(btnEdit, &QPushButton::clicked, this, &Training::onEdit);
    connect(btnDelete, &QPushButton::clicked, this, &Training::onDelete);
    connect(btnSearch, &QPushButton::clicked, this, &Training::onSearch);

    // 初始刷新表格
    refreshTable();
}

Training::~Training()
{
    delete ui;
}

// 设置按钮样式
void Training::setupButtonsStyle()
{
    // 添加按钮样式（蓝色）
    QString addStyle = "QPushButton { background-color: #2d8cf0; color: white; border-radius: 6px; font-family: 等线; font-weight: bold;}"
                       "QPushButton:hover { background-color: #1a73e8; }";
    // 编辑按钮样式（绿色）
    QString editStyle = "QPushButton { background-color: #19be6b; color: white; border-radius: 6px; font-family: 等线; font-weight: bold;}"
                        "QPushButton:hover { background-color: #13a456; }";
    // 删除按钮样式（红色）
    QString deleteStyle = "QPushButton { background-color: #f56c6c; color: white; border-radius: 6px; font-family: 等线; font-weight: bold;}"
                          "QPushButton:hover { background-color: #d93025; }";
    // 搜索按钮样式
    QString searchStyle = "QPushButton { background-color: #19be6b; color: white; border-radius: 6px; font-family: 等线; font-weight: bold;}"
                          "QPushButton:hover { background-color: #13a456; }";

    btnAdd->setStyleSheet(addStyle);
    btnAdd->setFixedSize(60, 30);
    btnEdit->setStyleSheet(editStyle);
    btnEdit->setFixedSize(60, 30);
    btnDelete->setStyleSheet(deleteStyle);
    btnDelete->setFixedSize(60, 30);
    btnSearch->setStyleSheet(searchStyle);
    btnSearch->setFixedSize(80, 30);
}

// 设置表格固定行高
void Training::setFixedRowHeight(int height)
{
    for (int row = 0; row < table->rowCount(); ++row) {
        table->setRowHeight(row, height);
    }
}

// 刷新表格
void Training::refreshTable()
{
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }

    // 查询trains表所有数据
    QSqlQuery query("SELECT * FROM trains");
    if (!query.exec()) {
        qDebug() << "培训记录查询失败：" << query.lastError().text();
        return;
    }

    // 填充表格
    table->setRowCount(0);
    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        for (int col = 0; col < 7; ++col) {
            QString val = query.value(col).toString();
            if (col == 6) {
                switch (val.toInt()) {
                case 1: val = "A"; break;
                case 2: val = "B"; break;
                case 3: val = "C"; break;
                case 4: val = "D"; break;
                default: val = "未知";
                }
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

    // 设置行高
    setFixedRowHeight(60);
}

// 增加培训记录
void Training::onAdd()
{
    bool ok;

    // 1. 输入员工ID
    int staffid = QInputDialog::getInt(this, "输入员工ID", "请输入员工ID：", 0, 0, 10000, 1, &ok);
    if (!ok) return;

    // 2. 输入培训开始时间
    QString startDate = QInputDialog::getText(this, "输入开始时间", "请输入培训开始时间（格式：yyyy-MM-dd）：", QLineEdit::Normal, "", &ok);
    if (!ok || startDate.isEmpty()) return;

    // 3. 输入培训结束时间
    QString endDate = QInputDialog::getText(this, "输入结束时间", "请输入培训结束时间（格式：yyyy-MM-dd）：", QLineEdit::Normal, "", &ok);
    if (!ok || endDate.isEmpty()) return;

    // 新增：4. 输入培训地点
    QString location = QInputDialog::getText(this, "输入培训地点", "请输入培训地点：", QLineEdit::Normal, "", &ok);
    if (!ok || location.isEmpty()) return;

    // 5. 输入培训评价（1-4对应A-D）
    int evalution = QInputDialog::getInt(this, "输入培训评价", "请输入培训评价（1=A, 2=B, 3=C, 4=D）：", 1, 1, 4, 1, &ok);
    if (!ok) return;

    // 6. 从users表获取员工姓名
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    QSqlQuery userQuery;
    userQuery.prepare("SELECT name FROM users WHERE id = ?");
    userQuery.addBindValue(staffid);
    if (!userQuery.exec() || !userQuery.next()) {
        QMessageBox::warning(this, "错误", "员工ID不存在！");
        return;
    }
    QString staffname = userQuery.value("name").toString();

    QSqlQuery insertQuery;
    insertQuery.prepare(
        "INSERT INTO trains ("
        "staffid, staffname, start_trainingdate, end_trainingdate, traininglocation, evalution"
        ") VALUES (?, ?, ?, ?, ?, ?)"
    );
    insertQuery.addBindValue(staffid);
    insertQuery.addBindValue(staffname);
    insertQuery.addBindValue(startDate);
    insertQuery.addBindValue(endDate);
    insertQuery.addBindValue(location);
    insertQuery.addBindValue(evalution);

    if (insertQuery.exec()) {
        QMessageBox::information(this, "成功", "培训记录添加成功！");
        refreshTable();
    } else {
        qDebug() << "添加失败：" << insertQuery.lastError().text();
        QMessageBox::warning(this, "失败", "培训记录添加失败！");
    }
}

// 编辑培训记录
void Training::onEdit()
{
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请选择一行进行编辑！");
        return;
    }

    // 获取当前培训记录ID（第0列）
    int trainingid = table->item(row, 0)->text().toInt();
    // 获取当前培训地点（第5列）
    QString currentLocation = table->item(row, 5)->text();

    bool ok;
    // 1. 输入新的开始时间
    QString newStartDate = QInputDialog::getText(this, "编辑开始时间", "请输入新的培训开始时间（格式：yyyy-MM-dd）：", QLineEdit::Normal, "", &ok);
    if (!ok || newStartDate.isEmpty()) return;

    // 2. 输入新的结束时间
    QString newEndDate = QInputDialog::getText(this, "编辑结束时间", "请输入新的培训结束时间（格式：yyyy-MM-dd）：", QLineEdit::Normal, "", &ok);
    if (!ok || newEndDate.isEmpty()) return;

    // 新增：3. 输入新的培训地点
    QString newLocation = QInputDialog::getText(this, "编辑培训地点", "请输入新的培训地点：", QLineEdit::Normal, currentLocation, &ok);
    if (!ok) return;

    // 4. 输入新的评级
    int newEvalution = QInputDialog::getInt(this, "编辑培训评价", "请输入新的培训评价（1=A, 2=B, 3=C, 4=D）：", 1, 1, 4, 1, &ok);
    if (!ok) return;

    // 更新数据库
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    QSqlQuery query;
    query.prepare("UPDATE trains SET start_trainingdate = ?, end_trainingdate = ?, traininglocation = ?, evalution = ? WHERE trainingid = ?");
    query.addBindValue(newStartDate);
    query.addBindValue(newEndDate);
    query.addBindValue(newLocation);  // 添加培训地点更新
    query.addBindValue(newEvalution);
    query.addBindValue(trainingid);

    if (query.exec()) {
        QMessageBox::information(this, "成功", "培训记录编辑成功！");
        refreshTable();
    } else {
        qDebug() << "编辑失败：" << query.lastError().text();
        QMessageBox::warning(this, "失败", "培训记录编辑失败！");
    }
}

// 删除培训记录
void Training::onDelete()
{
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请选择一行进行删除！");
        return;
    }

    // 获取培训记录ID（第0列）
    int trainingid = table->item(row, 0)->text().toInt();

    if (QMessageBox::question(this, "确认", "确定要删除该培训记录吗？") != QMessageBox::Yes) {
        return;
    }

    // 删除数据库记录
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM trains WHERE trainingid = ?");
    query.addBindValue(trainingid);

    if (query.exec()) {
        QMessageBox::information(this, "成功", "培训记录删除成功！");
        refreshTable();
    } else {
        qDebug() << "删除失败：" << query.lastError().text();
        QMessageBox::warning(this, "失败", "培训记录删除失败！");
    }
}

// 搜索培训记录
void Training::onSearch()
{
    QString searchText = lineEditSearch->text().trimmed();
    if (searchText.isEmpty()) {
        refreshTable();  // 空文本时刷新全部
        return;
    }

    // 根据radioButton选择构建查询条件
    int choose = searchGroup->checkedId();
    QString queryStr;
    switch (choose) {
    case 1:  // 按staffid搜索
        queryStr = "SELECT * FROM trains WHERE staffid = ?";
        break;
    case 2:  // 按staffname搜索
        queryStr = "SELECT * FROM trains WHERE staffname = ?";
        break;
    case 3:  // 按evalution搜索（转换为数字，如"A"->1）
        int eval;
        if (searchText == "A") eval = 1;
        else if (searchText == "B") eval = 2;
        else if (searchText == "C") eval = 3;
        else if (searchText == "D") eval = 4;
        else {
            QMessageBox::warning(this, "警告", "评价搜索格式错误（请输入A/B/C/D）！");
            return;
        }
        searchText = QString::number(eval);
        queryStr = "SELECT * FROM trains WHERE evalution = ?";
        break;
    default:
        return;
    }

    // 执行查询
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    QSqlQuery query;
    query.prepare(queryStr);
    query.addBindValue(searchText);
    if (!query.exec()) {
        qDebug() << "搜索失败：" << query.lastError().text();
        return;
    }

    // 填充表格
    table->setRowCount(0);
    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        for (int col = 0; col < 7; ++col) {  // 列数从8改为7
            QString val = query.value(col).toString();
            // 处理评价显示（同refreshTable）
            if (col == 6) {  // 评价列从7改为6
                switch (val.toInt()) {
                case 1: val = "A"; break;
                case 2: val = "B"; break;
                case 3: val = "C"; break;
                case 4: val = "D"; break;
                default: val = "未知";
                }
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
    if (row == 0) {
        QMessageBox::information(this, "提示", "未找到符合条件的记录！");
    }
}
