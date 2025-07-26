#ifndef ROSTERQUERYWIDGET_H
#define ROSTERQUERYWIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class RosterQueryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RosterQueryWidget(QWidget *parent = nullptr);
    ~RosterQueryWidget();

private slots:
    void onQueryButtonClicked();
    void onRefreshButtonClicked();

private:
    void initDatabase();
    void setupUI();
    void loadAllData();
    void queryByStaffId(int staffId);
    void setFixedRowHeight(QTableWidget *table, int height);

    QSqlDatabase database;
    QTableWidget *tableWidget;
    QLineEdit *staffIdEdit;
    QPushButton *queryButton;
    QPushButton *refreshButton;
};

#endif // ROSTERQUERYWIDGET_H
