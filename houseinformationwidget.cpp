#include "houseinformationwidget.h"
#include "ui_houseinformationwidget.h"

#include "databasemanager.h"
#include <QLabel>
#include <QStyle>
#include <QApplication>
#include <QSpacerItem>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QSqlQuery>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QTableWidgetItem>

HouseInformationWidget::HouseInformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HouseInformationWidget)
{
    // 1. 创建标题
    QLabel* title = new QLabel("汤臣二品房屋信息管理", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");

    // 2. 创建搜索框和按钮
    lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText("请输入房号，如：12号楼-801");
    lineEdit->setFixedHeight(30);

    btnSearch = new QPushButton("搜索", this);
    btnAdd = new QPushButton("新增", this);
    btnEdit = new QPushButton("编辑", this);
    btnDelete = new QPushButton("删除", this);

    QString btnStyle = "QPushButton { background-color: #2d8cf0; color: white; border-radius: 6px; font-family: 等线; font-weight: bold;}"
                       "QPushButton:hover { background-color: #1a73e8; }";
    btnAdd->setStyleSheet(btnStyle);
    btnAdd->setFixedSize(60, 30);
    btnEdit->setStyleSheet(btnStyle.replace("#2d8cf0", "#19be6b").replace("#1a73e8", "#13a456"));
    btnEdit->setFixedSize(60, 30);
    btnSearch->setStyleSheet(btnStyle);
    btnSearch->setFixedSize(60, 30);
    btnDelete->setStyleSheet(btnStyle.replace("#19be6b", "#f56c6c").replace("#13a456", "#d93025"));
    btnDelete->setFixedSize(60, 30);

    auto* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(lineEdit);
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

    // 3. 创建单选按钮
    radioAll = new QRadioButton("查看全部",this);
    radioEmpty = new QRadioButton("仅看空闲房屋", this);
    radioOccupied = new QRadioButton("仅看已售房屋", this);
    radioGroup = new QButtonGroup(this);
    radioGroup->addButton(radioEmpty, 0);
    radioGroup->addButton(radioOccupied, 1);
    radioGroup->addButton(radioAll, 2);

    auto* radioLayout = new QHBoxLayout;
    radioLayout->addStretch();
    radioLayout->addWidget(radioAll);
    radioLayout->addSpacing(30);
    radioLayout->addWidget(radioEmpty);
    radioLayout->addSpacing(30);
    radioLayout->addWidget(radioOccupied);
    radioLayout->addStretch();

    // 4. 创建表格
    table = new QTableWidget(this);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"房号", "业主ID", "业主姓名", "联系电话"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QHeaderView::section {  font-weight: bold; } "
                         "QTableWidget::item:selected { background: #2d8cf0;  }");

    // 5. 设置主布局
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(title);
    mainLayout->addLayout(btnLayout);
    mainLayout->addLayout(radioLayout);
    mainLayout->addWidget(table);

    // 6. 连接信号与槽
    connect(btnSearch, &QPushButton::clicked, this, &HouseInformationWidget::onSearch);
    connect(btnAdd, &QPushButton::clicked, this, &HouseInformationWidget::onAdd);
    connect(btnEdit, &QPushButton::clicked, this, &HouseInformationWidget::onEdit);
    connect(btnDelete, &QPushButton::clicked, this, &HouseInformationWidget::onDelete);
    connect(radioGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &HouseInformationWidget::onRadioButtonChanged);

    // 7. 初始化表格数据
    refreshTable();
}

HouseInformationWidget::~HouseInformationWidget()
{
    delete ui;
}

void HouseInformationWidget::refreshTable()
{
    table->setRowCount(0);
    populateTable();
}

