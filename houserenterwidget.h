#ifndef HOUSERENTERWIDGET_H
#define HOUSERENTERWIDGET_H
#include<QPushButton>
#include <QWidget>
#include <QTableWidget>

namespace Ui {
class HouseRenterWidget;
}

class HouseRenterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HouseRenterWidget(QWidget *parent = nullptr);
    ~HouseRenterWidget();

private slots:
    void onmyrenter();
    void onmyunrenter();
    void onrenter();
    void onunrenter();
    void onReturn();

private:
    void setFixedRowHeight(QTableWidget *table, int height);
    void refreshTable();
private:
    Ui::HouseRenterWidget *ui;
};

#endif // HOUSERENTERWIDGET_H
