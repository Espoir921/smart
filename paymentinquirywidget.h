#ifndef PAYMENTINQUIRYWIDGET_H
#define PAYMENTINQUIRYWIDGET_H
#include<databasemanager.h>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include<QTableWidget>
QT_BEGIN_NAMESPACE
namespace Ui { class PaymentInquiryWidget; }
QT_END_NAMESPACE

class PaymentInquiryWidget : public QWidget
{
    Q_OBJECT

public:
    PaymentInquiryWidget(QWidget *parent = nullptr);
    ~PaymentInquiryWidget();


private slots:
    void searchone();
    void searchtwo();
    void setup();
    void refreshone();
    void refreshtwo();
private:
    Ui::PaymentInquiryWidget *ui;
    DatabaseManager db;
    QTableWidget* table;
    QTableWidget* table2;
    QTabWidget *tabWidget;
    void refreshTable();
    void setFixedRowHeight(QTableWidget *table, int height);

};
#endif // PAYMENTINQUIRYWIDGET_H