void HouseInformationWidget::populateTable()
{
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::warning(this, "错误", "数据库连接失败！");
        return;
    }

    for (int building = 1; building <= 12; ++building) {
        for (int floor = 201; floor <= 2002; floor = floor+100) {
                for (int room = 1; room <= 2; ++room) {
                    QString roomNumber = QString("%1号楼-%2")
                            .arg(building)
                            .arg(floor);  // 房号作为第二个参数
                    if (room == 1) floor++;
                    if (room == 2) floor--;

                    int row = table->rowCount();
                    table->insertRow(row);
                    QTableWidgetItem* item0 = new QTableWidgetItem(roomNumber);
                    table->setItem(row, 0, item0);

                QSqlQuery query;
                query.prepare("SELECT id, name, phonenumber FROM users WHERE address = ?");
                query.addBindValue(roomNumber);
                if (query.exec() && query.next()) {
                    QTableWidgetItem* item1 = new QTableWidgetItem(query.value("id").toString());
                    QTableWidgetItem* item2 = new QTableWidgetItem(query.value("name").toString());
                    QTableWidgetItem* item3 = new QTableWidgetItem(query.value("phonenumber").toString());
                    table->setItem(row, 1, item1);
                    table->setItem(row, 2, item2);
                    table->setItem(row, 3, item3);
                }
            }
        }
    }

    // 设置行高
    for (int i = 0; i < table->rowCount(); ++i) {
        table->setRowHeight(i, 60);
    }
}

void HouseInformationWidget::onSearch()
{
    QString roomNumber = lineEdit->text().trimmed();
    if (!isValidRoomNumber(roomNumber)) {
        QMessageBox::warning(this, "错误", "请输入符合格式“X号楼-XXX”的房号！");
        return;
    }

    QStringList parts = roomNumber.split("-");
    if (parts.size() != 2) {
        QMessageBox::warning(this, "错误", "房号格式不正确！");
        return;
    }

    QString buildingStr = parts[0].replace("号楼", "");
    QString floorAndRoom = parts[1];
    int building = buildingStr.toInt();
    int floor = floorAndRoom.left(floorAndRoom.length() - 2).toInt();
    int room = floorAndRoom.right(2).toInt();

    if (building < 1 || building > 12 || floor < 2 || floor > 20 || (room != 1 && room != 2)) {
        QMessageBox::warning(this, "错误", "房号不在小区范围内！");
        return;
    }

    table->setRowCount(0);
    int row = table->rowCount();
    table->insertRow(row);
    QTableWidgetItem* item0 = new QTableWidgetItem(roomNumber);
    table->setItem(row, 0, item0);

    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::warning(this, "错误", "数据库连接失败！");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT id, name, phonenumber FROM users WHERE address = ?");
    query.addBindValue(roomNumber);
    if (query.exec() && query.next()) {
        QTableWidgetItem* item1 = new QTableWidgetItem(query.value("id").toString());
        QTableWidgetItem* item2 = new QTableWidgetItem(query.value("name").toString());
        QTableWidgetItem* item3 = new QTableWidgetItem(query.value("phonenumber").toString());
        table->setItem(row, 1, item1);
        table->setItem(row, 2, item2);
        table->setItem(row, 3, item3);
    }

    // 设置行高
    table->setRowHeight(row, 60);
}

bool HouseInformationWidget::isValidRoomNumber(const QString& roomNumber)
{
    QRegExp regex("^\\d+号楼-\\d+0[12]$");
    return regex.exactMatch(roomNumber);
}

