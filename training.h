#ifndef TRAINING_H
#define TRAINING_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QSqlQuery>
#include <QDateEdit>
#include <QLabel>

namespace Ui {
class Training;
}

class Training : public QWidget
{
    Q_OBJECT

public:
    explicit Training(QWidget *parent = nullptr);
    ~Training();

private slots:
    void onAdd();         // 增加培训记录
    void onEdit();        // 编辑培训记录
    void onDelete();      // 删除培训记录
    void onSearch();      // 搜索培训记录
    void refreshTable();  // 刷新表格

private:
    Ui::Training *ui;
    QLabel* label;
    QTableWidget* table;  // 培训记录表格
    // 按钮
    QPushButton* btnAdd;
    QPushButton* btnEdit;
    QPushButton* btnDelete;
    QPushButton* btnSearch;
    // 搜索相关
    QLineEdit* lineEditSearch;
    QRadioButton* radioStaffId;
    QRadioButton* radioStaffName;
    QRadioButton* radioEvalution;
    QButtonGroup* searchGroup;

    void setupButtonsStyle();  // 设置按钮样式
    void setFixedRowHeight(int height);  // 设置表格固定行高
};

#endif // TRAINING_H
