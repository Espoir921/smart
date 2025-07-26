#ifndef MYHOME_H
#define MYHOME_H

#include <QWidget>
#include <QSqlTableModel>
#include <QItemSelection>
#include <QSqlDatabase>

namespace Ui {
class MyHome;
}

class MyHome : public QWidget
{
    Q_OBJECT

public:
    explicit MyHome(QWidget *parent = nullptr);
    ~MyHome();
    void setUserId(const int userId);
    void setUserLocation(const QString userLocation);




private slots:

        void on_addButton_clicked();
        void on_deleteButton_clicked();

        void on_tableSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected); // 表格选中事件
        void on_refreshButton_clicked();

private:
    Ui::MyHome *ui;
    QSqlTableModel *model;
    QSqlQueryModel *customModel;
private:
       QSqlDatabase db;                  // 数据库连接
       void initDatabase();              // 初始化数据库连接
       void setupTableModel();
       void setupHeadersAndHiddenColumns();
       int m_userId;
       QString m_userLocation;

};

#endif // MYHOME_H
