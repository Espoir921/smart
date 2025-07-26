#include "attendancewidget.h"
#include "ui_attendancewidget.h"
#include "databasemanager.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStringBuilder>
#include <QSqlRecord>
#include <QStyledItemDelegate>

AttendanceWidget::AttendanceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttendanceWidget)
{
    ui->setupUi(this);

    // 设置字体和对齐方式
    ui->comboBoxday->setFont(QFont("SimHei", 10));
    label = ui->label;
    label->setStyleSheet ("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");

    // 重置委托
    ui->comboBoxday->setItemDelegate(new QStyledItemDelegate(this));
    // 初始化控件指针
    pushButtonadd = ui->pushButtonadd;
    pushButtonalter = ui->pushButtonalter;
    pushButtonSrch = ui->pushButtonSrch;
    comboBoxyear = ui->comboBoxyear;
    comboBoxmonth = ui->comboBoxmonth;
    comboBoxday = ui->comboBoxday;
    comboBoxtime = ui->comboBoxtime;
    tableWidgetdaily = ui->tableWidgetdaily;
    tableWidgetTab2 = ui->tableWidgetmonthly;


    // 初始化单选按钮组（Tab2）
    radioGroup = new QButtonGroup(this);
    radioGroup->addButton(ui->radioButtonId, 1);    // 按员工ID
    radioGroup->addButton(ui->radioButtonName, 2);  // 按姓名
    radioGroup->addButton(ui->radioButtonDpt, 3);   // 按部门
    radioGroup->addButton(ui->radioButtonPst, 4);   // 按岗位
    radioGroup->addButton(ui->radioButtonStu, 5);   // 按状态
    ui->radioButtonId->setChecked(true);

    // 初始化样式与控件
    setupButtonsStyle();
    initComboBoxes();
    setupTableStyle(tableWidgetdaily);
    setupTableStyle(tableWidgetTab2);
    tableWidgetTab2->setStyleSheet(
        "QTabBar::tab {"
        "    min-width: 120px;"    // 标签的最小宽度
        "    min-height: 50px;"    // 标签的最小高度
        "}"
    );
    tableWidgetdaily->setStyleSheet(
        "QTabBar::tab {"
        "    min-width: 120px;"    // 标签的最小宽度
        "    min-height: 50px;"    // 标签的最小高度
        "}"
    );



    // 连接信号槽
    connect(comboBoxyear, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AttendanceWidget::onYearChanged);
    connect(comboBoxmonth, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AttendanceWidget::onMonthChanged);
    connect(comboBoxday, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AttendanceWidget::onDateSelected);
    connect(comboBoxtime, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](){ if(onTotal()) refreshTableTab2(); });
    connect(pushButtonadd, &QPushButton::clicked, this, &AttendanceWidget::onAdd);
    connect(pushButtonalter, &QPushButton::clicked, this, &AttendanceWidget::onEdit);
    connect(pushButtonSrch, &QPushButton::clicked, this, &AttendanceWidget::onSearch);
}

AttendanceWidget::~AttendanceWidget()
{
    delete ui;
}

// 设置按钮样式
void AttendanceWidget::setupButtonsStyle()
{
    // 新增按钮（蓝色）
    QString addStyle = "QPushButton { background-color: #2d8cf0; color: white; border-radius: 6px; "
                       "font-family: 等线; font-weight: bold;}"
                       "QPushButton:hover { background-color: #1a73e8; }";
    pushButtonadd->setStyleSheet(addStyle);
    pushButtonadd->setFixedSize(60, 30);

    // 编辑按钮（绿色）
    QString editStyle = "QPushButton { background-color: #19be6b; color: white; border-radius: 6px; "
                        "font-family: 等线; font-weight: bold;}"
                        "QPushButton:hover { background-color: #13a456; }";
    pushButtonalter->setStyleSheet(editStyle);
    pushButtonalter->setFixedSize(60, 30);

    // 查找按钮（绿色）
    pushButtonSrch->setStyleSheet(editStyle);
    pushButtonSrch->setFixedSize(80, 30);
}

// 初始化表格样式
void AttendanceWidget::setupTableStyle(QTableWidget *table)
{
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QHeaderView::section { font-weight: bold; } "
                         "QTableWidget::item:selected { background: #2d8cf0; }");
    setFixedRowHeight(table, 60);

    // 修改TabWidget标签的高度为80
    ui->tabWidget->setStyleSheet(
        "QTabBar::tab {"
        "    min-width: 120px;"    // 标签的最小宽度
        "    min-height: 50px;"    // 标签的最小高度
        "}"
    );
}

