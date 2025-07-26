#include "history.h"
#include "ui_history.h"
#include "databasemanager.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDate>
#include <QTextEdit>
#include <QDialog>
#include <QDebug>
#include <QVBoxLayout>

History::History(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::History)
{
    ui->setupUi(this);
    model = new QSqlTableModel(this);
    initDatabase();
    setupTableModel();

}

History::~History()
{
    delete ui;
}

void History::initDatabase()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("try.db");
    }

    if (!db.open()) {
        QMessageBox::critical(this, "错误", "数据库连接失败：" + db.lastError().text());
    }
}


void History::setupTableModel()
{
    model->setTable("payment");

    model->setHeaderData(0, Qt::Horizontal, "缴费单编号");
    model->setHeaderData(1, Qt::Horizontal, "缴费周期");
    model->setHeaderData(2, Qt::Horizontal, "缴费类型");
    model->setHeaderData(3, Qt::Horizontal, "应付金额");
    model->setHeaderData(4, Qt::Horizontal, "实付金额");
    model->setHeaderData(5, Qt::Horizontal, "缴费时间");
    model->setHeaderData(6, Qt::Horizontal, "用户名");
    model->setHeaderData(7, Qt::Horizontal, "姓名");
    model->setHeaderData(8, Qt::Horizontal, "家庭地址");
    model->setHeaderData(9, Qt::Horizontal, "联系电话");
    model->setHeaderData(10, Qt::Horizontal, "缴费方式");

    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
}


void History::on_queryButton_2_clicked()
{
    QString paymentTime = ui->paymentTimeEdit->text().trimmed();
    QString address = ui->addressEdit->text().trimmed();


    QString filter = "1=1";
    if (!paymentTime.isEmpty()) {
        filter += QString(" AND paymenttime LIKE '%%1%'").arg(paymentTime);
    }
    if (!address.isEmpty()) {
        filter += QString(" AND location LIKE '%%1%'").arg(address);
    }

    model->setFilter(filter);
    model->select();


    ui->tableView->resizeColumnsToContents();
}


void History::on_generateInvoiceButton_clicked()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "提示", "请选择一条缴费记录！");
        return;
    }

    QModelIndex index = selected.first();
    generateInvoice(index);
}


void History::generateInvoice(const QModelIndex &index)
{

    int paymentid = model->data(model->index(index.row(), 0)).toInt();
    QString paymentPeriod = model->data(model->index(index.row(), 1)).toString();
    QString paymentType = model->data(model->index(index.row(), 2)).toString();
    int paid = model->data(model->index(index.row(), 4)).toInt();
    QString paymentTime = model->data(model->index(index.row(), 5)).toString();
    QString username = model->data(model->index(index.row(), 6)).toString();
    QString method = model->data(model->index(index.row(), 10)).toString();
    if (method == "1") method = "线上支付";
    else method = "线下支付";
//    qDebug()<<"method是"<<method;
    QString address = model->data(model->index(index.row(), 8)).toString();     

    QString invoice = QString("—————— 电子发票 ——————\n") +
                      QString("订单编号：%1\n").arg(paymentid) +
                      QString("缴费周期：%1\n").arg(paymentPeriod) +
                      QString("缴费类型：%1\n").arg(paymentType) +
                      QString("实付金额：%1 元\n").arg(paid) +
                      QString("缴费时间：%1\n").arg(paymentTime) +
                      QString("缴费人用户名：%1\n").arg(username) +
                      QString("缴费方式：%1\n").arg(method) +
                      QString("家庭地址：%1\n").arg(address) +
                      "——————————————";


    QDialog *invoiceDialog = new QDialog(this);
    invoiceDialog->setWindowTitle("电子发票");
    QTextEdit *textEdit = new QTextEdit(invoiceDialog);
    textEdit->setPlainText(invoice);
    textEdit->setReadOnly(true);


    QFont font("SimHei", 10);
    textEdit->setFont(font);


    invoiceDialog->resize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(invoiceDialog);
    layout->addWidget(textEdit);
    invoiceDialog->exec();
}
void History::on_back_clicked()
{
    emit historygoBack();
}
