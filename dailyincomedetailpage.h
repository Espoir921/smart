
#ifndef DAILYINCOMEDETAILPAGE_H
#define DAILYINCOMEDETAILPAGE_H
#include <QWidget>
#include <QTableWidget>  // 改用表格组件展示汇总数据
#include <QChartView>    // 用于柱状图
#include <QBarSeries>    // 柱状图系列
#include <QVBoxLayout>
#include "databasemanager.h"
QT_CHARTS_USE_NAMESPACE  // 引入Qt图表命名空间

namespace Ui {
class DailyIncomeDetailPage;
}

class DailyIncomeDetailPage : public QWidget
{
    Q_OBJECT
public:
    explicit DailyIncomeDetailPage(QWidget *parent = nullptr);
    ~DailyIncomeDetailPage();
    QTableView* tableView;
    QTableWidget *summaryTable;  // 汇总表格（展示类型、总金额、记录数）
private:
    Ui::DailyIncomeDetailPage *ui;

    QChartView *chartView;       // 柱状图视图
    DatabaseManager db;          // 数据库管理器
    void initUI();               // 初始化界面（表格+图表）
    void loadSummaryData();      // 加载汇总数据（替代原明细数据加载）
};
#endif // DAILYINCOMEDETAILPAGE_H