// 固定行高
void AttendanceWidget::setFixedRowHeight(QTableWidget *table, int height)
{
    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHeight(i, height);
}

// 初始化下拉框
void AttendanceWidget::initComboBoxes()
{
    // 年份下拉框（2024-2027，默认空白）
    comboBoxyear->addItem("");
    for (int y = 2024; y <= 2027; ++y)
        comboBoxyear->addItem(QString::number(y));

    // 月份下拉框（默认空白）
    comboBoxmonth->addItem("");

    // 日期下拉框（默认空白）
    comboBoxday->addItem("");

    // 月度选择下拉框（2024-01至2027-12）
    comboBoxtime->addItem("");
    for (int y = 2024; y <= 2027; ++y)
        for (int m = 1; m <= 12; ++m)
            comboBoxtime->addItem(QString("%1-%2").arg(y).arg(m, 2, 10, QChar('0')));
}

// 年份变化时更新月份
void AttendanceWidget::onYearChanged(int index)
{
    comboBoxmonth->clear();
    comboBoxmonth->addItem("");
    comboBoxday->clear();
    comboBoxday->addItem("");

    if (index <= 0) return; // 未选择年份
    // 添加1-12月
    for (int m = 1; m <= 12; ++m)
    {
        QString monthStr = (m < 10) ? QString::number(m) : QString("%1").arg(m);
        comboBoxmonth->addItem(monthStr);
    }
    // 计算下拉框需要的高度
    int itemCount = comboBoxmonth->count();
    int requiredHeight = itemCount * 22 + 10; // 每个选项22px高度

    // 设置下拉框样式
    comboBoxmonth->setStyleSheet(
        QString("QComboBox QAbstractItemView { max-height: %1px; }"
                "QComboBox { combobox-popup: 0; }"
                "QComboBox::item { height: 20px; }").arg(requiredHeight)
    );

    comboBoxmonth->update();

}

// 月份变化时更新日期
void AttendanceWidget::onMonthChanged(int index)
{
    comboBoxday->clear();
    comboBoxday->addItem("");

    if (index <= 0) return; // 未选择月份

    int year = comboBoxyear->currentText().toInt();
    int month = comboBoxmonth->currentText().toInt();
    int days = getDaysInMonth(year, month);

    // 添加日期选项
    for (int d = 1; d <= days; ++d)
    {
        QString dayStr = (d < 10) ? QString::number(d) : QString("%1").arg(d);
        comboBoxday->addItem(dayStr);
    }

    // 计算下拉框需要的高度
    int itemCount = comboBoxday->count();
    int requiredHeight = itemCount * 22 + 10; // 每个选项22px高度

    // 设置下拉框样式
    comboBoxday->setStyleSheet(
        QString("QComboBox QAbstractItemView { max-height: %1px; }"
                "QComboBox { combobox-popup: 0; }"
                "QComboBox::item { height: 20px; }").arg(requiredHeight)
    );

    comboBoxday->update();
}

// 计算月份天数
int AttendanceWidget::getDaysInMonth(int year, int month)
{
    if (month == 2)
        return isLeapYear(year) ? 29 : 28;
    else if (month == 4 || month == 6 || month == 9 || month == 11)
        return 30;
    else
        return 31;
}

// 判断闰年
bool AttendanceWidget::isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 获取当前选择的日期字符串
QString AttendanceWidget::getCurrentDateStr()
{
    QString y = comboBoxyear->currentText();
    QString m = comboBoxmonth->currentText();
    QString d = comboBoxday->currentText();
    if (y.isEmpty() || m.isEmpty() || d.isEmpty()) return "";
    // 依次填充年、月、日占位符，月和日按两位补0
    return QString("%1-%2-%3").arg(y).arg(m, 2, QChar('0')).arg(d, 2, QChar('0'));
}

// 日期选择后刷新Tab1
void AttendanceWidget::onDateSelected()
{
    QString date = getCurrentDateStr();
//    qDebug() << "Selected date: " << date; // 添加日志输出
    if (!date.isEmpty()) refreshTableTab1();
}

