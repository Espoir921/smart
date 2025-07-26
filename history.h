#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QTextEdit>

namespace Ui {
class History;
}


class History : public QWidget
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);
    ~History();


private slots:
    void on_queryButton_2_clicked();
    void on_generateInvoiceButton_clicked();

    void on_back_clicked();

signals:
    void historygoBack();

private:
    Ui::History *ui;
    QSqlTableModel *model;
    QSqlDatabase db;
    void initDatabase();
    void setupTableModel();
    void generateInvoice(const QModelIndex &index);
};

#endif // HISTORY_H
