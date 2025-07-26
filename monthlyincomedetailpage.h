
#ifndef MONTHLYINCOMEDETAILPAGE_H
#define MONTHLYINCOMEDETAILPAGE_H
#include <QWidget>
#include <QTableWidget>
#include <QChartView>
#include <QBarSeries>
#include <QVBoxLayout>
#include "databasemanager.h"
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MonthlyIncomeDetailPage;
}

class MonthlyIncomeDetailPage : public QWidget
{
    Q_OBJECT
public:
    explicit MonthlyIncomeDetailPage(QWidget *parent = nullptr);
    ~MonthlyIncomeDetailPage();
    QTableView* tableView;
    QTableWidget *summaryTable;

private:
    Ui::MonthlyIncomeDetailPage *ui;
    QChartView *chartView;
    DatabaseManager db;
    void initUI();
    void loadSummaryData();
};
#endif // MONTHLYINCOMEDETAILPAGE_H
