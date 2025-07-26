#include "carrenterwidget.h"
#include "ui_carrenterwidget.h"
#include <QDebug>
#include <QSqlError>
#include <QTabWidget>
#include "databasemanager.h"
#include <QSqlQuery>
#include <QDateTime>
#include <QDebug>


CarRenterWidget::CarRenterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CarRenterWidget)
{
    ui->setupUi(this);
    initDatabase();
    initUI();
    initStyle();
    loadData();
}

CarRenterWidget::~CarRenterWidget()
{
    delete ui;
}

void CarRenterWidget::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("try.db"); // 数据库文件（不存在则自动创建）

    if (!db.open()) {
        QMessageBox::critical(this, "错误", "数据库连接失败：" + db.lastError().text());
        return;
    }

    // 创建表结构（首次运行时创建表）
    QSqlQuery createTable(db);
    createTable.exec(R"(
        CREATE TABLE IF NOT EXISTS parkingspace (
            spaceid INTEGER PRIMARY KEY AUTOINCREMENT,
            type TEXT,
            licenseplate TEXT,
            username TEXT,
            name TEXT,
            phonenumber TEXT,
            location TEXT,
            start_date TEXT,
            end_date TEXT,
            registrationtime TEXT,
            status INTEGER,
            registerid TEXT,
            registername TEXT,
            nature INTEGER
        )
    )");
}

void CarRenterWidget::initUI()
{
    // 标签页
    tabWidget = new QTabWidget(this);
    //标题
    QLabel *titleLabel = new QLabel("汤臣二品车辆登记", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");

    // 未审核表格
    unAuditedTable = new QTableWidget(0, 14, this);
    unAuditedTable->setHorizontalHeaderLabels({
        "车位编号", "车型", "车牌号", "用户名", "姓名",
        "电话", "地址", "开始日期", "结束日期", "登记时间",
        "状态", "登记人ID", "登记人姓名", "车位性质"
    });

    // 已审核表格
    auditedTable = new QTableWidget(0, 14, this);
    QStringList headers = {
        "车位编号", "车型", "车牌号", "用户名", "姓名",
        "电话", "地址", "开始日期", "结束日期", "登记时间",
        "状态", "登记人ID", "登记人姓名", "车位性质"
    };
    auditedTable->setHorizontalHeaderLabels(headers);

    // 按钮区
    btnRefreshUnAudit = new QPushButton("刷新未审核", this);
    btnRefreshAudit = new QPushButton("刷新已审核", this);
    btnAudit = new QPushButton("审核选中行", this);
    btnAdd = new QPushButton("新增车位数据", this);

    // 布局：按钮栏 + 表格
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnRefreshUnAudit);
    btnLayout->addWidget(btnRefreshAudit);
    btnLayout->addWidget(btnAudit);
    btnLayout->addWidget(btnAdd);

    QWidget *unAuditPage = new QWidget;
    QVBoxLayout *unAuditLayout = new QVBoxLayout(unAuditPage);
    unAuditLayout->addLayout(btnLayout);
    unAuditLayout->addWidget(unAuditedTable);

    QWidget *auditPage = new QWidget;
    QVBoxLayout *auditLayout = new QVBoxLayout(auditPage);
    auditLayout->addWidget(btnRefreshAudit); // 已审核页也放刷新按钮
    auditLayout->addWidget(auditedTable);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    tabWidget->addTab(unAuditPage, "未审核");
    tabWidget->addTab(auditPage, "已审核");
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
    tabWidget->setStyleSheet(
            "QTabBar::tab {"
            "    min-width: 80px;"    // 标签的最小宽度
            "    min-height: 50px;"    // 标签的最小高度
            "}"
        );

    // 信号连接
    connect(btnRefreshUnAudit, &QPushButton::clicked, this, &CarRenterWidget::onRefreshUnAuditClicked);
    connect(btnRefreshAudit, &QPushButton::clicked, this, &CarRenterWidget::onRefreshAuditClicked);
    connect(btnAudit, &QPushButton::clicked, this, &CarRenterWidget::onAuditClicked);
    connect(btnAdd, &QPushButton::clicked, this, &CarRenterWidget::onAddClicked);

}

