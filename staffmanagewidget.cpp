#include "staffmanagewidget.h"
#include "ui_staffmanagewidget.h"
#include "databasemanager.h"

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

StaffManageWidget::StaffManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StaffManageWidget)
{
    choose = 1;

    ui->setupUi(this);
    table = new QTableWidget(this);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"员工ID", "用户名", "姓名", "部门", "岗位", "状态" });
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true); // 斑马线
    table->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QHeaderView::section {  font-weight: bold; } "
                         "QTableWidget::item:selected { background: #2d8cf0;  }");

    btnEdit = new QPushButton("编辑", this);
    btnDelete = new QPushButton("删除", this);
    btnAdd = new QPushButton("新增",this);
    btnSearch = new QPushButton("查找", this);

    //搜索框
    Lineedit = new QLineEdit(this);
    Lineedit->setFixedHeight(30);

    //按钮美化
    QString btnStyle = "QPushButton { background-color: #2d8cf0; color: white; border-radius: 6px; font-family: 等线; font-weight: bold;}"
                       "QPushButton:hover { background-color: #1a73e8; }";
    btnAdd->setStyleSheet(btnStyle);
    btnAdd->setFixedSize(60,30);
    btnEdit->setStyleSheet(btnStyle.replace("#2d8cf0", "#19be6b").replace("#1a73e8", "#13a456"));
    btnEdit->setFixedSize(60,30);
    btnSearch->setStyleSheet(btnStyle);
    btnSearch->setFixedSize(60,30);
    btnDelete->setStyleSheet(btnStyle.replace("#19be6b", "#f56c6c").replace("#13a456", "#d93025"));
    btnDelete->setFixedSize(60,30);


    auto* btnLayout = new QHBoxLayout; //水平对齐

    btnLayout->addStretch();
    btnLayout->addWidget(Lineedit);
    btnLayout->addSpacing(1);
    btnLayout->addWidget(btnSearch);
    btnLayout->addSpacing(100);
    btnLayout->addWidget(btnAdd);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(btnEdit);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(btnDelete);
    btnLayout->addSpacing(10);
    btnLayout->addStretch();

    //查找选项
    radioId = new QRadioButton("查找id", this);
    radioName = new QRadioButton("查找姓名", this);
    radioStatus = new QRadioButton("查找状态", this);
    radioPosition = new QRadioButton("查找岗位", this);
    radioUsername = new QRadioButton("查找用户名", this);
    radioDepartment = new QRadioButton("查找部门", this);

    auto* radioLayout = new QHBoxLayout; //水平对齐
    radioLayout->addStretch();
    radioLayout->addWidget(radioId);
    radioLayout->addSpacing(10);
    radioLayout->addWidget(radioUsername);
    radioLayout->addSpacing(10);
    radioLayout->addWidget(radioName);
    radioLayout->addSpacing(10);
    radioLayout->addWidget(radioDepartment);
    radioLayout->addSpacing(10);
    radioLayout->addWidget(radioPosition);
    radioLayout->addSpacing(10);
    radioLayout->addWidget(radioStatus);
    radioLayout->addSpacing(10);
    radioLayout->addStretch();

    //创建按钮组
    searchGroup = new QButtonGroup(this);
    searchGroup->addButton(radioId, 1);
    searchGroup->addButton(radioUsername, 2);
    searchGroup->addButton(radioName, 3);
    searchGroup->addButton(radioDepartment, 4);
    searchGroup->addButton(radioPosition, 5);
    searchGroup->addButton(radioStatus, 6);

    //默认选中第一个
    radioId->setChecked(true);

    //标题
    QLabel* title = new QLabel ("汤臣二品物业工作人员信息管理系统", this);  //创建Label
    title->setAlignment (Qt::AlignCenter);  //设置居中对齐
