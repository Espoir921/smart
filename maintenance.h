#ifndef MAINTENANCE_H
#define MAINTENANCE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>


namespace Ui {
class Maintenance;
}

class Maintenance : public QWidget
{
    Q_OBJECT

public:
    explicit Maintenance(QWidget *parent = nullptr);
    ~Maintenance();

private slots:
    void on_back_clicked();
    // 1. 基础样式设置
    void setStyleBase();
    // 2. 标题区域美化
    void styleTitleArea();
    // 3. 表单区域美化
    void styleFormArea();
    // 4. 按钮区域美化
    void styleButtonArea();
    // 5. 布局微调
    void adjustLayout();

private slots:
    void onSubmitRepairClicked();  // 提交报修信息按钮事件


signals:
    void maintenancegoBack();

private:
    Ui::Maintenance *ui;
    QSqlDatabase db;               // 数据库连接
        void initDatabase();           // 初始化数据库连接
        bool validateInput();          // 校验输入（必填项检查）
};

#endif // MAINTENANCE_H
