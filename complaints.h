#ifndef COMPLAINTS_H
#define COMPLAINTS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>

namespace Ui {
class Complaints;
}

class Complaints : public QWidget
{
    Q_OBJECT

public:
    explicit Complaints(QWidget *parent = nullptr);
    ~Complaints();

private slots:
    void on_back_clicked();
private slots:
    void onSubmitClicked();

private:
    Ui::Complaints *ui;
    bool validateInput();
    void initDatabase();
signals:
    void complaintgoBack();
private:
    QSqlDatabase db;
};

#endif // COMPLAINTS_H