//    radioLayout->setAlignment(Qt::AlignCenter);
    title->setStyleSheet ("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");
    auto* mainLayout = new QVBoxLayout(this);  //垂直布局
    mainLayout->addWidget(title);
    mainLayout->addLayout(btnLayout);
    mainLayout->addLayout(radioLayout);
    mainLayout->setAlignment(radioLayout, Qt::AlignHCenter);
    mainLayout->addWidget(table);

    //连接信号与槽
    connect(btnAdd, &QPushButton::clicked, this, &StaffManageWidget::onAdd);
    connect(btnEdit, &QPushButton::clicked, this, &StaffManageWidget::onEdit);
    connect(btnDelete, &QPushButton::clicked, this, &StaffManageWidget::onDelete);
    connect(btnSearch, &QPushButton::clicked, this, &StaffManageWidget::onSearch);
    connect(searchGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [&](int id) {
        choose = id;
    });

    refreshTable();

}

StaffManageWidget::~StaffManageWidget()
{
    delete ui;
}

//对应槽函数
void StaffManageWidget::onAdd(){
    bool ok;
    QString username = QInputDialog::getText(this, "新增工作人员", "用户名：", QLineEdit::Normal, "", &ok);
    if (!ok || username.isEmpty()) return;
    QString password = QInputDialog::getText(this, "新增工作人员", "密码：", QLineEdit::Normal, "", &ok);
    if (!ok || password.isEmpty()) return;
    QString name = QInputDialog::getText(this, "新增工作人员", "姓名：", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    QString department = QInputDialog::getText(this, "新增工作人员", "部门：", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    QString position = QInputDialog::getText(this, "新增工作人员", "岗位：", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    //打开数据库
    DatabaseManager db = DatabaseManager::getInstance();
    db.DatabaseManager::openDB(databasePath);

    //向数据库发送增加指令
    QSqlQuery q;
    q.prepare("INSERT INTO users (username, password, role, name, department, position, status)"
              "VALUES (?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(username);
    q.addBindValue(password);
    q.addBindValue(0);
    q.addBindValue(name);
    q.addBindValue(department);
    q.addBindValue(position);
    q.addBindValue(1);
    if (!q.exec()) {
        QSqlError error = q.lastError();
        qDebug() << "Insert Error:" << error.text();
        QMessageBox::warning(this, "失败", "添加账号失败，用户名可能已存在！");
        return;
    }
    else {
        QMessageBox::information(this,"提示", "添加成功！");
        refreshTable();
    }

}
void StaffManageWidget::onEdit(){
    int row = table->currentRow();
        if(row<0) return;
        // 获取ID（第1列）
        int id = table->item(row,0)->text().toInt();
        QString username = QInputDialog::getText(this, "编辑用户名", "用户名", QLineEdit::Normal, table->item(row,1)->text());
        QString name = QInputDialog::getText(this,"编辑姓名","姓名",QLineEdit::Normal,table->item(row,2)->text());
        QString department = QInputDialog::getText(this,"编辑部门","部门",QLineEdit::Normal,table->item(row,3)->text());
        QString position = QInputDialog::getText(this,"编辑岗位","岗位",QLineEdit::Normal,table->item(row,4)->text());
        QString status = QInputDialog::getText(this,"编辑状态","状态",QLineEdit::Normal,table->item(row,5)->text());
        int s = -1;
        if (status == "离职") s = 0;
        else if (status == "在职") s = 1;
        DatabaseManager db = DatabaseManager::getInstance();
        db.openDB(databasePath);
        QSqlQuery ql;
        //更新表中匹配 id 的记录，修改 username、name、department、position、status 字段
        ql.prepare("UPDATE users SET username = ?, name = ?, department = ?, position = ?,status = ? WHERE id = ?");
        ql.addBindValue(username);
        ql.addBindValue(name);
        ql.addBindValue(department);
        ql.addBindValue(position);
        ql.addBindValue(s);
        ql.addBindValue(id);
        ql.exec();
        refreshTable();
        table->setCurrentCell(row, 0); //把表格光标或选中状态切回当前行第 0 列（id 列），保持操作后的界面焦点。

        refreshTable();

}
void StaffManageWidget::onDelete(){
    int row = table->currentRow();
        if(row<0) return;
        int user_id = table->item(row,0)->text().toInt();
        if(QMessageBox::question(this,"确认","确定要删除该工作人员吗？")!=QMessageBox::Yes) return;

        DatabaseManager db;
        db.openDB(databasePath);
        QSqlQuery q2;
        q2.prepare("DELETE FROM users WHERE id=?");
        q2.addBindValue(user_id);
        q2.exec();
        refreshTable();
}
void StaffManageWidget::onSearch(){
    DatabaseManager db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query;
    QString edit = Lineedit->text().trimmed();
    if (!edit.isEmpty())
    {
        if (choose == 1) { //按ID搜索
            query.prepare("SELECT users.id, users.username, users.name, users.department, users.position, users.status "
                          "FROM users WHERE users.role=0 AND id = ?");
            query.addBindValue(edit);
            query.exec();
        }
        else if (choose == 2) { //按用户名搜索
            query.prepare("SELECT users.id, users.username, users.name, users.department, users.position, users.status "
                          "FROM users WHERE users.role=0 AND username = ?");
            query.addBindValue(edit);
            query.exec();
        }
        else if (choose == 3) { //按姓名搜索
            query.prepare("SELECT users.id, users.username, users.name, users.department, users.position, users.status "
                          "FROM users WHERE users.role=0 AND name = ?");
            query.addBindValue(edit);
            query.exec();
        }
        else if (choose == 4) { //按部门搜索
            query.prepare("SELECT users.id, users.username, users.name, users.department, users.position, users.status "
                          "FROM users WHERE users.role=0 AND department = ?");
            query.addBindValue(edit);
            query.exec();
        }
        else if (choose == 5) { //按岗位搜索
            query.prepare("SELECT users.id, users.username, users.name, users.department, users.position, users.status "
                          "FROM users WHERE users.role=0 AND position = ?");
            query.addBindValue(edit);
            query.exec();
        }
        else if (choose == 6) { //按状态搜索
            if(edit == "在职") edit = "1";
            else if(edit == "离职") edit = "0";
            query.prepare("SELECT users.id, users.username, users.name, users.department, users.position, users.status "
                          "FROM users WHERE users.role=0 AND status = ?");
            query.addBindValue(edit);
            query.exec();
        }

        //刷新表格
        table->setRowCount(0);
        int row = 0;
        while (query.next()) {
            table->insertRow(row);
            for (int col = 0; col < 6; ++col) {
                QString val = query.value(col).toString();
                if (col == 5) { // 状态
                    if (val == "1") val = "在职";
                    else if (val == "0") val = "离职";
                    else val = "未知";
                }
                table->setItem(row, col, new QTableWidgetItem(val));
            }
            ++row;
        }
        if (row == 0){
            QSqlError error = query.lastError();
            qDebug() << "Insert Error:" << error.text();
            QMessageBox::information(this, "提示", "未查询到结果!");
            return;
        }

        for (int row = 0; row < table->rowCount(); ++row) {
            for (int col = 0; col < table->columnCount(); ++col) {
                QTableWidgetItem *item = table->item(row, col);
                if (item) {
                    item->setTextAlignment(Qt::AlignCenter);
                }
            }
        }
        setFixedRowHeight(60);  // 设置行高为60像素

    }
    else {
        refreshTable();
    }


}

//连接数据库
void StaffManageWidget::refreshTable(){
    table->setRowCount(0);
    DatabaseManager db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query = db.query(
                "SELECT users.id, users.username, users.name, users.department, users.position, users.status "
                "FROM users WHERE users.role=0");
    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        for (int col = 0; col < 6; ++col) {
            QString val = query.value(col).toString();
            if (col == 5) { // 状态
                if (val == "1") val = "在职";
                else if (val == "0") val = "离职";
                else val = "未知";
            }
            table->setItem(row, col, new QTableWidgetItem(val));
        }
        ++row;
    }
    for (int row = 0; row < table->rowCount(); ++row) {
        for (int col = 0; col < table->columnCount(); ++col) {
            QTableWidgetItem *item = table->item(row, col);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    setFixedRowHeight(60);  // 设置行高为60像素

}

// 遍历所有行，设置行高
void StaffManageWidget::setFixedRowHeight(int height) {
    for (int row = 0; row < table->rowCount(); ++row) {
        table->setRowHeight(row, height);
    }
}
