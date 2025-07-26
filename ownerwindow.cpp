#include "ownerwindow.h"
#include "publicitywidget.h"
#include "noticewidget.h"
#include "warningwidget.h"
#include "initiativewidget.h"
#include "databasehelper.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidgetItem>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QStackedWidget>
#include <QListWidget>
#include <QSqlError>
#include <QTreeWidget>
#include "carregistration.h"
#include "MyHome.h"
#include "parking.h"
#include "maintenance.h"
#include "complaints.h"
#include "evaluation.h"
#include "bill.h"
#include "history.h"

OwnerWindow::OwnerWindow(int id, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OwnerWindow),
    m_uid(id)
{
    ui->setupUi(this);
    setWindowTitle("业主中心");

    // 打开公告数据库
    if (!m_db.openDB("try.db")) {
        QMessageBox::critical(this, "数据库错误", "无法打开公告数据库！");
    }

    // 初始化组件（含公告列表控件）
    m_noticeWidget = new NoticeWidget(this);
    m_publicityWidget = new PublicityWidget(this);
    m_warningWidget = new WarningWidget(this);
    m_initiativeWidget = new InitiativeWidget(this);

    // 初始化公告列表控件（关键修改：在OwnerWindow中创建并绑定到子页面）
    m_noticeList = new QListWidget(m_noticeWidget);
    m_publicityList = new QListWidget(m_publicityWidget);
    m_warningList = new QListWidget(m_warningWidget);
    m_initiativeList = new QListWidget(m_initiativeWidget);

    // 为子页面设置布局（将列表控件嵌入子页面）
    m_noticeWidget->setLayout(new QVBoxLayout());
    m_noticeWidget->layout()->addWidget(m_noticeList);
    m_publicityWidget->setLayout(new QVBoxLayout());
    m_publicityWidget->layout()->addWidget(m_publicityList);
    m_warningWidget->setLayout(new QVBoxLayout());
    m_warningWidget->layout()->addWidget(m_warningList);
    m_initiativeWidget->setLayout(new QVBoxLayout());
    m_initiativeWidget->layout()->addWidget(m_initiativeList);

    // 美化列表控件
    QList<QListWidget*> allLists = {m_noticeList, m_publicityList, m_warningList, m_initiativeList};
    for (QListWidget* list : allLists) {
        list->setStyleSheet(
            "QListWidget { border: 1px solid #eee; border-radius: 4px; padding: 8px; }"
            "QListWidget::item { padding: 6px; margin: 2px 0; border-bottom: 1px solid #f5f5f5; }"
            "QListWidget::item:hover { background-color: #f0f7ff; }"
        );
    }

    // 新增页面实例化
    m_vehicleRegistrationWidget = new CarRegistration(this);
    m_ownerInfoWidget = new MyHome(this);
    m_parkingSpaceQueryWidget = new Parking(this);
    m_onlineRepairWidget = new Maintenance(this);
    m_complaintSuggestionWidget = new Complaints(this);
    m_serviceEvaluationWidget = new Evaluation(this);
    m_onlinePaymentWidget = new Bill(this);
    m_paymentRecordWidget = new History(this);

    initLayout();
    initStatusBar();
    connectSignals();

    // 默认选中"通知"
    m_mainNavTree->setCurrentItem(m_mainNavTree->topLevelItem(0)->child(0));
    m_mainContentStack->setCurrentWidget(m_noticeWidget);
    loadAnnouncements("通知");

    m_ownerInfoWidget->setUserId(m_uid);
}

OwnerWindow::~OwnerWindow()
{
    delete ui;
}

