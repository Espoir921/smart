
#ifndef DAILYEXPENSEDETAILPAGE_H
#define DAILYEXPENSEDETAILPAGE_H
#include <QWidget>
#include <QTableWidget>
#include <QChartView>
#include <QBarSeries>
#include <QVBoxLayout>
#include "databasemanager.h"
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class DailyExpenseDetailPage;
}

class DailyExpenseDetailPage : public QWidget
{
    Q_OBJECT
public:
    explicit DailyExpenseDetailPage(QWidget *parent = nullptr);
    ~DailyExpenseDetailPage();
    QTableView* tableView;
    QTableWidget *summaryTable;

private:
    Ui::DailyExpenseDetailPage *ui;
    QChartView *chartView;
    DatabaseManager db;
    void initUI();
    void loadSummaryData();
};
#endif // DAILYEXPENSEDETAILPAGE_H