void CarRenterWidget::initStyle()
{
    // 表格美化
    auto beautifyTable = [](QTableWidget *table) {
        table->horizontalHeader()->setStyleSheet("QHeaderView { background: #ECECEC; color: #333; font-weight: bold; }");
        table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setAlternatingRowColors(true);
        table->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                             "QHeaderView::section { font-weight: bold; } "
                             "QTableWidget::item:selected { background: #2d8cf0; }");
        table->setShowGrid(false);
        table->setSelectionBehavior(QTableWidget::SelectRows);

    };
    beautifyTable(unAuditedTable);
    beautifyTable(auditedTable);
    setFixedRowHeight(unAuditedTable, 60);
    setFixedRowHeight(auditedTable, 60);


    // 按钮美化
    QString btnStyle = R"(
        QPushButton {
            background: #4CAF50;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-size: 14px;
        }
        QPushButton:hover {
            background: #45a049;
        }
    )";
    btnRefreshUnAudit->setStyleSheet(btnStyle);
    btnRefreshAudit->setStyleSheet(btnStyle);
    btnAudit->setStyleSheet(btnStyle);
    btnAdd->setStyleSheet(btnStyle);

    // 整体窗口
    setStyleSheet("background: white;");
}

// 固定行高
void CarRenterWidget::setFixedRowHeight(QTableWidget *table, int height)
{
    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHeight(i, height);
}


void CarRenterWidget::loadData()
{
    fillTable(unAuditedTable, 0); // 加载未审核数据（status=0）
    fillTable(auditedTable, 1);   // 加载已审核数据（status=1）
}

// --- 表格操作 ---
void CarRenterWidget::fillTable(QTableWidget *table, int status)
{
    table->setRowCount(0);
    table->setColumnCount(14); // 设置列数

    QStringList headers = {"车位编号", "车型", "车牌号", "用户名", "姓名", "电话", "地址", "开始日期", "结束日期", "登记时间", "状态", "登记人ID", "登记人姓名", "车位性质"};
    table->setHorizontalHeaderLabels(headers);

    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    // 修改SQL查询，根据传入的status参数过滤数据，并将status和nature转换为文本
    QSqlQuery query;
    query.prepare(R"(
        SELECT
            spaceid,
            type,
            licenseplate,
            username,
            name,
            phonenumber,
            location,
            start_date,
            end_date,
            registrationtime,
            CASE
                WHEN status = 0 THEN '未审核'
                WHEN status = 1 THEN '已审核'
                ELSE '未知状态'
            END AS status_text,
            registerid,
            registername,
            CASE
                WHEN nature = 0 THEN '无主'
                WHEN nature = 1 THEN '已出售'
                WHEN nature = 2 THEN '出租'
                ELSE '未知性质'
            END AS nature_text
        FROM parkingspace
        WHERE status = :status
    )");
    query.bindValue(":status", status);

    if (query.exec()) {
        int row = 0;
        while (query.next()) {
            table->insertRow(row);

            // 设置表格数据
            table->setItem(row, 0, new QTableWidgetItem(query.value("spaceid").toString()));
            table->setItem(row, 1, new QTableWidgetItem(query.value("type").toString()));
            table->setItem(row, 2, new QTableWidgetItem(query.value("licenseplate").toString()));
            table->setItem(row, 3, new QTableWidgetItem(query.value("username").toString()));
            table->setItem(row, 4, new QTableWidgetItem(query.value("name").toString()));
            table->setItem(row, 5, new QTableWidgetItem(query.value("phonenumber").toString()));
            table->setItem(row, 6, new QTableWidgetItem(query.value("location").toString()));
            table->setItem(row, 7, new QTableWidgetItem(query.value("start_date").toString()));
            table->setItem(row, 8, new QTableWidgetItem(query.value("end_date").toString()));
            table->setItem(row, 9, new QTableWidgetItem(query.value("registrationtime").toString()));
            table->setItem(row, 10, new QTableWidgetItem(query.value("status_text").toString()));
            table->setItem(row, 11, new QTableWidgetItem(query.value("registerid").toString()));
            table->setItem(row, 12, new QTableWidgetItem(query.value("registername").toString()));
            table->setItem(row, 13, new QTableWidgetItem(query.value("nature_text").toString()));

            row++;
        }

        // 设置单元格居中
        for (int r = 0; r < table->rowCount(); ++r) {
            for (int c = 0; c < table->columnCount(); ++c) {
                if (QTableWidgetItem* item = table->item(r, c)) {
                    item->setTextAlignment(Qt::AlignCenter);
                }
            }
        }
        setFixedRowHeight(table, 60);

    } else {
        QMessageBox::critical(this, "错误", "查询数据失败：" + query.lastError().text());
    }
}

