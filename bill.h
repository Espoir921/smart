#ifndef BILL_H
#define BILL_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableView>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDate>
#include <QInputDialog>
namespace Ui {
class Bill;
}

class Bill : public QWidget
{
    Q_OBJECT

public:
    explicit Bill(QWidget *parent = nullptr);
    ~Bill();


private slots:

    void on_back_clicked();
private slots:
    void onPaymentTypeChanged();
        void onGenerateBillClicked();
        void onPayButtonClicked();


signals:
    void billgoBack();

private:
    Ui::Bill *ui;
    QSqlDatabase db;
    QStandardItemModel *paymentModel;  // 表格数据模型
        void initDatabase();
        void initTableView();
        void clearInputFields();


};

#endif // BILL_H
