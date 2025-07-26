
#ifndef MONTHLYEXPENSEDETAILPAGE_H
#define MONTHLYEXPENSEDETAILPAGE_H
#include <QWidget>
#include <QTableWidget>
#include <QChartView>
#include <QBarSeries>
#include <QVBoxLayout>
#include "databasemanager.h"
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MonthlyExpenseDetailPage;
}

class MonthlyExpenseDetailPage : public QWidget
{
    Q_OBJECT
public:
    explicit MonthlyExpenseDetailPage(QWidget *parent = nullptr);
    ~MonthlyExpenseDetailPage();
    QTableView* tableView;
    QTableWidget *summaryTable;

private:
    Ui::MonthlyExpenseDetailPage *ui;
    QChartView *chartView;
    DatabaseManager db;
    void initUI();
    void loadSummaryData();
};
#endif // MONTHLYEXPENSEDETAILPAGE_H