void CarRenterWidget::refreshTable(QTableWidget *table, int status)
{
    fillTable(table, status);
}

// --- 审核逻辑 ---
void CarRenterWidget::onAuditClicked()
{
    int row = unAuditedTable->currentRow();
    if (row == -1) {
        QMessageBox::information(this, "提示", "请先选中要审核的行");
        return;
    }
    openAuditDialog(row);
}

void CarRenterWidget::openAuditDialog(int row)
{
//    // 弹出审核对话框
//    QDialog *dialog = new QDialog(this);
//    dialog->setWindowTitle("审核车位数据");
//    dialog->setFixedSize(600, 300);

    // 获取当前行数据
    QTableWidgetItem *itemSpaceId = unAuditedTable->item(row, 0);
    QString spaceId = itemSpaceId ? itemSpaceId->text() : "";
    QTableWidgetItem *itemtime = unAuditedTable->item(row,8);
    int nature = 2;
    if(itemtime->text().isEmpty()) nature = 1;

//    // 表单组件
//    QLabel *lblStart = new QLabel("开始日期:", dialog);
//    QDateEdit *editStart = new QDateEdit(QDate::currentDate(), dialog);

//    QLabel *lblEnd = new QLabel("结束日期:", dialog);
//    QDateEdit *editEnd = new QDateEdit(QDate::currentDate(), dialog);

//    QPushButton *btnSave = new QPushButton("确认审核", dialog);
//    QPushButton *btnCancel = new QPushButton("取消", dialog);

//    // 布局
//    QVBoxLayout *layout = new QVBoxLayout(dialog);

//    layout->addWidget(lblStart);
//    layout->addWidget(editStart);
//    layout->addWidget(lblEnd);
//    layout->addWidget(editEnd);

//    QHBoxLayout *btnLayout = new QHBoxLayout;
//    btnLayout->addWidget(btnSave);
//    btnLayout->addWidget(btnCancel);
//    layout->addLayout(btnLayout);
      QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "审核", "是否通过审核？",QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

//        QSqlQuery query;
//        query.prepare(R"(
//            UPDATE parkingspace
//            SET status = 1,
//                start_date = :start,
//                end_date = :end
//            WHERE spaceid = :id
//        )");
//        query.bindValue(":start", editStart->date().toString("yyyy-MM-dd"));
//        query.bindValue(":end", editEnd->date().toString("yyyy-MM-dd"));
//        query.bindValue(":id", spaceId.toInt());

      if (reply == QMessageBox::Yes){
          QDateTime currentTime = QDateTime::currentDateTime();
          QString time = currentTime.toString("yyyy-MM-dd hh:mm:ss");
          QSqlQuery query;
                  query.prepare("UPDATE parkingspace SET registrationtime = ?, status = ?, registerid = ?, registername = ?, nature = ? WHERE spaceid = ?" );
                  query.addBindValue(time);
                  query.addBindValue(1);
                  query.addBindValue(m_userId);
                  query.addBindValue(m_name);
                  query.addBindValue(nature);
                  query.addBindValue(spaceId.toInt());

        if (query.exec()) {
            QMessageBox::information(this, "成功", "审核完成！数据已更新");
            refreshTable(unAuditedTable, 0); // 刷新未审核
            refreshTable(auditedTable, 1);   // 刷新已审核
//            dialog->close();
        } else {
            QMessageBox::warning(this, "错误", "审核失败：" + query.lastError().text());
        }
    }

}