//考勤结果结算
void AttendanceWidget::setupStatus(QString date){
    // 打开数据库
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    // 搜索员工信息
    QSqlQuery userQuery;
    userQuery.prepare("SELECT id, name, department, position FROM users WHERE role = 0");
    if (!userQuery.exec()) {
        QMessageBox::warning(this, "错误", "更新员工信息失败！");
        return;
    }
    // 检查并补充缺失记录
    while (userQuery.next()) {
        int staffid = userQuery.value(0).toInt();
        QString name = userQuery.value(1).toString();
        QString dept = userQuery.value(2).toString();
        QString pos = userQuery.value(3).toString();

        // 检查是否已存在记录
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM attendance WHERE date = ? AND staffid = ?");
        checkQuery.addBindValue(date);
        checkQuery.addBindValue(staffid);
        if (checkQuery.exec() && checkQuery.next()) {
            if (checkQuery.value(0).toInt() == 0) {
                // 插入新记录
                QSqlQuery insertQuery;
                insertQuery.prepare(
                        "INSERT INTO attendance (date, staffid, name, department, position, status, role) "
                        "VALUES (?, ?, ?, ?, ?, 0, 0)"
                    );
                insertQuery.addBindValue(date);
                insertQuery.addBindValue(staffid);
                insertQuery.addBindValue(name);
                insertQuery.addBindValue(dept);
                insertQuery.addBindValue(pos);
                if (!insertQuery.exec()) {
                    qDebug() << "插入失败:" << insertQuery.lastError().text();
                }
            }
        }
    }

}

// 新增考勤记录
void AttendanceWidget::onAdd()
{
    // 检查日期选择
    QString date = getCurrentDateStr();
    if (date.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择日期！");
        return;
    }

    //更新当日打卡
    setupStatus(date);

    // 查询员工信息
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    QSqlQuery query;
    query.prepare("SELECT id, staffid, name, department, position, status FROM attendance WHERE role = 0 AND date = ?");
    query.addBindValue(date);
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "查询员工信息失败！");
        return;
    }


    while (query.next()) {
        int id = query.value(0).toInt();
        int staffid = query.value(1).toInt();
        QString name = query.value(2).toString();
        QString dept = query.value(3).toString();
        QString pos = query.value(4).toString();
        int status = query.value(5).toInt();
        int result = 1; // 默认出勤
//       qDebug() << "姓名: " << name;

        // 计算出勤结果（参考leaves表）
        if (status == 0) {
            // 检查是否请假
            QSqlQuery leaveQuery;
            leaveQuery.prepare("SELECT start_date, end_date, approvalresult FROM leaves WHERE staffid = ?");
//            qDebug()<<"id:"<<staffid;
            leaveQuery.addBindValue(staffid);
            if (leaveQuery.exec()) {
                QDate current = QDate::fromString(date, "yyyy-MM-dd");
                while (leaveQuery.next()) {
                    QDate start = QDate::fromString(leaveQuery.value(0).toString(), "yyyy-MM-dd");
                    QDate end = QDate::fromString(leaveQuery.value(1).toString(), "yyyy-MM-dd");
//                    qDebug()<<"start_date:"<<start;
//                    qDebug()<<"end_date:"<<end;
                    int out = leaveQuery.value(2).toInt();
                    if (current >= start && current <= end && out == 1) {
                        result = 2; // 请假
                        break;
                    }
                }
                if (result != 2) result = 0; // 旷工
            }
        }

        // 更新数据库
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE attendance SET result = ? WHERE id = ? AND staffid = ?");
        updateQuery.addBindValue(result);
        updateQuery.addBindValue(id);
        updateQuery.addBindValue(staffid);
        if (!updateQuery.exec()) {
            QMessageBox::warning(this, "错误", "更新考勤记录失败！");
            return;
        }
    }


    QMessageBox::information(this, "成功", "考勤记录添加成功！");
    refreshTableTab1();
}



// 编辑考勤状态（仅修改result字段）
void AttendanceWidget::onEdit()
{
    int row = tableWidgetdaily->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请选择一行！");
        return;
    }
    int id = tableWidgetdaily->item(row, 0)->text().toInt(); // 考勤单编号

    // 输入新状态（0=旷工，1=出勤，2=请假）
    bool ok;
    int newResult = QInputDialog::getInt(this, "编辑状态", "状态（0=旷工，1=出勤，2=请假）：", 0, 0, 2, 1, &ok);
    if (!ok) return;

    // 更新数据库
    DatabaseManager& db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query;
    query.prepare("UPDATE attendance SET result = ? WHERE id = ?");
    query.addBindValue(newResult);
    query.addBindValue(id);
    if (query.exec()) {
        QMessageBox::information(this, "成功", "编辑成功！");
        refreshTableTab1();
    } else {
        qDebug() << "更新失败：" << query.lastError().text();
        QMessageBox::warning(this, "失败", "编辑失败！");
    }
}

