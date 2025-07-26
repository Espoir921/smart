#include "staffwindow.h"
#include "ui_staffwindow.h"
#include "attendancecheckwidget.h"
#include "houserenterwidget.h"
#include "repairworkorderwidget.h"
#include "visitorregistrationwidget.h"
#include "paymentinquirywidget.h"
#include "announcementswidget.h"
#include "rosterquerywidget.h"
#include "staffleaveswidget.h"

#include "carrenterwidget.h"
#include "carrecordwidget.h"
#include <QStackedWidget>
#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QHBoxLayout>
#include <QWidget>
#include <QDebug>

StaffWindow::StaffWindow(int userId, QWidget *parent)
    : QMainWindow(parent)
   , m_userId(userId)
   , ui(new Ui::StaffWindow)
{
    ui->setupUi(this);

    setWindowTitle("物业工作人员管理平台");
    ui->menubar->hide();

   setupNavigationAndContent();//初始化左侧导航栏、右侧内容区布局
   setupStatusBar();//初始化状态栏
    setupStyles();//初始化样式（QSS 美化导航栏、内容区等）
}

StaffWindow::~StaffWindow()
{
    delete ui;
}


void StaffWindow::setupNavigationAndContent()
{

    navigationTree = new QTreeWidget(this); //左侧导航栏
    navigationTree->setHeaderHidden(true); // 隐藏默认表头
    QTreeWidgetItem *dailyWorkMenu = new QTreeWidgetItem(navigationTree);
    dailyWorkMenu->setText(0, "日常工作");

    QTreeWidgetItem *actAttendanceCheck = new QTreeWidgetItem(dailyWorkMenu);
    actAttendanceCheck->setText(0, "出勤打卡");
    QTreeWidgetItem *actHouseRenter = new QTreeWidgetItem(dailyWorkMenu);
    actHouseRenter->setText(0, "房屋出租管理");
    QTreeWidgetItem *actRepairWorkOrder = new QTreeWidgetItem(dailyWorkMenu);
    actRepairWorkOrder->setText(0, "报修工单处理");

    QTreeWidgetItem *ownerServiceMenu = new QTreeWidgetItem(navigationTree);
    ownerServiceMenu->setText(0, "业主服务");

    QTreeWidgetItem *actVisitorRegistration = new QTreeWidgetItem(ownerServiceMenu);
    actVisitorRegistration->setText(0, "访客登记");
    QTreeWidgetItem *actPaymentInquiry = new QTreeWidgetItem(ownerServiceMenu);
    actPaymentInquiry->setText(0, "缴费查询");

    QTreeWidgetItem *personalCenterMenu = new QTreeWidgetItem(navigationTree);
    personalCenterMenu->setText(0, "个人中心");
    QTreeWidgetItem *actRosterQuery = new QTreeWidgetItem(personalCenterMenu);
    actRosterQuery->setText(0, "排班查询");
    QTreeWidgetItem *actAnnouncements = new QTreeWidgetItem(personalCenterMenu);
    actAnnouncements->setText(0, "通知公告");
    QTreeWidgetItem *actStaffLeaves = new QTreeWidgetItem(personalCenterMenu);
    actStaffLeaves->setText(0, "请销假");


    QTreeWidgetItem *vehicleRecordMenu = new QTreeWidgetItem(navigationTree);
    vehicleRecordMenu->setText(0, "车辆记录");
    QTreeWidgetItem *actCarRenter = new QTreeWidgetItem(vehicleRecordMenu);
    actCarRenter->setText(0, "车辆登记");
    QTreeWidgetItem *actCarRecord = new QTreeWidgetItem(vehicleRecordMenu);
    actCarRecord->setText(0, "车辆进出记录");

    stackedWidget = new QStackedWidget(this);//右侧内容区
    attendancecheck = new AttendanceCheckWidget(this);
    houserenter = new HouseRenterWidget(this);

    repairworkorder = new RepairWorkOrderWidget(this);
    visitorRegistrationWidget = new VisitorRegistrationWidget(this);
    paymentInquiryWidget = new PaymentInquiryWidget(this);


    rosterQueryWidget = new RosterQueryWidget(this);
    announcementsWidget = new AnnouncementsWidget(this);
    staffLeavesWidget = new StaffLeavesWidget(this);

    carRenterWidget = new CarRenterWidget(this);
    carRecordWidget = new CarRecordWidget(this);

    stackedWidget->addWidget(attendancecheck);
    stackedWidget->addWidget(houserenter);
    stackedWidget->addWidget(repairworkorder);
    stackedWidget->addWidget(visitorRegistrationWidget);
    stackedWidget->addWidget(paymentInquiryWidget);

    stackedWidget->addWidget(rosterQueryWidget);
    stackedWidget->addWidget(announcementsWidget);
    stackedWidget->addWidget(staffLeavesWidget);
    stackedWidget->addWidget(carRenterWidget);
    stackedWidget->addWidget(carRecordWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout();//左侧导航、右侧内容
    mainLayout->addWidget(navigationTree, 1);
    mainLayout->addWidget(stackedWidget, 5);

    QWidget *centralContainer = new QWidget(this);
    centralContainer->setLayout(mainLayout);
    setCentralWidget(centralContainer);

    attendancecheck->setUserId(m_userId);
    staffLeavesWidget->setUserId(m_userId);
    carRenterWidget->setUserId(m_userId);

    connect(navigationTree, &QTreeWidget::itemClicked, this, &StaffWindow::onNavigationItemClicked);
}

void StaffWindow::onNavigationItemClicked(QTreeWidgetItem *item)
{

    QString text = item->text(0);
    if (text == "出勤打卡") {
        stackedWidget->setCurrentWidget(attendancecheck);
    } else if (text == "房屋出租管理") {
        stackedWidget->setCurrentWidget(houserenter);
    } else if (text == "报修工单处理") {
        stackedWidget->setCurrentWidget(repairworkorder);
    } else if (text == "访客登记") {
        stackedWidget->setCurrentWidget(visitorRegistrationWidget);
    } else if (text == "缴费查询") {
        stackedWidget->setCurrentWidget(paymentInquiryWidget);
    }
      else if (text == "排班查询") {
        stackedWidget->setCurrentWidget(rosterQueryWidget);
    } else if (text == "通知公告") {
        stackedWidget->setCurrentWidget(announcementsWidget);
    } else if (text == "请销假") {
        stackedWidget->setCurrentWidget(staffLeavesWidget);
    }  else if (text == "车辆登记") {
        stackedWidget->setCurrentWidget(carRenterWidget);
    } else if (text == "车辆进出记录") {
        stackedWidget->setCurrentWidget(carRecordWidget);
    }
}

void StaffWindow::setupStatusBar()
{
    userlabel = new QLabel(QString("当前操作人员：%1").arg(m_userId), this);
    timelabel = new QLabel(this);

    ui->statusbar->addPermanentWidget(userlabel);
    ui->statusbar->addPermanentWidget(timelabel);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        timelabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    });
    timer->start(1000);

    stackedWidget->setCurrentWidget(attendancecheck);
}

