
#ifndef COMPLAINTRECORDPAGE_H
#define COMPLAINTRECORDPAGE_H

#include <QWidget>
#include <QHeaderView>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCheckBox>
#include "databasemanager.h"

namespace Ui {
class ComplaintRecordPage;
}

class ComplaintRecordPage : public QWidget {
    Q_OBJECT

public:
    explicit ComplaintRecordPage(QWidget *parent = nullptr);
    ~ComplaintRecordPage();
    void setUserId(int userId);


private slots:
    void onMarkHandled();          // 处理"标记已处理"按钮点击
    void loadComplaintsFromDatabase(); // 从数据库加载投诉数据

private:
    Ui::ComplaintRecordPage *ui;

    QTableWidget *tableWidget;    // 投诉表格
    QPushButton *btnMark;          // "标记已处理"按钮
    void initUI();                 // 初始化界面布局
    void setFixedRowHeight(QTableWidget *table, int height);
    int m_userId;
    QString m_name;
};

#endif // COMPLAINTRECORDPAGE_H

