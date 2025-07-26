#ifndef PARKINGSPACEALLOCATIONWIDGET_H
#define PARKINGSPACEALLOCATIONWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSqlQuery>

namespace Ui {
class ParkingSpaceAllocationWidget;
}

class ParkingSpaceAllocationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParkingSpaceAllocationWidget(QWidget *parent = nullptr);
    ~ParkingSpaceAllocationWidget();

private slots:
    void refreshTable();                  // 刷新表格数据
    void onSearch();                      // 搜索按钮槽函数
    void onRadioButtonChanged(int id);    // 单选按钮切换槽函数

private:
    Ui::ParkingSpaceAllocationWidget *ui;
    QTableWidget *table;                  // 车位信息表格
    QLineEdit *searchLineEdit;            // 搜索输入框
    QPushButton *searchBtn;               // 搜索按钮
    QRadioButton *emptyRadio;             // 空车位单选按钮
    QRadioButton *occupiedRadio;          // 已售/出租车位单选按钮
    QButtonGroup *radioGroup;             // 单选按钮组

    void populateTable(QSqlQuery &query); // 填充表格数据（辅助函数）
};

#endif // PARKINGSPACEALLOCATIONWIDGET_H
