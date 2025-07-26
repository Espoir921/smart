#ifndef EVALUATION_H
#define EVALUATION_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>

namespace Ui {
class Evaluation;
}

class Evaluation : public QWidget
{
    Q_OBJECT

public:
    explicit Evaluation(QWidget *parent = nullptr);
    ~Evaluation();

private slots:
    void onQueryBtnClicked();       // 查询报修记录按钮事件
    void onSubmitEvalBtnClicked();  // 提交评价按钮事件
    void onTableItemClicked(int row, int column); // 点击表格项事件（选填记录）


signals:
    void evaluationgoBack();

private slots:
    void on_back_clicked();

private:
    Ui::Evaluation *ui;
    QSqlDatabase db;               // 数据库连接
        int currentRequestId;          // 当前选中的报修记录 ID
        void initDatabase();           // 初始化数据库连接
        void loadRecords(const QString &username); // 加载报修记录到表格
};

#endif // EVALUATION_H
