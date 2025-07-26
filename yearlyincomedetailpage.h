
#ifndef YEARLYINCOMEDETAILPAGE_H
#define YEARLYINCOMEDETAILPAGE_H
#include <QWidget>
#include <QTableWidget>
#include <QChartView>
#include <QBarSeries>
#include <QVBoxLayout>
#include "databasemanager.h"
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class YearlyIncomeDetailPage;
}

class YearlyIncomeDetailPage : public QWidget
{
    Q_OBJECT
public:
    explicit YearlyIncomeDetailPage(QWidget *parent = nullptr);
    ~YearlyIncomeDetailPage();
    QTableView* tableView;
    QTableWidget *summaryTable;

private:
    Ui::YearlyIncomeDetailPage *ui;
    QChartView *chartView;
    DatabaseManager db;
    void initUI();
    void loadSummaryData();
};
#endif // YEARLYINCOMEDETAILPAGE_H
