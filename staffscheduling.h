#ifndef STAFFSCHEDULING_H
#define STAFFSCHEDULING_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

namespace Ui {
class StaffScheduling;
}

class StaffScheduling : public QWidget
{
    Q_OBJECT

public:
    explicit StaffScheduling(QWidget *parent = nullptr);
    ~StaffScheduling();
    void setUserId(const int userId); // 添加设置当前用户ID的函数（用于获取approverid和approver）

private slots:
    void onComboBoxChanged();
    void onAdd();
    void onEdit();
    void onDelete();
    void refreshTable();

private:
    Ui::StaffScheduling *ui;
    QLabel* label;
    QTableWidget* table;
    QPushButton* btnAdd;
    QPushButton* btnEdit;
    QPushButton* btnDelete;
    QComboBox* comboBox;
    int m_userId; // 存储当前登录用户ID（用于approverid）
    void setupButtonsStyle();
    void setupComboBox();
    void setFixedRowHeight(int height);
};

#endif // STAFFSCHEDULING_H
