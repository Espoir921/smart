#ifndef LEAVEAPPROVEWIDGET_H
#define LEAVEAPPROVEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QMessageBox>
#include <QTabWidget>

namespace Ui {
class LeaveApproveWidget;
}

class LeaveApproveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LeaveApproveWidget(QWidget *parent = nullptr);
    ~LeaveApproveWidget();
    void setUserId(const int userId);   //添加设置用户id的函数

private:
    Ui::LeaveApproveWidget *ui;
    void refreshTable();
    int m_userId;   //用于保存用户Id

    QTabWidget* tabwidget;
    //已处理界面
    QTableWidget* table1;
    QTableWidget* table0;
    QPushButton* btnSearch;
    QRadioButton* radioID;
    QRadioButton* radioId;
    QRadioButton* radioName;
    QRadioButton* radioDate;
    QRadioButton* radioHandler;
    QButtonGroup* searchGroup;
    QLabel* label;
    int choose;

    //未处理界面
    QPushButton* btnHandle;
    QMessageBox msgBox;

    void setFixedRowHeight(int height);
private slots:
        void onSearch();
        void onHandle();

};

#endif // LEAVEAPPROVEWIDGET_H
