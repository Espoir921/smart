#ifndef REPAIRWORKORDERWIDGET_H
#define REPAIRWORKORDERWIDGET_H

#include <QWidget>
#include<QPushButton>
#include <QTableWidget>

namespace Ui {
class RepairWorkOrderWidget;
}

class RepairWorkOrderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RepairWorkOrderWidget(QWidget *parent = nullptr);
    ~RepairWorkOrderWidget();

private slots:
    void onchange();
    void ondelete();
    void onReturn();
private:
    void setFixedRowHeight(QTableWidget *table, int height);
    void refreshTable();

private:
    Ui::RepairWorkOrderWidget *ui;
};

#endif // REPAIRWORKORDERWIDGET_H
