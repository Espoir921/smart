#include "evaluation.h"
#include "ui_evaluation.h"
#include "databasemanager.h"

#include<QDebug>

Evaluation::Evaluation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Evaluation),
    currentRequestId(-1)
{
    ui->setupUi(this);
    initDatabase();


        ui->recordTable->setColumnHidden(0, true); // 识）
        ui->recordTable->setColumnCount(5);
        ui->recordTable->setHorizontalHeaderLabels({
            "ID", "姓名", "地址", "报修时间", "状态"
        });
        ui->recordTable->horizontalHeader()->setStretchLastSection(true);


        connect(ui->queryBtn, &QPushButton::clicked, this, &Evaluation::onQueryBtnClicked);
        connect(ui->submitEvalBtn, &QPushButton::clicked, this, &Evaluation::onSubmitEvalBtnClicked);
        connect(ui->recordTable, &QTableWidget::cellClicked, this, &Evaluation::onTableItemClicked);
}

Evaluation::~Evaluation()
{
    delete ui;
}

void Evaluation::initDatabase()
{

    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {

        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("try.db");
    }


    if (!db.open()) {
        qCritical() << "数据库打开失败：" << db.lastError().text();
        QMessageBox::critical(this, "错误", "无法连接数据库，请检查路径或权限！");
    }
}

void Evaluation::loadRecords(const QString &username)
{

    ui->recordTable->setRowCount(0);
    DatabaseManager db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query;
    query.prepare("SELECT requestid, username, location, reporttime, status FROM maintenance WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec()) {
        while (query.next()) {
            int row = ui->recordTable->rowCount();
            ui->recordTable->insertRow(row);
            ui->recordTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            ui->recordTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            ui->recordTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
            ui->recordTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
            QString status = query.value(4).toString();
            if (status == "1") status = "已处理";
            else status = "未处理";
            ui->recordTable->setItem(row, 4, new QTableWidgetItem(status));

            if (query.value(4).toInt() == 1) {
                ui->recordTable->item(row, 4)->setForeground(Qt::green);
            } else {
                ui->recordTable->item(row, 4)->setForeground(Qt::red);
            }
        }
    } else {
        qCritical() << "查询失败：" << query.lastError().text();
        QMessageBox::warning(this, "错误", "无法加载报修记录！");
    }
}

void Evaluation::onQueryBtnClicked()
{
    QString username = ui->usernameEdit->text().trimmed();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入姓名！");
        return;
    }

    loadRecords(username);
}

void Evaluation::onTableItemClicked(int row, int column)
{
    Q_UNUSED(column);
    int requestId = ui->recordTable->item(row, 0)->text().toInt();
    QString status = ui->recordTable->item(row, 4)->text();

    currentRequestId = requestId;


    if (status == "已处理") {
        ui->evalEdit->setEnabled(true);
        ui->submitEvalBtn->setEnabled(true);
        QMessageBox::information(this, "提示", "可对该记录提交评价");
    } else {
        ui->evalEdit->setEnabled(false);
        ui->submitEvalBtn->setEnabled(false);
        QMessageBox::information(this, "提示", "该记录状态不允许评价");
    }
}

void Evaluation::onSubmitEvalBtnClicked()
{
    if (currentRequestId == -1) {
        QMessageBox::warning(this, "错误", "请先选择报修记录！");
        return;
    }

    QString evaluation = ui->evalEdit->toPlainText().trimmed();
    if (evaluation.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "评价内容不能为空！");
        return;
    }

    DatabaseManager db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query;
    query.prepare("UPDATE maintenance SET evaluation = :evaluation WHERE requestid = :requestid AND status = 1");
    query.bindValue(":evaluation", evaluation);
    query.bindValue(":requestid", currentRequestId);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            QMessageBox::information(this, "提交成功", "评价已提交！");

            ui->evalEdit->clear();
            currentRequestId = -1;
            ui->evalEdit->setEnabled(false);
            ui->submitEvalBtn->setEnabled(false);

            loadRecords(ui->usernameEdit->text().trimmed());
        } else {
            QMessageBox::warning(this, "提交失败", "该记录状态已变更，无法评价！");
        }
    } else {
        qCritical() << "更新失败：" << query.lastError().text();
        QMessageBox::critical(this, "错误", "提交评价失败！");
    }
}

void Evaluation::on_back_clicked()
{
    emit evaluationgoBack();
}