void HouseInformationWidget::onAdd()
{
    bool ok;
    QString address = QInputDialog::getText(this, "新增业主信息", "地址(输入格式示例：7号楼801)", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    QSqlQuery q;
    q.prepare("SELECT id FROM users WHERE address = ?");
    q.addBindValue(address);
    if(!q.exec()){
        QMessageBox::information(this, "提示", "已有业主信息，无法新增！");
        return;
    }
    QString username = QInputDialog::getText(this, "新增业主信息", "用户名：", QLineEdit::Normal, "", &ok);
    if (!ok || username.isEmpty()) return;
    QString password = QInputDialog::getText(this, "新增业主信息", "密码：", QLineEdit::Normal, "", &ok);
    if (!ok || password.isEmpty()) return;
    QString name = QInputDialog::getText(this, "新增业主信息", "姓名：", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    QString phonenumber = QInputDialog::getText(this, "新增业主信息", "联系电话：", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, name, phonenumber, address) VALUES(?, ?, ?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(name);
    query.addBindValue(phonenumber);
    query.addBindValue(address);
    query.exec();
    refreshTable();
}

void HouseInformationWidget::onEdit()
{
    // 这里可以实现编辑功能，暂时留空
    int row = table->currentRow();
    if (row < 0) return;
    QString address = table->item(row,0)->text();
    QTableWidgetItem *item = table->item(row, 1);
    if (item == nullptr || item->text().isEmpty()) {
        QMessageBox::information(this, "提示", "未录入业主信息，请先新增");
        return;
    }
    QString name = QInputDialog::getText(this,"编辑姓名","姓名",QLineEdit::Normal,table->item(row,2)->text());
    QString phonenumber = QInputDialog::getText(this,"编辑联系电话","电话",QLineEdit::Normal,table->item(row,3)->text());
    DatabaseManager db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery ql;
    ql.prepare("UPDATE users SET name = ?, phonenumber = ? WHERE address = ?");
    ql.addBindValue(name);
    ql.addBindValue(phonenumber);
    ql.addBindValue(address);
    ql.exec();
    refreshTable();
    table->setCurrentCell(row, 0);

}

void HouseInformationWidget::onDelete()
{
    int row = table->currentRow();
    if(row<0) return;
    int user_id = table->item(row,1)->text().toInt();
    if(QMessageBox::question(this,"确认","确定要删除该业主信息吗？")!=QMessageBox::Yes) return;

    DatabaseManager db;
    db.openDB(databasePath);
    QSqlQuery q2;
    q2.prepare("DELETE FROM users WHERE id=?");
    q2.addBindValue(user_id);
    q2.exec();
    refreshTable();
}

void HouseInformationWidget::onRadioButtonChanged(int id)
{
    table->setRowCount(0);
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::warning(this, "错误", "数据库连接失败！");
        return;
    }

    QSqlQuery query;
    if (id == 0) {
        // 仅看空闲房屋
        for (int building = 1; building <= 12; ++building) {
            for (int floor = 201; floor <= 2002; floor +=100) {
                for (int room = 1; room <= 2; ++room) {
                    QString roomNumber = QString("%1号楼-%2").arg(building).arg(floor);
                    if (room == 1) floor++;
                    if (room == 2) floor--;
                    query.prepare("SELECT id FROM users WHERE address = ?");
                    query.addBindValue(roomNumber);
                    if (query.exec() && !query.next()) {
                        int row = table->rowCount();
                        table->insertRow(row);
                        QTableWidgetItem* item0 = new QTableWidgetItem(roomNumber);
                        table->setItem(row, 0, item0);
                    }
                }
            }
        }
    } else if (id == 1){
        // 仅看已售房屋
        for (int building = 1; building <= 12; ++building) {
            for (int floor = 201; floor <= 2002; floor += 100) {
                for (int room = 1; room <= 2; ++room) {
                    QString roomNumber = QString("%1号楼-%2").arg(building).arg(floor);
                    if (room == 1) floor++;
                    if (room == 2) floor--;
                    query.prepare("SELECT id, name, phonenumber FROM users WHERE address = ?");
                    query.addBindValue(roomNumber);
                    if (query.exec() && query.next()) {
                        int row = table->rowCount();
                        table->insertRow(row);
                        QTableWidgetItem* item0 = new QTableWidgetItem(roomNumber);
                        QTableWidgetItem* item1 = new QTableWidgetItem(query.value("id").toString());
                        QTableWidgetItem* item2 = new QTableWidgetItem(query.value("name").toString());
                        QTableWidgetItem* item3 = new QTableWidgetItem(query.value("phonenumber").toString());
                        table->setItem(row, 0, item0);
                        table->setItem(row, 1, item1);
                        table->setItem(row, 2, item2);
                        table->setItem(row, 3, item3);
                    }
                }
            }
        }
    }else {
        refreshTable();
}

    // 设置行高
    for (int i = 0; i < table->rowCount(); ++i) {
        table->setRowHeight(i, 60);
    }
}
