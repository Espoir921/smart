#include "announcementswidget.h"
#include "ui_announcementswidget.h"
#include <QMessageBox>
#include <QSqlError>
#include<QSqlQuery>
#include<QDebug>
#include <QComboBox>

AnnouncementsWidget::AnnouncementsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnnouncementsWidget)
{
    ui->setupUi(this);
    initDatabase();
    loadDataToTableWidget();
    connect(ui->pushButton,&QPushButton::clicked,this,&AnnouncementsWidget::on_publishBtn_clicked);
    connect(ui->pushButton_2,&QPushButton::clicked,this,&AnnouncementsWidget::on_editBtn_clicked);
    connect(ui->pushButton_3,&QPushButton::clicked,this,&AnnouncementsWidget::on_deleteBtn_clicked);
    connect(ui->pushButton_4,&QPushButton::clicked,this,&AnnouncementsWidget::on_refreshBtn_clicked);
    connect(ui->pushButton_5,&QPushButton::clicked,this,&AnnouncementsWidget::onread);

}

AnnouncementsWidget::~AnnouncementsWidget()
{
    delete ui;
}

// 初始化数据库
void AnnouncementsWidget::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("try.db");

    if (!db.open()) {
        QMessageBox::critical(this, "错误", "数据库打开失败：" + db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS announcements (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type TEXT,
            content TEXT,
            creator TEXT,
            created_at TEXT
        )
    )");

    // 初始化表格
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"公告类型", "文本", "发布者", "发布时间"});
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

// 从数据库加载数据到表格
void AnnouncementsWidget::loadDataToTableWidget()
{
    ui->tableWidget->setRowCount(0); // 清空表格

    QSqlQuery query("SELECT type, content, creator, created_at FROM announcements");
    int row = 0;

    while (query.next()) {
        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));

        row++;
    }
}

// 获取选中的行号
int AnnouncementsWidget::getSelectedRow() const
{
    auto selectedItems = ui->tableWidget->selectedItems();
    if (selectedItems.isEmpty())
        return -1;

    return selectedItems.first()->row();
}

// 发布公告
void AnnouncementsWidget::on_publishBtn_clicked()
{
    qDebug()<<"来了";
    db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("try.db");

        if (!db.open()) {
            QMessageBox::critical(this, "错误", "数据库打开失败：" + db.lastError().text());
            return;
        }
    PublishDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QSqlQuery query;
        query.prepare("INSERT INTO announcements (type, content, creator, created_at) "
                      "VALUES (:type, :content, :creator, :created_at)");
        query.bindValue(":type", dialog.type);
        query.bindValue(":content", dialog.content);
        query.bindValue(":creator", dialog.creator);
        query.bindValue(":created_at", dialog.createdTime);

        if (query.exec()) {
            QMessageBox::information(this, "成功", "公告发布成功！");
            loadDataToTableWidget(); // 刷新表格
        } else {
            QMessageBox::critical(this, "失败", "数据库错误：" + query.lastError().text());
        }
    }
}

// 编辑公告
void AnnouncementsWidget::on_editBtn_clicked()
{
    int row = getSelectedRow();
    if (row == -1) {
        QMessageBox::warning(this, "提示", "请选中要编辑的行！");
        return;
    }

    // 获取当前行数据
    QString type = ui->tableWidget->item(row, 0)->text();
    QString content = ui->tableWidget->item(row, 1)->text();
    QString creator = ui->tableWidget->item(row, 2)->text();
    QString createdTime = ui->tableWidget->item(row, 3)->text();

    // 弹出编辑对话框
    PublishDialog dialog(this, true);
    dialog.type = type;
    dialog.content = content;
    dialog.creator = creator;

     dialog.createdTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

     if (dialog.exec() == QDialog::Accepted) {
             // 获取公告ID（仍用原时间查询，避免因内容修改导致找不到记录）
             QSqlQuery getIdQuery;
             getIdQuery.prepare("SELECT id FROM announcements WHERE content = :content AND created_at = :created_at");
             getIdQuery.bindValue(":content", content);
             getIdQuery.bindValue(":created_at", createdTime); // 用原时间查ID
             getIdQuery.exec();

             if (getIdQuery.next()) {
                 int id = getIdQuery.value(0).toInt();

                 // 更新数据库（时间改为当前时间）
                 QSqlQuery updateQuery;
                 updateQuery.prepare("UPDATE announcements SET type = :type, content = :content, "
                                    "creator = :creator, created_at = :created_at WHERE id = :id");
                 updateQuery.bindValue(":type", dialog.type);
                 updateQuery.bindValue(":content", dialog.content);
                 updateQuery.bindValue(":creator", dialog.creator);
                 updateQuery.bindValue(":created_at", dialog.createdTime); // 存入当前时间
                 updateQuery.bindValue(":id", id);

                 if (updateQuery.exec()) {
                     QMessageBox::information(this, "成功", "公告编辑成功！");
                     loadDataToTableWidget(); // 刷新表格
                 } else {

                     QMessageBox::critical(this, "失败", "数据库错误：" + updateQuery.lastError().text());
                 }
             }
         }
     }

