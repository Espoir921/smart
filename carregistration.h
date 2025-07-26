#ifndef CARREGISTRATION_H
#define CARREGISTRATION_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateEdit>
#include <QComboBox>

namespace Ui {
class CarRegistration;
}

class CarRegistration : public QWidget
{
    Q_OBJECT

public:
    explicit CarRegistration(QWidget *parent = nullptr);
    ~CarRegistration();

private slots:
    void onApplyTypeChanged(int index);
    void onSubmitClicked();

signals:
    void carregistrationgoBack();

private slots:
    void on_back_clicked();

private:
    Ui::CarRegistration *ui;
    QSqlDatabase db;
    void initDb();
};

#endif // CARREGISTRATION_H
