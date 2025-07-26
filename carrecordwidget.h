#ifndef CARRECORDWIDGET_H
#define CARRECORDWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "databasemanager.h"

class CarRecordWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CarRecordWidget(QWidget *parent = nullptr);
    ~CarRecordWidget();

private slots:
    void onrefresh();
private:
    QTableWidget *table;
    QPushButton* refresh;
    void refreshTable();
    void setFixedRowHeight(QTableWidget *table, int height);
};

#endif // CARRECORDWIDGET_H
