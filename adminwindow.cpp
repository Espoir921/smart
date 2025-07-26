#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "databasemanager.h"

#include <QMenuBar>
#include <QTimer>
#include <QDateTime>
#include <QHBoxLayout>
#include <QWidget>
#include <QVBoxLayout>
#include <QMessageBox>

AdminWindow::AdminWindow(int userID, QWidget *parent) :
    QMainWindow(parent), my_userId(userID), ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    setWindowTitle("物业管理中心");
    ui->menubar->hide();
    setupUI();
    setupStyles();
    setupStatusBar();
    navigationTree->setCurrentItem(navigationTree->topLevelItem(0)->child(0));
    stackedWidget->setCurrentWidget(staffWidget);
}

AdminWindow::~AdminWindow()
{
    delete ui;
}

void AdminWindow::setupUI()
{
    // 1. 左侧导航栏
    navigationTree = new QTreeWidget(this);
    navigationTree->setHeaderHidden(true);

    // 人事管理主菜单
    QTreeWidgetItem *personnelManagement = new QTreeWidgetItem(navigationTree);
    personnelManagement->setText(0, "人事管理");

    // 人事管理子菜单
    QTreeWidgetItem *staffManage = new QTreeWidgetItem(personnelManagement);
    staffManage->setText(0, "物业工作人员信息维护");
    QTreeWidgetItem *leaveApprove = new QTreeWidgetItem(personnelManagement);
    leaveApprove->setText(0, "请销假审批");
    QTreeWidgetItem *attendance = new QTreeWidgetItem(personnelManagement);
    attendance->setText(0, "考勤记录管理");
    QTreeWidgetItem *scheduling = new QTreeWidgetItem(personnelManagement);
    scheduling->setText(0, "排班管理");
    QTreeWidgetItem *training = new QTreeWidgetItem(personnelManagement);
    training->setText(0, "培训记录");

    // 系统信息设置主菜单
    QTreeWidgetItem *systemSettings = new QTreeWidgetItem(navigationTree);
    systemSettings->setText(0, "系统信息设置");

    // 系统信息设置子菜单
    QTreeWidgetItem *houseInfo = new QTreeWidgetItem(systemSettings);
    houseInfo->setText(0, "房屋信息管理");
    QTreeWidgetItem *facilityInfo = new QTreeWidgetItem(systemSettings);
    facilityInfo->setText(0, "公共设施设置");
    QTreeWidgetItem *parkingSpace = new QTreeWidgetItem(systemSettings);
    parkingSpace->setText(0, "停车位分配");
    QTreeWidgetItem *chargingStandard = new QTreeWidgetItem(systemSettings);
    chargingStandard->setText(0, "收费标准设置");

    // 财务中心主菜单
    QTreeWidgetItem *financialCenter = new QTreeWidgetItem(navigationTree);
    financialCenter->setText(0, "财务中心");

    // 收入明细分类（保留原有的收入明细页面）
    QTreeWidgetItem *incomeCategory = new QTreeWidgetItem(financialCenter);
    incomeCategory->setText(0, "收入明细");

    // 收入子菜单
    QTreeWidgetItem *incomeSummary = new QTreeWidgetItem(incomeCategory);
    incomeSummary->setText(0, "收入汇总"); // 原有的收入明细页面
    QTreeWidgetItem *dailyIncome = new QTreeWidgetItem(incomeCategory);
    dailyIncome->setText(0, "日收入明细");
    QTreeWidgetItem *monthlyIncome = new QTreeWidgetItem(incomeCategory);
    monthlyIncome->setText(0, "月收入明细");
    QTreeWidgetItem *yearlyIncome = new QTreeWidgetItem(incomeCategory);
    yearlyIncome->setText(0, "年收入明细");

    // 支出明细分类（保留原有的支出明细页面）
    QTreeWidgetItem *expenseCategory = new QTreeWidgetItem(financialCenter);
    expenseCategory->setText(0, "支出明细");

    // 支出子菜单
    QTreeWidgetItem *expenditureSummary = new QTreeWidgetItem(expenseCategory);
    expenditureSummary->setText(0, "支出汇总"); // 原有的支出明细页面
    QTreeWidgetItem *dailyExpense = new QTreeWidgetItem(expenseCategory);
    dailyExpense->setText(0, "日支出明细");
    QTreeWidgetItem *monthlyExpense = new QTreeWidgetItem(expenseCategory);
    monthlyExpense->setText(0, "月支出明细");
    QTreeWidgetItem *yearlyExpense = new QTreeWidgetItem(expenseCategory);
    yearlyExpense->setText(0, "年支出明细");

    // 生成费用报表
    QTreeWidgetItem *feeReport = new QTreeWidgetItem(financialCenter);
    feeReport->setText(0, "生成费用报表");

    // 用户反馈主菜单
    QTreeWidgetItem *userFeedback = new QTreeWidgetItem(navigationTree);
    userFeedback->setText(0, "用户反馈");

    // 用户反馈子菜单
    QTreeWidgetItem *complaintRecord = new QTreeWidgetItem(userFeedback);
    complaintRecord->setText(0, "投诉记录");

    // 展开所有菜单项
    for (int i = 0; i < navigationTree->topLevelItemCount(); ++i) {
        navigationTree->topLevelItem(i)->setExpanded(true);
    }
    // 特别展开财务分类
    incomeCategory->setExpanded(true);
    expenseCategory->setExpanded(true);

    // 导航项切换时触发内容页切换
    connect(navigationTree, &QTreeWidget::itemClicked,
            this, &AdminWindow::onNavigationItemChanged);

    // 2. 右侧内容区
    stackedWidget = new QStackedWidget(this);

    // 人事管理页面
    staffWidget = new StaffManageWidget(this);
    leaveWidget = new LeaveApproveWidget(this);
    leaveWidget->setUserId(my_userId);
    attendanceWidget = new AttendanceWidget(this);
    staffschedulingWidget = new StaffScheduling(this);
    staffschedulingWidget->setUserId(my_userId);
    trainingWiget = new Training(this);

    // 系统设置页面
    houseInfoWidget = new HouseInformationWidget(this);
    facilityInfoWidget = new FacilityInformationWidget(this);
    parkingSpaceWidget = new ParkingSpaceAllocationWidget(this);
    chargingStandardSettingWidget = new ChargingStandardSettingWidget(this);

    // 投诉页面
    complaintPage = new ComplaintRecordPage(this);
    complaintPage->setUserId(my_userId);

    // 财务页面
    feeReportPage = new FeeReportPage(this);
    incomePage = new IncomeDetailPage(this); // 原有的收入明细页面
    expenditurePage = new ExpenditureDetailPage(this); // 原有的支出明细页面
    dailyIncomePage = new DailyIncomeDetailPage();
    monthlyIncomePage = new MonthlyIncomeDetailPage();
    yearlyIncomePage = new YearlyIncomeDetailPage();
    dailyExpensePage = new DailyExpenseDetailPage();
    monthlyExpensePage = new MonthlyExpenseDetailPage();
    yearlyExpensePage = new YearlyExpenseDetailPage();

    // 将页面添加到内容区
    stackedWidget->addWidget(staffWidget);
    stackedWidget->addWidget(leaveWidget);
    stackedWidget->addWidget(attendanceWidget);
    stackedWidget->addWidget(staffschedulingWidget);
    stackedWidget->addWidget(trainingWiget);
    stackedWidget->addWidget(houseInfoWidget);
    stackedWidget->addWidget(facilityInfoWidget);
    stackedWidget->addWidget(parkingSpaceWidget);
    stackedWidget->addWidget(chargingStandardSettingWidget);
    stackedWidget->addWidget(complaintPage);
    stackedWidget->addWidget(feeReportPage);
    stackedWidget->addWidget(incomePage); // 原有收入明细页面
    stackedWidget->addWidget(expenditurePage); // 原有支出明细页面
    stackedWidget->addWidget(dailyIncomePage);
    stackedWidget->addWidget(monthlyIncomePage);
    stackedWidget->addWidget(yearlyIncomePage);
    stackedWidget->addWidget(dailyExpensePage);
    stackedWidget->addWidget(monthlyExpensePage);
    stackedWidget->addWidget(yearlyExpensePage);

    // 3. 主布局
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(navigationTree, 2);
    mainLayout->addWidget(stackedWidget, 9);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void AdminWindow::setupStyles()
{
    navigationTree->setStyleSheet(R"(
        QTreeWidget {
            background-color: #f2f8ff;
            color: black;
            border: none;
            padding: 10px 0;
            font-size: 24px;
            font-family: '楷体';
            font-weight: bold;
        }
        QTreeWidget::item {
            height: 50px;
            padding-left: 20px;
            font-size: 16px;
        }
        QTreeWidget::item:selected {
            background-color: #cbdeff;
            border-left: 4px solid #1e61b8;
            color: black;
        }
        QTreeWidget::item:hover:!selected {
            background-color: #4a5059;
        }
        QTreeWidget::branch:has-children:!has-siblings:closed,
        QTreeWidget::branch:closed:has-children:has-siblings {
            border-image: none;
            image: url(:/right_arrow.png);
        }
        QTreeWidget::branch:open:has-children:!has-siblings,
        QTreeWidget::branch:open:has-children:has-siblings  {
            border-image: none;
            image: url(:/down_arrow.png);
        }
    )");

    stackedWidget->setStyleSheet(R"(
        QStackedWidget {
            background-color: #ffffff;
            border: none;
        }
    )");

    setStyleSheet("QMainWindow { background-color: #f0f0f0; }");
}

void AdminWindow::setupStatusBar()
{
    userLabel = new QLabel(QString("当前操作人员：%1").arg(my_userId), this);
    timeLabel = new QLabel(this);
    ui->statusbar->addPermanentWidget(userLabel);
    ui->statusbar->addPermanentWidget(timeLabel);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    });
    timer->start(1000);
}