// --- 新增数据 ---
    void CarRenterWidget::onAddClicked()
    {
        openAddDialog();
        openUpdateDialog();
        refreshTable(auditedTable, 0);
    }

    //获取当前id和name
    void CarRenterWidget::setUserId(const int userId){
        m_userId = userId;
        DatabaseManager& db = DatabaseManager::getInstance();
        db.openDB(databasePath);
        QSqlQuery query;
        query.prepare("SELECT name FROM users WHERE id = ?");
        query.addBindValue(m_userId);
        query.exec();
        while (query.next()){
            m_name = query.value(0).toString();
        }
    }

void CarRenterWidget::openAddDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("新增车位数据");
    dialog->setFixedSize(400, 400);

    // 表单组件
    QLabel *lblType = new QLabel("车型:", dialog);
    QLineEdit *editType = new QLineEdit(dialog);

    QLabel *lblPlate = new QLabel("车牌号:", dialog);
    QLineEdit *editPlate = new QLineEdit(dialog);

    QLabel *lblUser = new QLabel("用户名:", dialog);
    QLineEdit *editUser = new QLineEdit(dialog);

    QLabel *lblName = new QLabel("姓名:", dialog);
    QLineEdit *editName = new QLineEdit(dialog);

    QLabel *lblPhone = new QLabel("电话:", dialog);
    QLineEdit *editPhone = new QLineEdit(dialog);

    QLabel *lblLoc = new QLabel("地址:", dialog);
    QLineEdit *editLoc = new QLineEdit(dialog);

    QLabel *lblNature = new QLabel("车位性质:", dialog);
    QComboBox *comboNature = new QComboBox(dialog);
    comboNature->addItems({"无主", "已出售", "出租"});

    QPushButton *btnSave = new QPushButton("保存", dialog);
    QPushButton *btnCancel = new QPushButton("取消", dialog);

    // 布局
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(lblType);
    layout->addWidget(editType);
    layout->addWidget(lblPlate);
    layout->addWidget(editPlate);
    layout->addWidget(lblUser);
    layout->addWidget(editUser);
    layout->addWidget(lblName);
    layout->addWidget(editName);
    layout->addWidget(lblPhone);
    layout->addWidget(editPhone);
    layout->addWidget(lblLoc);
    layout->addWidget(editLoc);
    layout->addWidget(lblNature);
    layout->addWidget(comboNature);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    layout->addLayout(btnLayout);

    // 保存逻辑
    connect(btnSave, &QPushButton::clicked, [=]() {
        QMap<QString, QString> data;
        data["type"] = editType->text();
        data["licenseplate"] = editPlate->text();
        data["username"] = editUser->text();
        data["name"] = editName->text();
        data["phonenumber"] = editPhone->text();
        data["location"] = editLoc->text();
        data["nature"] = QString::number(comboNature->currentIndex()); // 0/1/2
        data["status"] = "1"; // 新增默认已审核
        addDataToDB(data);
        dialog->close();
//        refreshTable(unAuditedTable, 0); // 刷新未审核表格
    });

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::close);
    dialog->exec();
}



