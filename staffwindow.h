#ifndef STAFFWINDOW_H
#define STAFFWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include <QTreeWidget>       //用于左侧导航栏
#include <QTreeWidgetItem>   //导航树节点

QT_BEGIN_NAMESPACE
namespace Ui {
class StaffWindow;
}
QT_END_NAMESPACE

class AttendanceCheckWidget;
class HouseRenterWidget;
class RepairWorkOrderWidget;
class VisitorRegistrationWidget;
class PaymentInquiryWidget;
class AnnouncementsWidget;

class RosterQueryWidget;
class AnnouncementsWidget;
class StaffLeavesWidget;

class CarRenterWidget;
class CarRecordWidget;

class StaffWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StaffWindow(int userId, QWidget *parent = nullptr);
    ~StaffWindow();

private slots:
    void showattendancecheck();//出勤打卡
    void showhouserenter();//房屋出租管理
    void showrepairworkorder();//报修工单处理
    void showVisitorRegistration();    // 访客登记
    void showPaymentInquiry();         // 缴费查询


    void showRosterQuery();            // 排班查询
    void showAnnouncements();          // 通知公告
    void showStaffLeaves();            // 请销假

    void showCarRenter();              // 车位出租// 车辆登记
    void showCarRecord();              // 车辆进出记录
    void onNavigationItemClicked(QTreeWidgetItem *item);// 左侧导航栏点击事件的槽函数

private:
    void setupMenus();

  //布局、样式、状态栏相关初始化函数声明
    void setupNavigationAndContent(); // 搭建左侧导航 + 右侧内容区
    void setupStatusBar();            // 状态栏初始化（原逻辑迁移）
    void setupStyles();               // QSS 样式美化

    int m_userId;
    QLabel* userlabel;
    QLabel* timelabel;
    QTimer* timer;

    Ui::StaffWindow *ui;
    QStackedWidget* stackedWidget;
    // 左侧导航栏成员
    QTreeWidget* navigationTree;
    // 各个功能页面指针（和之前保持一致）
    AttendanceCheckWidget* attendancecheck;
    HouseRenterWidget* houserenter;
    RepairWorkOrderWidget* repairworkorder;
    VisitorRegistrationWidget* visitorRegistrationWidget;
    PaymentInquiryWidget* paymentInquiryWidget;

    RosterQueryWidget* rosterQueryWidget;
    AnnouncementsWidget* announcementsWidget;
    StaffLeavesWidget* staffLeavesWidget;


    CarRenterWidget* carRenterWidget;
    CarRecordWidget* carRecordWidget;
};

#endif // STAFFWINDOW_H
