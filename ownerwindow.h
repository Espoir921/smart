#ifndef OWNERWINDOW_H
#define OWNERWINDOW_H
#include "ui_ownerwindow.h"
#include "databasemanager.h"
#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QTimer>

// 前置声明
class NoticeWidget;
class PublicityWidget;
class WarningWidget;
class InitiativeWidget;
class CarRegistration;
class MyHome;
class Parking;
class Maintenance;
class Complaints;
class Evaluation;
class Bill;
class History;

QT_BEGIN_NAMESPACE
namespace Ui { class OwnerWindow; }
QT_END_NAMESPACE

class OwnerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OwnerWindow(int id, QWidget *parent = nullptr);
    ~OwnerWindow();

private:
    Ui::OwnerWindow *ui;
    int m_uid; // 业主ID
    DatabaseManager m_db;
    void loadAnnouncements(const QString& type);
    void initLayout();

    // 状态栏组件
    QLabel* m_userLabel;
    QLabel* m_timeLabel;
    QTimer* m_timer;

    // 导航与内容组件
    QTreeWidget* m_mainNavTree; // 左侧主导航
    QStackedWidget* m_mainContentStack; // 主内容区域

    // "公告"相关
    NoticeWidget* m_noticeWidget;      // 通知页面
    PublicityWidget* m_publicityWidget;// 公示页面
    WarningWidget* m_warningWidget;    // 警示页面
    InitiativeWidget* m_initiativeWidget; // 倡议页面
    // 新增：为每个公告子页面绑定独立的列表控件
    QListWidget* m_noticeList;    // 通知列表
    QListWidget* m_publicityList; // 公示列表
    QListWidget* m_warningList;  // 警示列表
    QListWidget* m_initiativeList;// 倡议列表

    // 新增页面
    CarRegistration* m_vehicleRegistrationWidget;
    MyHome* m_ownerInfoWidget;
    Parking* m_parkingSpaceQueryWidget;
    Maintenance* m_onlineRepairWidget;
    Complaints* m_complaintSuggestionWidget;
    Evaluation* m_serviceEvaluationWidget;
    Bill* m_onlinePaymentWidget;
    History* m_paymentRecordWidget;

    void initStatusBar();
    void connectSignals();

private slots:
    void onMainNavChanged(QTreeWidgetItem* item, int column);
  //  void onAnnouncementSubNavClicked(QTreeWidgetItem* item, int column);
    void updateTime();
};

#endif // OWNERWINDOW_H