// 初始化布局
void OwnerWindow::initLayout()
{
    // 中央部件与主布局
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 左侧主导航
    m_mainNavTree = new QTreeWidget(this);
    m_mainNavTree->setHeaderHidden(true);

    // 公告主菜单
    QTreeWidgetItem *announcementMenu = new QTreeWidgetItem(m_mainNavTree);
    announcementMenu->setText(0, "公告");

    // 公告子菜单
    QTreeWidgetItem *noticeItem = new QTreeWidgetItem(announcementMenu);
    noticeItem->setText(0, "通知");
    QTreeWidgetItem *publicityItem = new QTreeWidgetItem(announcementMenu);
    publicityItem->setText(0, "公示");
    QTreeWidgetItem *warningItem = new QTreeWidgetItem(announcementMenu);
    warningItem->setText(0, "警示");
    QTreeWidgetItem *initiativeItem = new QTreeWidgetItem(announcementMenu);
    initiativeItem->setText(0, "倡议");

    // 我的家园主菜单
    QTreeWidgetItem *myHomeMenu = new QTreeWidgetItem(m_mainNavTree);
    myHomeMenu->setText(0, "我的家园");

    // 我的家园子菜单
    QTreeWidgetItem *vehicleRegistrationItem = new QTreeWidgetItem(myHomeMenu);
    vehicleRegistrationItem->setText(0, "车辆登记");
    QTreeWidgetItem *ownerInfoItem = new QTreeWidgetItem(myHomeMenu);
    ownerInfoItem->setText(0, "业主信息");
    QTreeWidgetItem *parkingSpaceQueryItem = new QTreeWidgetItem(myHomeMenu);
    parkingSpaceQueryItem->setText(0, "车位查询");

    // 物业服务主菜单
    QTreeWidgetItem *propertyServiceMenu = new QTreeWidgetItem(m_mainNavTree);
    propertyServiceMenu->setText(0, "物业服务");

    // 物业服务子菜单
    QTreeWidgetItem *onlineRepairItem = new QTreeWidgetItem(propertyServiceMenu);
    onlineRepairItem->setText(0, "在线报修");
    QTreeWidgetItem *complaintSuggestionItem = new QTreeWidgetItem(propertyServiceMenu);
    complaintSuggestionItem->setText(0, "投诉建议");
    QTreeWidgetItem *serviceEvaluationItem = new QTreeWidgetItem(propertyServiceMenu);
    serviceEvaluationItem->setText(0, "服务评价");

    // 缴费中心主菜单
    QTreeWidgetItem *paymentCenterMenu = new QTreeWidgetItem(m_mainNavTree);
    paymentCenterMenu->setText(0, "缴费中心");

    // 缴费中心子菜单
    QTreeWidgetItem *onlinePaymentItem = new QTreeWidgetItem(paymentCenterMenu);
    onlinePaymentItem->setText(0, "在线缴费");
    QTreeWidgetItem *paymentRecordItem = new QTreeWidgetItem(paymentCenterMenu);
    paymentRecordItem->setText(0, "缴费记录");

    // 主内容区域
    m_mainContentStack = new QStackedWidget(this);

    // 添加页面到内容区
    m_mainContentStack->addWidget(m_noticeWidget);
    m_mainContentStack->addWidget(m_publicityWidget);
    m_mainContentStack->addWidget(m_warningWidget);
    m_mainContentStack->addWidget(m_initiativeWidget);
    m_mainContentStack->addWidget(m_vehicleRegistrationWidget);
    m_mainContentStack->addWidget(m_ownerInfoWidget);
    m_mainContentStack->addWidget(m_parkingSpaceQueryWidget);
    m_mainContentStack->addWidget(m_onlineRepairWidget);
    m_mainContentStack->addWidget(m_complaintSuggestionWidget);
    m_mainContentStack->addWidget(m_serviceEvaluationWidget);
    m_mainContentStack->addWidget(m_onlinePaymentWidget);
    m_mainContentStack->addWidget(m_paymentRecordWidget);

    // 组装主布局
    mainLayout->addWidget(m_mainNavTree, 2);
    mainLayout->addWidget(m_mainContentStack, 9);

    // 样式美化，与AdminWindow相同
    m_mainNavTree->setStyleSheet(R"(
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
        }
        QTreeWidget::branch:open:has-children:!has-siblings,
        QTreeWidget::branch:open:has-children:has-siblings  {
            border-image: none;
        }
    )");

    m_mainContentStack->setStyleSheet(R"(
        QStackedWidget {
            background-color: #ffffff; /* 白色背景 */
            border: none; /* 无边框 */
        }
    )");

    setStyleSheet("QMainWindow { background-color: #f0f0f0; }");
}

