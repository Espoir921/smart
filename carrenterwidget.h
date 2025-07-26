#ifndef CARRENTERWIDGET_H
#define CARRENTERWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QLabel>
#include <QComboBox>

namespace Ui {
class CarRenterWidget;
}

class CarRenterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CarRenterWidget(QWidget *parent = nullptr);
    ~CarRenterWidget();
    void setUserId(const int userId);   //添加设置用户id的函数

private:
    Ui::CarRenterWidget *ui;
    QSqlDatabase db;               // 数据库连接
    QTabWidget *tabWidget;         // 标签页
    QTableWidget *unAuditedTable;  // 未审核表格
    QTableWidget *auditedTable;    // 已审核表格
    QPushButton *btnRefreshUnAudit;// 未审核刷新
    QPushButton *btnRefreshAudit;  // 已审核刷新
    QPushButton *btnAudit;         // 审核按钮
    QPushButton *btnAdd;           // 新增数据按钮

    // 初始化核心功能
    void initDatabase();   // 数据库连接
    void initUI();         // 界面布局
    void initStyle();      // 美化样式
    void loadData();       // 加载数据
    void setFixedRowHeight(QTableWidget *table, int height); //固定行高

    // 表格操作
    void fillTable(QTableWidget *table,int status); // 通用填充表格方法
    void refreshTable(QTableWidget *table, int status); // 通用刷新方法

    // 审核逻辑
    void auditRow();       // 执行审核
    void openAuditDialog(int row); // 打开审核对话框

    // 新增数据
    void openAddDialog();  // 打开新增对话框
    void openUpdateDialog();
    void addDataToDB(const QMap<QString, QString>& data); // 插入数据库
    void updateDataToDB(const QMap<QString, QString>& data);// 更新数据库

    int addId;
    int m_userId;

    QString m_name;

private slots:
    void onRefreshUnAuditClicked(); // 未审核刷新槽
    void onRefreshAuditClicked();   // 已审核刷新槽
    void onAuditClicked();          // 审核按钮槽
    void onAddClicked();            // 新增按钮槽
};

#endif // CARRENTERWIDGET_H