// 搜索考勤记录
void AttendanceWidget::onSearch()
{
    QString key = ui->lineEdit->text().trimmed();
    if (key.isEmpty()) {
        refreshTableTab2();
        return;
    }

    // 构建查询条件
    QString sql;
    switch (radioGroup->checkedId()) {
    case 1:  // 按员工ID
        sql = "SELECT * FROM attendances WHERE staffid = ?";
        break;
    case 2:  // 按姓名
        sql = "SELECT * FROM attendances WHERE name = ?";
        break;
    case 3:  // 按部门
        sql = "SELECT * FROM attendances WHERE department = ?";
        break;
    case 4:  // 按岗位
        sql = "SELECT * FROM attendances WHERE position = ?";
        break;
    case 5:  // 按状态
        int res;
        if (key == "全勤") res = 1;
        else if (key == "非全勤") res = 0;
        else {
            QMessageBox::warning(this, "错误", "请输入：全勤/非全勤");
            return;
        }
        key = QString::number(res);
        sql = "SELECT * FROM attendances WHERE status = ?";
        break;
    }

    // 执行查询
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    QSqlQuery query;
    query.prepare(sql);
//    qDebug()<<"?"<<sql<<"  ?"<<key;
    query.addBindValue(key);
    if (!query.exec()) {
        qDebug() << "查询失败：" << query.lastError().text();
        QMessageBox::warning(this, "错误", "查询考勤记录失败！");
        return;
    }
//    qDebug()<<"try";

    // 填充表格
    tableWidgetTab2->setRowCount(0);
    int row = 0;
    if (!query.first()) {
//        qDebug() << "查询结果为空";
        QMessageBox::information(this, "提示", "未找到匹配的记录");
        return;
    }
    query.previous();
    int cnt = 0;

    while (query.next()) {
        ++cnt;
//        qDebug()<<"正在插入表格行数"<<cnt;
        tableWidgetTab2->insertRow(row);
        int fieldCount = query.record().count();
//        qDebug()<<"there?";
        for (int col = 0; col < fieldCount; ++col) { // 修改此处，确保不越界
//            qDebug()<<"正在插入表格列数"<<col;
            if (col == 6) { // 转换status为文字
                if (!query.value(col).isNull()) { // 处理非空值
                    QString val = query.value(col).toString();
//                    qDebug()<<"a";
                    val = (val == "1" ? "是" : "否");
                    tableWidgetTab2->setItem(row, col, new QTableWidgetItem(val));
                }
            } else {
                if (!query.value(col).isNull()) { // 处理空值
                    QString val = query.value(col).toString();
                    tableWidgetTab2->setItem(row, col, new QTableWidgetItem(val));
                }
            }
            if (tableWidgetTab2->item(row, col)) { // 确保item不为空
                tableWidgetTab2->item(row, col)->setTextAlignment(Qt::AlignCenter);
            }
        }
        row++;
    }
//    qDebug()<<"hao";
    setFixedRowHeight(tableWidgetTab2, 60);
}

