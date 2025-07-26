
#ifndef EXPENDITUREDETAILPAGE_H
#define EXPENDITUREDETAILPAGE_H
#include <QWidget>
#include <QTableWidget>  // 汇总表格
#include <QChartView>    // 柱状图视图
#include <QVBoxLayout>
#include "databasemanager.h"
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class ExpenditureDetailPage;
}

class ExpenditureDetailPage : public QWidget
{
    Q_OBJECT
public:
    explicit ExpenditureDetailPage(QWidget *parent = nullptr);
    ~ExpenditureDetailPage();
    QTableView* tableView;
    QTableWidget *summaryTable;  // 汇总表格
private:
    Ui::ExpenditureDetailPage *ui;

    QChartView *chartView;       // 柱状图视图
    DatabaseManager db;          // 数据库管理器
    void initUI();               // 初始化界面
    void loadSummaryData();      // 加载汇总数据
};
#endif // EXPENDITUREDETAILPAGE_H
