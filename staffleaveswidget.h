#ifndef STAFFLEAVESWIDGET_H
#define STAFFLEAVESWIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTabWidget>
#include <QTableWidget>
#include <QLabel>
#include <QDateEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace Ui {
class StaffLeavesWidget;
}

class StaffLeavesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StaffLeavesWidget(QWidget *parent = nullptr);
    ~StaffLeavesWidget();
    //获取当前用户名
    void setUserId(const int userId);

private slots:
    // 提交请假申请槽函数
    void onSubmitLeaveClicked();
    // 刷新待审批列表槽函数
    void onRefreshPendingClicked();
    // 刷新已审批列表槽函数
    void onRefreshApprovedClicked();

private:
    Ui::StaffLeavesWidget *ui;
    QSqlDatabase db;  // 数据库连接

    void setFixedRowHeight(QTableWidget *table, int height);
    // TabWidget 及子页面组件
    QTabWidget *tabWidget;
    QWidget *pendingTab;   // 待审批页面
    QWidget *approvedTab;  // 已审批页面
    QTableWidget *pendingTable; // 待审批表格
    QTableWidget *approvedTable; // 已审批表格

    // 请假申请表单组件
    QLabel *staffIdLabel; // 将 QLineEdit 替换为 QLabel
    QLabel *staffNameLabel; // 将 QLineEdit 替换为 QLabel
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QTextEdit *reasonEdit;
    QPushButton *submitBtn;

    int m_userId;   //用于保存用户Id

    // 初始化数据库
    void initDatabase();
    // 初始化 TabWidget 及页面
    void initTabWidget();
    // 初始化请假申请表单
    void initLeaveForm();
    // 加载待审批数据（approvalprogress = 0）
    void loadPendingData();
    // 加载已审批数据（approvalprogress = 1）
    void loadApprovedData();
    // 加载用户信息
    void loadUserInfo();
};

#endif // STAFFLEAVESWIDGET_H