// 初始化状态栏（保持不变）
void OwnerWindow::initStatusBar()
{
    m_userLabel = new QLabel(QString("当前用户：业主 ID=%1").arg(m_uid), this);
    m_timeLabel = new QLabel(this);
    m_timer = new QTimer(this);
    ui->statusbar->addPermanentWidget(m_userLabel, 1);
    ui->statusbar->addPermanentWidget(m_timeLabel, 1);
    updateTime();
}

void OwnerWindow::loadAnnouncements(const QString& type) {
    QListWidget* targetList = nullptr;
    // 匹配公告类型到列表控件（确保正确）
    if (type == "通知") targetList = m_noticeList;
    else if (type == "公示") targetList = m_publicityList;
    else if (type == "警示") targetList = m_warningList;
    else if (type == "倡议") targetList = m_initiativeList;

    if (!targetList) return;
    targetList->clear();

    QSqlDatabase db = DatabaseHelper::getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT content, creator, created_at FROM announcements WHERE type = ? ORDER BY created_at DESC");
    query.bindValue(0, type); // 显式绑定参数（唯一占位符）

    if (!query.exec()) {
        targetList->addItem("DB Error: " + query.lastError().text());
        qDebug() << "SQL Query:" << query.lastQuery();
        qDebug() << "DB Error:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString content = query.value(0).toString();
        QString creator = query.value(1).toString();
        QString time = query.value(2).toString();
        QDateTime dt = QDateTime::fromString(time, "yyyy-MM-dd HH:mm:ss");
        if (!dt.isValid()) dt = QDateTime::fromString(time, Qt::ISODate);

        targetList->addItem(
            QString("[%1] 发布者: %2 时间: %3\n内容: %4")
            .arg(type)
            .arg(creator)
            .arg(dt.toString("yyyy-MM-dd HH:mm"))
            .arg(content)
        );
    }
}

// 连接信号槽（保持不变）
void OwnerWindow::connectSignals()
{
    connect(m_mainNavTree, &QTreeWidget::itemClicked,
            this, &OwnerWindow::onMainNavChanged);
    connect(m_timer, &QTimer::timeout, this, &OwnerWindow::updateTime);
    m_timer->start(1000);
}

// 主导航切换
void OwnerWindow::onMainNavChanged(QTreeWidgetItem* item, int column)
{
    if (item->text(0) == "通知") {
        m_mainContentStack->setCurrentWidget(m_noticeWidget);
        loadAnnouncements("通知");
    } else if (item->text(0) == "公示") {
        m_mainContentStack->setCurrentWidget(m_publicityWidget);
        loadAnnouncements("公示");
    } else if (item->text(0) == "警示") {
        m_mainContentStack->setCurrentWidget(m_warningWidget);
        loadAnnouncements("警示");
    } else if (item->text(0) == "倡议") {
        m_mainContentStack->setCurrentWidget(m_initiativeWidget);
        loadAnnouncements("倡议");
    } else if (item->text(0) == "车辆登记") {
        m_mainContentStack->setCurrentWidget(m_vehicleRegistrationWidget);
    } else if (item->text(0) == "业主信息") {
        m_mainContentStack->setCurrentWidget(m_ownerInfoWidget);
    } else if (item->text(0) == "车位查询") {
        m_mainContentStack->setCurrentWidget(m_parkingSpaceQueryWidget);
    } else if (item->text(0) == "在线报修") {
        m_mainContentStack->setCurrentWidget(m_onlineRepairWidget);
    } else if (item->text(0) == "投诉建议") {
        m_mainContentStack->setCurrentWidget(m_complaintSuggestionWidget);
    } else if (item->text(0) == "服务评价") {
        m_mainContentStack->setCurrentWidget(m_serviceEvaluationWidget);
    } else if (item->text(0) == "在线缴费") {
        m_mainContentStack->setCurrentWidget(m_onlinePaymentWidget);
    } else if (item->text(0) == "缴费记录") {
        m_mainContentStack->setCurrentWidget(m_paymentRecordWidget);
    }
}

void OwnerWindow::updateTime(){
    m_timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
}
