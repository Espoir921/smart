
#include "incomedetailpage.h"
#include "ui_incomedetailpage.h"
#include <QLabel>
#include <QHeaderView>
#include <QBarSet>
#include <QChart>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QtCharts/QBarSeries>
#include <QDebug>

IncomeDetailPage::IncomeDetailPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IncomeDetailPage) {
    ui->setupUi(this);
    initUI();
    loadSummaryData();
}

IncomeDetailPage::~IncomeDetailPage() {
    delete ui;
}

void IncomeDetailPage::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel("收入明细表", this);
    titleLabel->setStyleSheet("color: blue; font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    summaryTable = new QTableWidget(this);
    summaryTable->setColumnCount(3);
    summaryTable->setHorizontalHeaderLabels({"收入类型", "总金额(元)", "记录数"});
    summaryTable->horizontalHeader()->setStretchLastSection(true);
    summaryTable->verticalHeader()->setVisible(false);
    mainLayout->addWidget(summaryTable);

    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(220);
    mainLayout->addWidget(chartView);

    setLayout(mainLayout);
}

void IncomeDetailPage::loadSummaryData() {
    QStringList incomeTypes = {"物业费", "车位费"};
        QVector<double> sums(incomeTypes.size(), 0.0);
        QVector<int> counts(incomeTypes.size(), 0);
        double totalSum = 0.0;
        int totalCount = 0;

        DatabaseManager db;
        if (db.open("try.db")) {
            for (int i = 0; i < incomeTypes.size(); ++i) {
                QSqlQuery query(db.db);
                // 修正：移除多余的status条件（与支出明细逻辑统一）
                query.prepare("SELECT SUM(amount), COUNT(*) FROM income WHERE type=?");
                query.addBindValue(incomeTypes[i]);
                if (query.exec() && query.next()) {
                    sums[i] = query.value(0).toDouble();
                    counts[i] = query.value(1).toInt();
                    totalSum += sums[i];
                    totalCount += counts[i];
                } else {
                    qDebug() << "Income query error:" << query.lastError().text(); // 调试输出
                }
            }
            db.close();
        }

    // 更新表格
    summaryTable->setRowCount(incomeTypes.size() + 1);
    for (int i = 0; i < incomeTypes.size(); ++i) {
        summaryTable->setItem(i, 0, new QTableWidgetItem(incomeTypes[i]));
        summaryTable->setItem(i, 1, new QTableWidgetItem(QString::number(sums[i], 'f', 2)));
        summaryTable->setItem(i, 2, new QTableWidgetItem(QString::number(counts[i])));
    }
    int totalRow = incomeTypes.size();
    summaryTable->setItem(totalRow, 0, new QTableWidgetItem("总计"));
    summaryTable->setItem(totalRow, 1, new QTableWidgetItem(QString::number(totalSum, 'f', 2)));
    summaryTable->setItem(totalRow, 2, new QTableWidgetItem(QString::number(totalCount)));

    // 更新图表
    QBarSet *set = new QBarSet("收入总金额");
    for (double sum : sums) *set << sum;
    set->setColor(QColor("#1976d2"));

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("各类收入总额统计");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(incomeTypes);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("金额(元)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView->setChart(chart);
}
