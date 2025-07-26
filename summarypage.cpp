#include "summarypage.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QCheckBox>
#include <QMessageBox>

SummaryPage::SummaryPage(QWidget *parent) : QWidget(parent) {
    initUI();
    refreshData();
}

void SummaryPage::initUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 顶部标题和刷新按钮
    QHBoxLayout* topLayout = new QHBoxLayout();
    QLabel* title = new QLabel("数据汇总", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");
    refreshBtn = new QPushButton("刷新", this);
    refreshBtn->setStyleSheet("background-color: #4CAF50; color: white; padding: 6px 12px;");
    connect(refreshBtn, &QPushButton::clicked, this, &SummaryPage::refreshData);
    topLayout->addWidget(title);
    topLayout->addStretch();
    topLayout->addWidget(refreshBtn);
    mainLayout->addLayout(topLayout);

    // 中间内容区：投诉表格和统计
    QHBoxLayout* contentLayout = new QHBoxLayout();

    // 投诉表格（4列：ID, 内容, 时间, 已处理）
    complaintTable = new QTableWidget(this);
    complaintTable->setColumnCount(4);
    complaintTable->setHorizontalHeaderLabels({"ID", "内容", "时间", "已处理"});
    complaintTable->horizontalHeader()->setStretchLastSection(true);
    contentLayout->addWidget(complaintTable, 2);

    // 统计面板
    QVBoxLayout* statsLayout = new QVBoxLayout();
    statsLayout->setContentsMargins(15, 0, 15, 0);

    QLabel* incomeTitle = new QLabel("收入统计", this);
    incomeTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-top: 10px;");
    propertyFee = new QLabel("物业费：0.00", this);
    parkingFee = new QLabel("车位费：0.00", this);

    QLabel* expenseTitle = new QLabel("支出统计", this);
    expenseTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-top: 20px;");
    salary = new QLabel("工资：0.00", this);
    maintenance = new QLabel("维修费：0.00", this);

    statsLayout->addWidget(incomeTitle);
    statsLayout->addWidget(propertyFee);
    statsLayout->addWidget(parkingFee);
    statsLayout->addWidget(expenseTitle);
    statsLayout->addWidget(salary);
    statsLayout->addWidget(maintenance);
    statsLayout->addStretch();
    contentLayout->addLayout(statsLayout, 1);

    mainLayout->addLayout(contentLayout);

    // 底部时间戳
    QLabel* timeLabel = new QLabel(this);
    timeLabel->setStyleSheet("color: #666; margin-top: 10px;");
    timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    mainLayout->addWidget(timeLabel);
}

void SummaryPage::loadComplaints() {
    DatabaseManager db;
    if (db.open("try.db")) {
        QList<QVariantList> complaints = db.getComplaints();
        complaintTable->setRowCount(complaints.size());

        for (int i = 0; i < complaints.size(); ++i) {
            const QVariantList& row = complaints[i];
            complaintTable->setItem(i, 0, new QTableWidgetItem(row[0].toString()));
            complaintTable->setItem(i, 1, new QTableWidgetItem(row[1].toString()));
            complaintTable->setItem(i, 2, new QTableWidgetItem(row[2].toString()));

            QCheckBox* resolved = new QCheckBox();
            resolved->setChecked(row[3].toBool());
            complaintTable->setCellWidget(i, 3, resolved);

            QCheckBox* handled = new QCheckBox();
            handled->setChecked(row[4].toBool());
            complaintTable->setCellWidget(i, 4, handled);
        }
        db.close();
    } else {
        QMessageBox::critical(this, "错误", "无法连接投诉数据库！");
    }
}

void SummaryPage::calculateStats() {
    DatabaseManager db;

    // 收入统计（物业费、车位费）
    if (db.open("try.db")) {
        propertyFee->setText(QString("物业费：%.2f").arg(db.getIncomeSum("物业费")));
        parkingFee->setText(QString("车位费：%.2f").arg(db.getIncomeSum("车位收费")));
        db.close();
    }

    // 支出统计（工资、维修费）
    if (db.open("try.db")) {
        salary->setText(QString("工资：%.2f").arg(db.getExpenseSum("工资")));
        maintenance->setText(QString("维修费：%.2f").arg(db.getExpenseSum("维修费")));
        db.close();
    }
}

void SummaryPage::refreshData() {
    loadComplaints();
    calculateStats();
}
