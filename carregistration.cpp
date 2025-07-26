#include "carregistration.h"
#include "ui_carregistration.h"
#include "databasemanager.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QRegularExpression>
#include <QDate>


CarRegistration::CarRegistration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CarRegistration)
{
    ui->setupUi(this);
   initDb();
   ui->comboBoxApplyType->addItems(QStringList() << "车位购买" << "车位租赁");
   ui->dateEditStart->setDisplayFormat("yyyy-MM");
      ui->dateEditEnd  ->setDisplayFormat("yyyy-MM");

      onApplyTypeChanged(0);
      connect(ui->comboBoxApplyType,QOverload<int>::of(&QComboBox::currentIndexChanged),this, &CarRegistration::onApplyTypeChanged);
      connect(ui->pushButtonSubmit, &QPushButton::clicked,this, &CarRegistration::onSubmitClicked);
}

CarRegistration::~CarRegistration()
{
    if (db.isOpen()) {
        db.close();
    }
    delete ui;
}

void CarRegistration::initDb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("try.db");
    if (!db.open())
        QMessageBox::critical(this, "数据库", db.lastError().text());
}

void CarRegistration::onApplyTypeChanged(int index)
{
    bool isLease = (index == 1);
    ui->dateEditStart->setEnabled(isLease);
    ui->dateEditEnd  ->setEnabled(isLease);
}

void CarRegistration::onSubmitClicked()
{
    DatabaseManager db = DatabaseManager::getInstance();
    if (!db.openDB(databasePath)) {
        QMessageBox::warning(this, "提示", "数据库未连接");
        return;
    }

    QString username   = ui->lineEditUsername->text().trimmed();
    QString phone      = ui->lineEditPhone->text().trimmed();

    if (username.isEmpty() || phone.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和联系电话为必填！");
        return;
    }

    int nature = ui->comboBoxApplyType->currentIndex() + 1;
    QDate startDate = ui->dateEditStart->date();
    QDate endDate   = ui->dateEditEnd->date();


    QSqlQuery q;
    q.prepare("INSERT INTO parkingspace "
              "(type, licenseplate, username, name, phonenumber, location, start_date, end_date, nature, status) "
              "VALUES (:type, :licenseplate, :username, :name, :phonenumber, :location, :start_date, :end_date, :nature, :status)");
    q.bindValue(":type", ui->lineEditCarType->text().trimmed());
    q.bindValue(":licenseplate", ui->lineEditLicense->text().trimmed());
    q.bindValue(":username", username);
    q.bindValue(":name", ui->lineEditName->text().trimmed());
    q.bindValue(":phonenumber", phone);
    q.bindValue(":location", ui->lineEditLocation->text().trimmed());

    if (nature == 1) {
        q.bindValue(":start_date", QVariant());
        q.bindValue(":end_date", QVariant());
    } else {
        q.bindValue(":start_date", startDate);
        q.bindValue(":end_date", endDate);
    }
    if (nature == 2) {
        QString startDateStr = startDate.toString("yyyy-MM");
        QString endDateStr = endDate.toString("yyyy-MM");
        q.bindValue(":start_date", startDateStr);
        q.bindValue(":end_date", endDateStr);
    }
    q.bindValue(":nature", 0);
    q.bindValue(":status", 0);
    if (!q.exec())
        QMessageBox::critical(this, "错误", q.lastError().text());
    else {
        QMessageBox::information(this, "提示", "申请已提交！");

        ui->lineEditCarType->clear();
        ui->lineEditLicense->clear();
        ui->lineEditUsername->clear();
        ui->lineEditName->clear();
        ui->lineEditPhone->clear();
        ui->lineEditLocation->clear();
        ui->comboBoxApplyType->setCurrentIndex(0);
        onApplyTypeChanged(0);
    }
}
void CarRegistration::on_back_clicked()
{
    emit carregistrationgoBack();
}