void AdminWindow::executeDbOperationsAndShowPage(int index)
{
    // 在显示财务页面时执行数据库操作
    if (stackedWidget->widget(index) == incomePage ||
        stackedWidget->widget(index) == expenditurePage ||
        stackedWidget->widget(index) == dailyIncomePage ||
        stackedWidget->widget(index) == monthlyIncomePage ||
        stackedWidget->widget(index) == yearlyIncomePage ||
        stackedWidget->widget(index) == dailyExpensePage ||
        stackedWidget->widget(index) == monthlyExpensePage ||
        stackedWidget->widget(index) == yearlyExpensePage)
    {
        DatabaseManager db;
        if (db.open("try.db")) {
            db.processParkingSpaceData();
            db.transferAttendanceToExpense();
            db.close();
        } else {
            QMessageBox::critical(this, "数据库错误", "无法打开数据库");
            return;
        }
    }
    stackedWidget->setCurrentIndex(index);
}

void AdminWindow::onNavigationItemChanged(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    // 人事管理
    if (item->text(0) == "物业工作人员信息维护") {
        stackedWidget->setCurrentWidget(staffWidget);
    } else if (item->text(0) == "请销假审批") {
        stackedWidget->setCurrentWidget(leaveWidget);
    } else if (item->text(0) == "考勤记录管理") {
        stackedWidget->setCurrentWidget(attendanceWidget);
    } else if (item->text(0) == "排班管理") {
        stackedWidget->setCurrentWidget(staffschedulingWidget);
    } else if (item->text(0) == "培训记录") {
        stackedWidget->setCurrentWidget(trainingWiget);
    }

    // 系统设置
    else if (item->text(0) == "房屋信息管理") {
        stackedWidget->setCurrentWidget(houseInfoWidget);
    } else if (item->text(0) == "公共设施设置") {
        stackedWidget->setCurrentWidget(facilityInfoWidget);
    } else if (item->text(0) == "停车位分配") {
        stackedWidget->setCurrentWidget(parkingSpaceWidget);
    } else if (item->text(0) == "收费标准设置") {
        stackedWidget->setCurrentWidget(chargingStandardSettingWidget);
    }

    // 财务中心 - 收入明细
    else if (item->text(0) == "收入汇总") {
        executeDbOperationsAndShowPage(stackedWidget->indexOf(incomePage));
    } else if (item->text(0) == "日收入明细") {
        executeDbOperationsAndShowPage(stackedWidget->indexOf(dailyIncomePage));
    } else if (item->text(0) == "月收入明细") {
        executeDbOperationsAndShowPage(stackedWidget->indexOf(monthlyIncomePage));
    } else if (item->text(0) == "年收入明细") {
        executeDbOperationsAndShowPage(stackedWidget->indexOf(yearlyIncomePage));
    }

    // 财务中心 - 支出明细
    else if (item->text(0) == "支出汇总") {
        executeDbOperationsAndShowPage(stackedWidget->indexOf(expenditurePage));
    } else if (item->text(0) == "日支出明细") {
        executeDbOperationsAndShowPage(stackedWidget->indexOf(dailyExpensePage));
    } else if (item->text(0) == "月支出明细") {
        executeDbOperationsAndShowPage(stackedWidget->indexOf(monthlyExpensePage));
    } else if (item->text(0) == "年支出明细") {
        executeDbOperationsAndShowPage(stackedWidget->indexOf(yearlyExpensePage));
    }

    // 财务中心 - 报表
    else if (item->text(0) == "生成费用报表") {
        stackedWidget->setCurrentWidget(feeReportPage);
    }

    // 用户反馈
    else if (item->text(0) == "投诉记录") {
        stackedWidget->setCurrentWidget(complaintPage);
    }
}