// 汇总月度数据（Tab2核心函数）
bool AttendanceWidget::onTotal()
{
    QString month = comboBoxtime->currentText();
    if (month.isEmpty()) {
        QMessageBox::warning(this,"失败","数据未更新！");
        return false;
    }
    // 解析年月
    QStringList parts = month.split('-');
    int year = parts[0].toInt();
    int mon = parts[1].toInt();
    QDate lastDay(year, mon, getDaysInMonth(year, mon));
//   qDebug()<<"year:"<<year<<"  mon:"<<mon;

    // 判断月份是否结束
    if (lastDay > QDate::currentDate()) return false;

    // 检查是否已汇总
    DatabaseManager& db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query;
    query.prepare("SELECT * FROM attendances WHERE month = ?");
    query.addBindValue(month);
    if (query.exec() && query.next()) return true;
//    qDebug()<<"this is a test.";

    // 从attendance表汇总数据
    query.prepare("SELECT staffid, name, department, position, "
                  "SUM(CASE WHEN result=1 THEN 1 ELSE 0 END) AS attend, "
                  "SUM(CASE WHEN result=0 THEN 1 ELSE 0 END) AS absent, "
                  "SUM(CASE WHEN result=2 THEN 1 ELSE 0 END) AS leave "
                  "FROM attendance WHERE strftime('%Y-%m', date) = ? GROUP BY staffid");
    query.addBindValue(month);
    query.exec();

    while (query.next()) {
//        qDebug()<<"bug";
        int staffid = query.value(0).toInt();
//        qDebug()<<"staffid is:"<<staffid;
        QString name = query.value(1).toString();
        QString dept = query.value(2).toString();
        QString pos = query.value(3).toString();
        int attend = query.value(4).toInt();
        int absent = query.value(5).toInt();
        int leave = query.value(6).toInt();
//        qDebug()<<"leavenumber:"<<leave;
        int full = (attend == getDaysInMonth(year, mon)) ? 1 : 0;

        QSqlQuery insert;
        insert.prepare("INSERT INTO attendances (staffid, staffname, department, position, "
                      "month, status, leavenumber, absentnumber) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        insert.addBindValue(staffid);
        insert.addBindValue(name);
        insert.addBindValue(dept);
        insert.addBindValue(pos);
        insert.addBindValue(month);
        insert.addBindValue(full);
        insert.addBindValue(leave);
        insert.addBindValue(absent);
        insert.exec();
    }
    return true;
}

// 刷新Tab1表格（按日期查询）
void AttendanceWidget::refreshTableTab1()
{
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }

    QString date = getCurrentDateStr();
    QSqlQuery query;
    query.prepare("SELECT id, staffid, name, department, position, result FROM attendance WHERE role = 0 AND date = ?");
    query.addBindValue(date);
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "查询考勤记录失败！");
        return;
    }

    tableWidgetdaily->setRowCount(0);
    int row = 0;
    while (query.next()) {
        tableWidgetdaily->insertRow(row);
        for (int col = 0; col < 6; ++col) {
            QString val = query.value(col).toString();
            if (col == 5) {
                if (val == "0") val = "缺勤";
                else if (val == "1") val = "出勤";
                else if (val == "2") val = "请假";
                else val = "";
            }
            tableWidgetdaily->setItem(row, col, new QTableWidgetItem(val));
        }
        ++row;
    }

    // 设置单元格居中
    for (int r = 0; r < tableWidgetdaily->rowCount(); ++r) {
        for (int c = 0; c < tableWidgetdaily->columnCount(); ++c) {
            if (QTableWidgetItem* item = tableWidgetdaily->item(r, c)) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    setFixedRowHeight(tableWidgetdaily, 60);
}

// 刷新Tab2表格
void AttendanceWidget::refreshTableTab2()
{
    QString timeText = comboBoxtime->currentText();
    if (timeText.isEmpty()) {
        tableWidgetTab2->setRowCount(0);
        return;
    }

    // 从数据库查询对应时间的数据
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }

    QSqlQuery query;
    // 假设查询语句根据时间查询考勤数据
    query.prepare("SELECT * FROM attendances WHERE month = ?");

    query.addBindValue(timeText);
    if (!query.exec()) {
        qDebug() << "查询失败：" << query.lastError().text();
        return;
    }

    // 填充表格
    tableWidgetTab2->setRowCount(0);
    int row = 0;
    while (query.next()) {
        tableWidgetTab2->insertRow(row);
        // 按attendances表字段顺序填充
        for (int col = 0; col < query.record().count(); ++col) {
            if (query.value(col).isNull()) {
                tableWidgetTab2->setItem(row, col, new QTableWidgetItem(""));
            } else {
                QString val = query.value(col).toString();
                if (col == 6) {
                    val = (val == "1" ? "是" : "否");
                }
                tableWidgetTab2->setItem(row, col, new QTableWidgetItem(val));
            }
        }
        ++row;
    }

    // 设置单元格居中
    for (int r = 0; r < tableWidgetTab2->rowCount(); ++r) {
        for (int c = 0; c < tableWidgetTab2->columnCount(); ++c) {
            if (QTableWidgetItem* item = tableWidgetTab2->item(r, c)) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    setFixedRowHeight(tableWidgetTab2, 60);

    // 确保表格可见
    tableWidgetTab2->show();
}