void StaffWindow::setupStyles()
{

      navigationTree->setStyleSheet(R"(
          QTreeWidget {
              background-color: #f2f8ff; /* 浅色背景 */
              color: black; /* 文字黑色 */
              border: none; /* 无边框 */
              padding: 10px 0;
              font-size: 24px; /* 设置导航栏整体字号 */
              font-family: '楷体'; /*设置字体*/
              font-weight: bold; /*设置字体加粗*/
          }
          QTreeWidget::item {
              height: 50px; /* 项高度 */
              padding-left: 20px; /* 文字左缩进 */
              font-size: 16px; /* 设置导航栏每个项的字号 */
          }
          QTreeWidget::item:selected {
              background-color: #cbdeff ; /* 选中项高亮（蓝色） */
              border-left: 4px solid #1e61b8; /* 左侧高亮边 */
              color: black; /* 文字黑色 */
          }
          QTreeWidget::item:hover:!selected {
              background-color: #4a5059; /* hover效果 */
          }
          QTreeWidget::branch:has-children:!has-siblings:closed,
          QTreeWidget::branch:closed:has-children:has-siblings {
              border-image: none;
              image: url(:/right_arrow.png); /* 可替换为实际图标路径 */
          }
          QTreeWidget::branch:open:has-children:!has-siblings,
          QTreeWidget::branch:open:has-children:has-siblings  {
              border-image: none;
              image: url(:/down_arrow.png); /* 可替换为实际图标路径 */
          }
      )");
      stackedWidget->setStyleSheet(R"(
          QStackedWidget {
              background-color: #ffffff; /* 白色背景 */
              border: none; /* 无边框 */
          }
      )");

      setStyleSheet("QMainWindow { background-color: #f0f0f0; }");

}

void StaffWindow::showattendancecheck() {
       stackedWidget->setCurrentWidget(attendancecheck);
   }
   void StaffWindow::showhouserenter() {
       stackedWidget->setCurrentWidget(houserenter);
   }
   void StaffWindow::showrepairworkorder() {
       stackedWidget->setCurrentWidget(repairworkorder);
   }
   void StaffWindow::showVisitorRegistration() {
       stackedWidget->setCurrentWidget(visitorRegistrationWidget);
   }

   void StaffWindow::showPaymentInquiry() {
       stackedWidget->setCurrentWidget(paymentInquiryWidget);
   }

   void StaffWindow::showRosterQuery() {
       stackedWidget->setCurrentWidget(rosterQueryWidget);
   }

   void StaffWindow::showAnnouncements() {
       stackedWidget->setCurrentWidget(announcementsWidget);
   }

   void StaffWindow::showStaffLeaves() {
       stackedWidget->setCurrentWidget(staffLeavesWidget);
   }

   void StaffWindow::showCarRenter() {
       stackedWidget->setCurrentWidget(carRenterWidget);
   }

   void StaffWindow::showCarRecord() {
       stackedWidget->setCurrentWidget(carRecordWidget);
   }
