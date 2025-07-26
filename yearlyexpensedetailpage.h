
#ifndef YEARLYEXPENSEDETAILPAGE_H
#define YEARLYEXPENSEDETAILPAGE_H
#include <QWidget>
#include <QTableWidget>
#include <QChartView>
#include <QBarSeries>
#include <QVBoxLayout>
#include "databasemanager.h"
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class YearlyExpenseDetailPage;
}

class YearlyExpenseDetailPage : public QWidget
{
    Q_OBJECT
public:
    explicit YearlyExpenseDetailPage(QWidget *parent = nullptr);
    ~YearlyExpenseDetailPage();
    QTableView* tableView;
    QTableWidget *summaryTable;

private:
    Ui::YearlyExpenseDetailPage *ui;
    QChartView *chartView;
    DatabaseManager db;
    void initUI();
    void loadSummaryData();
};
#endif // YEARLYEXPENSEDETAILPAGE_H
