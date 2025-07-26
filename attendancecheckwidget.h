#ifndef ATTENDANCECHECKWIDGET_H
#define ATTENDANCECHECKWIDGET_H
#include<QPushButton>
#include <QWidget>
#include<QTableWidget>//表格
namespace Ui {
class AttendanceCheckWidget;
}

class AttendanceCheckWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttendanceCheckWidget(QWidget *parent = nullptr);
    ~AttendanceCheckWidget();
    //获取当前用户名
    void setUserId(const int userId);

private:
    int m_userId = -1;   //用于保存用户Id
    QTableWidget* table;
    QPushButton* btnAdd;
    QPushButton* btnEdit;
    QPushButton* btnDelete;
    QPushButton* btnReturn;

private:
    void setFixedRowHeight(QTableWidget *table, int height);
    void refreshTable();
    bool onCheck(QString date);

private slots:
    void onAdd();
    void onEdit();
    void onDelete();
    void onReturn();


private:
    Ui::AttendanceCheckWidget *ui;
};

#endif // ATTENDANCECHECKWIDGET_H
