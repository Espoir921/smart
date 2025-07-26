#include "parkingspaceallocationwidget.h"
#include "ui_parkingspaceallocationwidget.h"
#include "databasemanager.h"
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlError>
#include <QTableWidgetItem>

ParkingSpaceAllocationWidget::ParkingSpaceAllocationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParkingSpaceAllocationWidget)
{
    ui->setupUi(this);
    // 清除原有UI布局
    if (this->layout()) {
        QLayoutItem *item;
        while ((item = this->layout()->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        delete this->layout();
    }

    // 1. 创建标题
    QLabel *titleLabel = new QLabel("汤臣二品小区车位分配管理系统", this);
    titleLabel->setStyleSheet("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);

    // 2. 创建搜索区控件
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("请输入车位编号");
//    searchLineEdit->setFixedWidth(300);
    searchLineEdit->setFixedHeight(30);
    searchBtn = new QPushButton("搜索", this);
    QString btnStyle = "QPushButton { background-color: #19be6b; color: white; border-radius: 6px; font-family: 等线; font-weight: bold;}"
                       "QPushButton:hover { background-color: #13a456; }";
    searchBtn->setStyleSheet(btnStyle);
    searchBtn->setFixedSize(80,30);

    // 3. 创建单选按钮
    emptyRadio = new QRadioButton("查看空车位", this);
    occupiedRadio = new QRadioButton("查看已售/出租车位", this);
    radioGroup = new QButtonGroup(this);
    radioGroup->addButton(emptyRadio, 0);
    radioGroup->addButton(occupiedRadio, 1);

    // 4. 创建表格
    table = new QTableWidget(this);
    table->setColumnCount(14);
    QStringList headers = {
        "车位编号", "车型", "车牌号", "用户名", "姓名", "联系电话",
        "用户住址", "使用开始日期", "使用截止日期", "登记时间",
        "状态", "登记人ID", "登记人姓名", "车位性质"
    };
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QHeaderView::section { font-weight: bold; background: #f0f0f0; }"
                         "QTableWidget::item:selected { background: #2d8cf0; color: white; }");

    // 5. 设置布局
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 标题（第0行，跨4列）
    mainLayout->addWidget(titleLabel, 0, 0, 1, 4, Qt::AlignCenter);

    // 搜索区（第1行左侧）
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->addSpacing(100);  // 左侧间距
    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(searchBtn);
    searchLayout->setSpacing(5);    // 输入框和按钮之间的间距
    searchLayout->setAlignment(Qt::AlignLeft);  // 左对齐

    // 单选按钮区（第1行右侧）
    QHBoxLayout *radioLayout = new QHBoxLayout();
    radioLayout->addWidget(emptyRadio);
    radioLayout->addWidget(occupiedRadio);
    radioLayout->setSpacing(10);  // 单选按钮之间的间距

    // 添加右侧固定宽度的间隔（300px）
    radioLayout->addSpacing(300);
    radioLayout->setAlignment(Qt::AlignLeft);  // 左对齐，使间隔出现在右侧

    // 添加水平间隔（伸缩项）到主布局
    QSpacerItem *horizontalSpacer = new QSpacerItem(
        40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    // 将布局和伸缩项添加到主网格布局
    mainLayout->addLayout(searchLayout, 1, 0, 1, 1);  // 第1行第0列
    mainLayout->addItem(horizontalSpacer, 1, 1, 1, 1);  // 第1行第1列（伸缩项）
    mainLayout->addLayout(radioLayout, 1, 2, 1, 2);    // 第1行第2-3列

    // 表格（第2行，跨4列）
    mainLayout->addWidget(table, 2, 0, 1, 4);

    // 6. 连接信号与槽
    connect(searchBtn, &QPushButton::clicked, this, &ParkingSpaceAllocationWidget::onSearch);
    connect(radioGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &ParkingSpaceAllocationWidget::onRadioButtonChanged);

    // 初始加载数据
    refreshTable();
}

ParkingSpaceAllocationWidget::~ParkingSpaceAllocationWidget()
{
    delete ui;
}

// 刷新表格数据
void ParkingSpaceAllocationWidget::refreshTable()
{
    DatabaseManager &db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::warning(this, "错误", "数据库连接失败！");
        return;
    }

    QSqlQuery query = db.query("SELECT * FROM parkingspace");
    populateTable(query);
}

// 填充表格数据（辅助函数）
void ParkingSpaceAllocationWidget::populateTable(QSqlQuery &query)
{
    table->setRowCount(0); // 清空表格

    int row = 0;
    while (query.next()) {
        table->insertRow(row);

        // 1. 车位编号
        QTableWidgetItem *item0 = new QTableWidgetItem(query.value("spaceid").toString());
        // 2. 车型
        QTableWidgetItem *item1 = new QTableWidgetItem(query.value("type").toString());
        // 3. 车牌号
        QTableWidgetItem *item2 = new QTableWidgetItem(query.value("licenseplate").toString());
        // 4. 用户名
        QTableWidgetItem *item3 = new QTableWidgetItem(query.value("username").toString());
        // 5. 姓名
        QTableWidgetItem *item4 = new QTableWidgetItem(query.value("name").toString());
        // 6. 联系电话
        QTableWidgetItem *item5 = new QTableWidgetItem(query.value("phonenumber").toString());
        // 7. 用户住址
        QTableWidgetItem *item6 = new QTableWidgetItem(query.value("location").toString());
        // 8. 开始日期
        QTableWidgetItem *item7 = new QTableWidgetItem(query.value("start_date").toString());
        // 9. 截止日期
        QTableWidgetItem *item8 = new QTableWidgetItem(query.value("end_date").toString());
        // 10. 登记时间
        QTableWidgetItem *item9 = new QTableWidgetItem(query.value("registrationtime").toString());
        // 11. 状态（0=未审核，1=已审核）
        int status = query.value("status").toInt();
        QTableWidgetItem *item10 = new QTableWidgetItem(status == 1 ? "已审核" : "未审核");
        // 12. 登记人ID
        QTableWidgetItem *item11 = new QTableWidgetItem(query.value("registerid").toString());
        // 13. 登记人姓名
        QTableWidgetItem *item12 = new QTableWidgetItem(query.value("registername").toString());
        // 14. 车位性质（0=无主，1=已出售，2=出租）
        int nature = query.value("nature").toInt();
        QString natureText;
        switch (nature) {
            case 0: natureText = "无主"; break;
            case 1: natureText = "已出售"; break;
            case 2: natureText = "出租"; break;
            default: natureText = " ";
        }
        QTableWidgetItem *item13 = new QTableWidgetItem(natureText);

        // 设置单元格居中对齐
        QList<QTableWidgetItem*> items = {
            item0, item1, item2, item3, item4, item5, item6,
            item7, item8, item9, item10, item11, item12, item13
        };
        foreach (QTableWidgetItem *item, items) {
            item->setTextAlignment(Qt::AlignCenter);
        }

        // 添加到表格
        for (int col = 0; col < 14; ++col) {
            table->setItem(row, col, items[col]);
        }

        row++;
    }

    // 设置行高
    for (int i = 0; i < table->rowCount(); ++i) {
        table->setRowHeight(i, 60);
    }
}

// 搜索按钮槽函数（按车位编号搜索）
void ParkingSpaceAllocationWidget::onSearch()
{
    QString spaceId = searchLineEdit->text().trimmed();
    DatabaseManager &db = DatabaseManager::getInstance();

    if (!db.openDB(databasePath)) {
        QMessageBox::warning(this, "错误", "数据库连接失败！");
        return;
    }

    QSqlQuery query;
    if (spaceId.isEmpty()) {
        // 空输入时加载全部数据
        query = db.query("SELECT * FROM parkingspace");
    } else {
        // 按车位编号搜索
        query.prepare("SELECT * FROM parkingspace WHERE spaceid = ?");
        query.addBindValue(spaceId);
        if (!query.exec()) {
            QMessageBox::warning(this, "错误", "查询失败：" + query.lastError().text());
            return;
        }
    }

    populateTable(query);

    // 无结果提示
    if (table->rowCount() == 0 && !spaceId.isEmpty()) {
        QMessageBox::information(this, "提示", "未找到编号为 " + spaceId + " 的车位");
    }
}

// 单选按钮切换槽函数（空车位/已售车位）
void ParkingSpaceAllocationWidget::onRadioButtonChanged(int id)
{
    DatabaseManager &db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::warning(this, "错误", "数据库连接失败！");
        return;
    }

    QSqlQuery query;
    if (id == 0) {
        // 查看空车位（nature=0）
        query = db.query("SELECT * FROM parkingspace WHERE nature = 0");
    } else {
        // 查看已售/出租车位（nature!=0）
        query = db.query("SELECT * FROM parkingspace WHERE nature != 0");
    }

    populateTable(query);

    // 无结果提示
    if (table->rowCount() == 0) {
        QString msg = (id == 0) ? "未找到空车位" : "未找到已售/出租车位";
        QMessageBox::information(this, "提示", msg);
    }
}
