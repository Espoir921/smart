#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QTimer>
#include "staffmanagewidget.h"
#include "leaveapprovewidget.h"
#include "attendancewidget.h"
#include "training.h"
#include "staffscheduling.h"
#include "houseinformationwidget.h"
#include "facilityinformationwidget.h"
#include "parkingspaceallocationwidget.h"
#include "chargingstandardsettingwidget.h"
#include "incomedetailpage.h" // 保留原有的收入明细页面
#include "expendituredetailpage.h" // 保留原有的支出明细页面
#include "complaintrecordpage.h"
#include "feereportpage.h"
#include "dailyexpensedetailpage.h"
#include "dailyincomedetailpage.h"
#include "monthlyexpensedetailpage.h"
#include "monthlyincomedetailpage.h"
#include "yearlyexpensedetailpage.h"
#include "yearlyincomedetailpage.h"

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(int userID, QWidget *parent = nullptr);
    ~AdminWindow();

private slots:
    void onNavigationItemChanged(QTreeWidgetItem *item, int column);

private:
    void setupUI();
    void setupStyles();
    void setupStatusBar();
    void executeDbOperationsAndShowPage(int index);

    int my_userId;
    Ui::AdminWindow *ui;
    QTreeWidget *navigationTree;
    QStackedWidget *stackedWidget;

    // 功能模块页面
    StaffManageWidget *staffWidget;
    LeaveApproveWidget *leaveWidget;
    AttendanceWidget *attendanceWidget;
    StaffScheduling *staffschedulingWidget;
    Training *trainingWiget;
    HouseInformationWidget *houseInfoWidget;
    FacilityInformationWidget *facilityInfoWidget;
    ParkingSpaceAllocationWidget *parkingSpaceWidget;
    ChargingStandardSettingWidget *chargingStandardSettingWidget;

    // 财务中心页面
    IncomeDetailPage *incomePage; // 保留原有的收入明细页面
    ExpenditureDetailPage *expenditurePage; // 保留原有的支出明细页面
    ComplaintRecordPage *complaintPage;
    FeeReportPage *feeReportPage;
    DailyIncomeDetailPage *dailyIncomePage;
    MonthlyIncomeDetailPage *monthlyIncomePage;
    YearlyIncomeDetailPage *yearlyIncomePage;
    DailyExpenseDetailPage *dailyExpensePage;
    MonthlyExpenseDetailPage *monthlyExpensePage;
    YearlyExpenseDetailPage *yearlyExpensePage;

    // 状态栏组件
    QLabel *userLabel;
    QLabel *timeLabel;
    QTimer *timer;
};

#endif // ADMINWINDOW_H
