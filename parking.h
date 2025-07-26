#ifndef PARKING_H
#define PARKING_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include "ui_parking.h"

namespace Ui {
class Parking;
}

class Parking : public QWidget
{
    Q_OBJECT

public:
    explicit Parking(QWidget *parent = nullptr);
    ~Parking();


private slots:
    void on_queryButton_clicked();
    void on_showBigMapButton_clicked();



signals:
    void parkinggoBack();
private slots:
    void on_pushButton_2_clicked();




private:
    Ui::Parking *ui;
    QSqlDatabase db;
        QSqlTableModel *model;

        void initDatabase();
        void setupTable();
};

#endif // PARKING_H
