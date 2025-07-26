
#include "dailyexpensedetailpage.h"
#include "ui_dailyexpensedetailpage.h"
#include <QLabel>
#include <QHeaderView>
#include <QBarSet>
#include <QChart>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QtCharts/QBarSeries>
#include <QDebug>

DailyExpenseDetailPage::DailyExpenseDetailPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DailyExpenseDetailPage) {
    ui->setupUi(this);
    initUI();
    loadSummaryData();
}

DailyExpenseDetailPage::~DailyExpenseDetailPage() {
    delete ui;
}

void DailyExpenseDetailPage::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel("日支出明细表", this);
    titleLabel->setStyleSheet("color: blue; font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    summaryTable = new QTableWidget(this);
    summaryTable->setColumnCount(3);
    summaryTable->setHorizontalHeaderLabels({"支出类型", "总金额(元)", "记录数"});
    summaryTable->horizontalHeader()->setStretchLastSection(true);
    summaryTable->verticalHeader()->setVisible(false);
    mainLayout->addWidget(summaryTable);

    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(220);
    mainLayout->addWidget(chartView);

    setLayout(mainLayout);
}

void DailyExpenseDetailPage::loadSummaryData() {
    QStringList expenseTypes = {"工资", "维修费"};
    QVector<double> sums(expenseTypes.size(), 0.0);
    QVector<int> counts(expenseTypes.size(), 0);
    double totalSum = 0.0;
    int totalCount = 0;

    DatabaseManager db;
    if (db.open("try.db")) {
        QDate yesterday = QDate::currentDate().addDays(-1);

        for (int i = 0; i < expenseTypes.size(); ++i) {
                QSqlQuery query(db.db);
                // 修正：统一使用命名占位符
                query.prepare("SELECT SUM(amount), COUNT(*) FROM expense WHERE type=:type AND date = :date");
                query.bindValue(":type", expenseTypes[i]);
                query.bindValue(":date", yesterday);
                if (query.exec() && query.next()) {
                    sums[i] = query.value(0).toDouble();
                    counts[i] = query.value(1).toInt();
                    totalSum += sums[i];
                    totalCount += counts[i];
                } else {
                    qDebug() << "Expense query error:" << query.lastError().text();
                }
            }
        db.close();
    }

    // 更新表格
    summaryTable->setRowCount(expenseTypes.size() + 1);
    for (int i = 0; i < expenseTypes.size(); ++i) {
        summaryTable->setItem(i, 0, new QTableWidgetItem(expenseTypes[i]));
        summaryTable->setItem(i, 1, new QTableWidgetItem(QString::number(sums[i], 'f', 2)));
        summaryTable->setItem(i, 2, new QTableWidgetItem(QString::number(counts[i])));
    }
    int totalRow = expenseTypes.size();
    summaryTable->setItem(totalRow, 0, new QTableWidgetItem("总计"));
    summaryTable->setItem(totalRow, 1, new QTableWidgetItem(QString::number(totalSum, 'f', 2)));
    summaryTable->setItem(totalRow, 2, new QTableWidgetItem(QString::number(totalCount)));

    // 更新图表
    QBarSet *set = new QBarSet("支出总金额");
    for (double sum : sums) *set << sum;
    set->setColor(QColor("#e53935"));

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("各类日支出总额统计");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(expenseTypes);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("金额(元)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView->setChart(chart);
}