// 删除公告
void AnnouncementsWidget::on_deleteBtn_clicked()
{
    int row = getSelectedRow();
    if (row == -1) {
        QMessageBox::warning(this, "提示", "请选中要删除的行！");
        return;
    }

    QString content = ui->tableWidget->item(row, 1)->text();
    QString createdTime = ui->tableWidget->item(row, 3)->text();

    if (QMessageBox::question(this, "确认", "是否删除选中公告？") == QMessageBox::Yes) {
        // 获取公告ID
        QSqlQuery getIdQuery;
        getIdQuery.prepare("SELECT id FROM announcements WHERE content = :content AND created_at = :created_at");
        getIdQuery.bindValue(":content", content);
        getIdQuery.bindValue(":created_at", createdTime);
        getIdQuery.exec();

        if (getIdQuery.next()) {
            int id = getIdQuery.value(0).toInt();

            // 删除记录
            QSqlQuery deleteQuery;
            deleteQuery.prepare("DELETE FROM announcements WHERE id = :id");
            deleteQuery.bindValue(":id", id);

            if (deleteQuery.exec()) {
                QMessageBox::information(this, "成功", "公告删除成功！");
                loadDataToTableWidget(); // 刷新表格
            } else {
                QMessageBox::critical(this, "失败", "数据库错误：" + deleteQuery.lastError().text());
            }
        }
    }
}

// 刷新表格
void AnnouncementsWidget::on_refreshBtn_clicked()
{
    initDatabase();
    loadDataToTableWidget();
}

// 发布/编辑对话框实现
PublishDialog::PublishDialog(QWidget *parent, bool isEditMode) : QDialog(parent)
{
    setWindowTitle(isEditMode ? "编辑公告" : "发布公告");

    // 界面组件
    auto comboType = new QComboBox(this);
    comboType->addItems({"通知", "公示", "警示", "倡议"});

    auto textEdit = new QTextEdit(this);
    auto leCreator = new QLineEdit(this);
    auto leTime = new QLineEdit(this);
    leTime->setReadOnly(true);
    leTime->setStyleSheet("background-color: #f0f0f0;");

    // 确认/取消按钮
    auto btnOk = new QPushButton("确认", this);
    auto btnCancel = new QPushButton("取消", this);

    // 布局
    auto layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("公告类型:", this));
    layout->addWidget(comboType);
    layout->addWidget(new QLabel("公告内容:", this));
    layout->addWidget(textEdit);
    layout->addWidget(new QLabel("发布者:", this));
    layout->addWidget(leCreator);
    layout->addWidget(new QLabel("发布时间:", this));
    layout->addWidget(leTime);

    auto btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);
    layout->addLayout(btnLayout);

//    // 设置当前时间（发布时自动生成，编辑时使用原有时间）
//    if (isEditMode) {
//        comboType->setCurrentText(type);
//        textEdit->setPlainText(content);
//        leCreator->setText(creator);
//        leTime->setText(createdTime);
//    } else {
        leTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
//    }

    // 确认按钮点击事件
    connect(btnOk, &QPushButton::clicked, [=]() {
        if (textEdit->toPlainText().isEmpty() || leCreator->text().isEmpty()) {
            QMessageBox::warning(this, "提示", "公告内容和发布者不能为空！");
            return;
        }

        type = comboType->currentText();
        content = textEdit->toPlainText();
        creator = leCreator->text();
        createdTime = leTime->text();
        accept();
    });

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void AnnouncementsWidget::onread() {
    // 获取选中的行（因为是整行选择，取第一行即可）
    QList<QTableWidgetItem *> selectedItems = ui->tableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择一个公告！");
        return;
    }
    // 取选中项所在的行号
    int row = selectedItems.first()->row();

    // 从表格列中直接获取数据（列索引0-3对应类型、文本、发布者、时间）
    QString type = ui->tableWidget->item(row, 0)->text();
    QString content = ui->tableWidget->item(row, 1)->text();
    QString creator = ui->tableWidget->item(row, 2)->text();
    QString time = ui->tableWidget->item(row, 3)->text();

    QDialog dialog(this);
    dialog.setWindowTitle("公告详情");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *typeLabel = new QLabel(QString("类型：%1").arg(type), &dialog);
    QLabel *creatorLabel = new QLabel(QString("创建人：%1").arg(creator), &dialog);
    QLabel *timeLabel = new QLabel(QString("时间：%1").arg(time), &dialog);
    QTextEdit *contentEdit = new QTextEdit(content, &dialog);
    contentEdit->setReadOnly(true);

    layout->addWidget(typeLabel);
    layout->addWidget(creatorLabel);
    layout->addWidget(timeLabel);
    layout->addWidget(contentEdit);

    dialog.exec();
}
