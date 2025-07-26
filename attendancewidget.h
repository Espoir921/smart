#ifndef ATTENDANCEWIDGET_H
#define ATTENDANCEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDate>
#include <QLabel>

namespace Ui {
class AttendanceWidget;
}

class AttendanceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttendanceWidget(QWidget *parent = nullptr);
    ~AttendanceWidget();

private slots:
    // 按钮槽函数
    void onAdd();         // 新增考勤记录
    void onEdit();        // 编辑考勤状态
    void onSearch();      // 搜索考勤记录
    // 联动与刷新槽函数
    void onYearChanged(int index);    // 年份选择变化
    void onMonthChanged(int index);   // 月份选择变化
    void onDateSelected();            // 日期选择后刷新Tab1
    bool onTotal();                   // 汇总月度数据
    void refreshTableTab1();          // 刷新Tab1表格
    void refreshTableTab2();          // 刷新Tab2表格

private:
    Ui::AttendanceWidget *ui;

    QLabel* label;
    // 控件指针
    QPushButton *pushButtonadd;       // 新增按钮
    QPushButton *pushButtonalter;     // 编辑按钮
    QPushButton *pushButtonSrch;      // 查找按钮
    QComboBox *comboBoxyear;          // 年份选择
    QComboBox *comboBoxmonth;         // 月份选择
    QComboBox *comboBoxday;           // 日期选择
    QComboBox *comboBoxtime;          // 月度选择
    QTableWidget *tableWidgetdaily;   // Tab1表格
    QTableWidget *tableWidgetTab2;    // Tab2表格
    QButtonGroup *radioGroup;         // 单选按钮组

    // 辅助函数
    void setupButtonsStyle();         // 设置按钮样式
    void setFixedRowHeight(QTableWidget *table, int height); // 固定行高
    int getDaysInMonth(int year, int month); // 获取月份天数
    bool isLeapYear(int year);        // 判断闰年
    QString getCurrentDateStr();      // 获取当前选择的日期字符串
    void initComboBoxes();
    void setupTableStyle(QTableWidget *table);
    void setupStatus(QString date);
};

#endif // ATTENDANCEWIDGET_H
