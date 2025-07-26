#include "bill.h"
#include "ui_bill.h"
#include "databasemanager.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QInputDialog>
#include <QLineEdit>
#include <QDebug>
#include <QDate>

Bill::Bill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Bill)
{
    ui->setupUi(this);
    initDatabase();
    initTableView();
        ui->paymentTypeComboBox->addItems(QStringList() << "车位购买" << "车位租赁" << "物业费");
        connect(ui->paymentTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Bill::onPaymentTypeChanged);
        connect(ui->generateBillButton, &QPushButton::clicked, this, &Bill::onGenerateBillClicked);
        connect(ui->payButton, &QPushButton::clicked, this, &Bill::onPayButtonClicked);
}

Bill::~Bill()
{
    if (db.isOpen()) {
        db.close();
    }
    delete ui;
}

void Bill::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("try.db");

    if (!db.open()) {
        QMessageBox::critical(this, "数据库错误", "无法打开数据库：" + db.lastError().text());
    }
}

void Bill::initTableView()
{

    paymentModel = new QStandardItemModel(0, 4, this);
    paymentModel->setHorizontalHeaderLabels(QStringList() << "缴费周期" << "类型" << "应付款" << "地址");


    ui->paymentTableView->setModel(paymentModel);
    ui->paymentTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->paymentTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Bill::onPaymentTypeChanged()
{
    ui->paymentPeriodSpinBox->setEnabled(false);
    ui->paymentPeriodSpinBox->setValue(0);
    ui->payableAmountLabel->setText("应付金额：0");
    ui->paymentPeriodSpinBox->setEnabled(true);

}

void Bill::onGenerateBillClicked()
{
    QString location = ui->addressLineEdit->text().trimmed();
    int paymentType = ui->paymentTypeComboBox->currentIndex();
    int paymentPeriod = ui->paymentPeriodSpinBox->value();
    int payable = 0;

    if (paymentType == 0) {
        QMessageBox::information(this, "提示", "请到线下缴费");
        return;
    }

    if (paymentType == 1) {



                payable = (paymentPeriod / 12) * 5000 + (paymentPeriod % 12) * 500;


    } else if (paymentType == 2) {
        payable = paymentPeriod * 120;
    }

    int row = paymentModel->rowCount();
    paymentModel->insertRow(row);
    paymentModel->setData(paymentModel->index(row, 0), paymentPeriod);
    paymentModel->setData(paymentModel->index(row, 1), paymentType == 1? 2 : 1);
    paymentModel->setData(paymentModel->index(row, 2), payable);
    paymentModel->setData(paymentModel->index(row, 3), location);


    ui->payableAmountLabel->setText("应付金额：" + QString::number(payable));
}

void Bill::onPayButtonClicked()
{

    QModelIndexList selectedRows = ui->paymentTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "提示", "请选择要缴费的账单");
        return;
    }

    int selectedRow = selectedRows.first().row();
    int paymentPeriod = paymentModel->data(paymentModel->index(selectedRow, 0)).toInt();
    int type = paymentModel->data(paymentModel->index(selectedRow, 1)).toInt();
    int payable = paymentModel->data(paymentModel->index(selectedRow, 2)).toInt();
    QString location = paymentModel->data(paymentModel->index(selectedRow, 3)).toString();
    QString username = QInputDialog::getText(this, "输入信息", "用户名：");
    QString name = QInputDialog::getText(this, "输入信息", "姓名：");
    QString phoneNumber = QInputDialog::getText(this, "输入信息", "电话号码：");

    if (username.isEmpty() || name.isEmpty() || phoneNumber.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写完整的用户名、姓名和电话号码");
        return;
    }

    DatabaseManager db = DatabaseManager::getInstance();
    db.openDB(databasePath);
    QSqlQuery query;
    query.prepare("INSERT INTO payment "
                  "(paymentperiod, type, payable, paid, paymenttime, username, name, location, phonenumber, method) "
                  "VALUES (:paymentperiod, :type, :payable, :paid, :paymenttime, :username, :name, :location, :phonenumber, :method)");
    query.bindValue(":paymentperiod", paymentPeriod);
    query.bindValue(":type", type);
    query.bindValue(":payable", payable);
    query.bindValue(":paid", payable);
    query.bindValue(":paymenttime", QDate::currentDate().toString("yyyy-MM-dd"));
    query.bindValue(":username", username);
    query.bindValue(":name", name);
    query.bindValue(":location", location);
    query.bindValue(":phonenumber", phoneNumber);
    query.bindValue(":method", 1);

    if (query.exec()) {
        QMessageBox::information(this, "提示", "缴费成功");


        paymentModel->removeRow(selectedRow);

        clearInputFields();
    } else {
        QMessageBox::critical(this, "错误", "缴费失败：" + query.lastError().text());
    }
}



void Bill::clearInputFields()
{
    ui->addressLineEdit->clear();
    ui->paymentTypeComboBox->setCurrentIndex(0);
    ui->paymentPeriodSpinBox->setValue(0);
    ui->payableAmountLabel->setText("应付金额：0");
}
void Bill::on_back_clicked()
{
    emit billgoBack();
}