void CarRenterWidget::addDataToDB(const QMap<QString, QString>& data)
{
    DatabaseManager& db = DatabaseManager::getInstance();
        if (!db.openDB(databasePath)) {
            QMessageBox::critical(this, "错误", "数据库连接失败！");
            return;
        }
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO parkingspace
        (type, licenseplate, username, name, phonenumber, location, nature, status)
        VALUES (:type, :plate, :user, :name, :phone, :loc, :nature, :status)
    )");
    query.bindValue(":type", data["type"]);
    query.bindValue(":plate", data["licenseplate"]);
    query.bindValue(":user", data["username"]);
    query.bindValue(":name", data["name"]);
    query.bindValue(":phone", data["phonenumber"]);
    query.bindValue(":loc", data["location"]);
    query.bindValue(":nature", data["nature"]);
    query.bindValue(":status", data["status"]);

    if (query.exec()) {
            QSqlQuery q;
            q.prepare(R"(
                SELECT spaceid FROM parkingspace
                WHERE type = :type AND licenseplate = :plate AND username = :user AND phonenumber = :phone AND location = :loc
            )");
        q.bindValue(":type", data["type"]);
        q.bindValue(":plate", data["licenseplate"]);
        q.bindValue(":user", data["username"]);
        q.bindValue(":phone", data["phonenumber"]);
        q.bindValue(":loc", data["location"]);
        q.exec();
        while (q.next()){
                    addId = q.value(0).toInt();
//                    qDebug()<<"addId is"<<addId;

                }

    } else {
        QMessageBox::warning(this, "错误", "新增失败：" + query.lastError().text());
    }
}

void CarRenterWidget::openUpdateDialog(){
    QDialog *dialog = new QDialog(this);
    QLabel *lblstart = new QLabel("车位使用开始时间:", dialog);
    QLineEdit *editstart = new QLineEdit(dialog);

    QLabel *lblend = new QLabel("车位使用结束时间:", dialog);
    QLineEdit *editend = new QLineEdit(dialog);
    dialog->setWindowTitle("购买者信息录入");
    dialog->setFixedSize(400, 400);
    QLabel *lblid = new QLabel("登记人id:", dialog);
    QLineEdit *editid = new QLineEdit(dialog);

    QLabel *lblname = new QLabel("登记人姓名:", dialog);
    QLineEdit *editname = new QLineEdit(dialog);

    QPushButton *btnSave = new QPushButton("保存", dialog);
    QPushButton *btnCancel = new QPushButton("取消", dialog);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(lblstart);
    layout->addWidget(editstart);
    layout->addWidget(lblend);
    layout->addWidget(editend);
    layout->addWidget(lblid);
    layout->addWidget(editid);
    layout->addWidget(lblname);
    layout->addWidget(editname);
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    layout->addLayout(btnLayout);
    connect(btnSave, &QPushButton::clicked, [=]() {
        QMap<QString, QString> data;
        data["start_date"] = editstart->text();
        data["end_date"] = editend->text();
        data["register"] = editid->text();
        data["registername"] = editname->text();
        updateDataToDB(data);

        dialog->close();
        refreshTable(unAuditedTable, 0); // 刷新未审核表格
        });

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::close);
    dialog->exec();
}

void CarRenterWidget::updateDataToDB(const QMap<QString, QString>& data)
{
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::critical(this, "错误", "数据库连接失败！");
        return;
    }
    // 获取当前日期和时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString time = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    QSqlQuery query;
    query.prepare(R"(
        UPDATE parkingspace
        SET start_date = :start_date,
            end_date = :end_date,
            registrationtime = :time,
            registerid = :register,
            registername = :registername
        WHERE spaceid = :id
    )");
    query.bindValue(":start_date", data["start_date"]);
    query.bindValue(":end_date", data["end_date"]);
    query.bindValue(":time", time);
    query.bindValue(":register", data["register"]);
    query.bindValue(":registername", data["registername"]);
    query.bindValue(":id", addId);
//        qDebug()<<addId;


    if (query.exec()) {
        QMessageBox::information(this, "成功", "数据新增成功！");
    } else {
        QMessageBox::warning(this, "错误", "新增失败：" + query.lastError().text());
    }
}


// --- 槽函数 ---
void CarRenterWidget::onRefreshUnAuditClicked()
{
    refreshTable(unAuditedTable, 0);
}

void CarRenterWidget::onRefreshAuditClicked()
{
    refreshTable(auditedTable, 1);
}
