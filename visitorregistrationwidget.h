#ifndef VISITORREGISTRATIONWIDGET_H
#define VISITORREGISTRATIONWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QTableWidgetItem>
// 用于数据库操作（假设用 Qt SQL 模块，根据实际情况替换）
#include <QSqlDatabase>
#include <QSqlQuery>

namespace Ui {
class VisitorRegistrationWidget;
}

class VisitorRegistrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VisitorRegistrationWidget(QWidget *parent = nullptr);
    ~VisitorRegistrationWidget();

private:
    void onchange();
    void onReturn();
    void refreshTable();
    void onadd();
    void setFixedRowHeight(QTableWidget *table, int height);

private slots:

    void onSearchButtonClicked();
    void onEditButtonClicked();
    void onComboBoxCurrentIndexChanged(int index);

private:
    Ui::VisitorRegistrationWidget *ui;
    int currentSearchType = 0;
    QSqlDatabase db;
    QString getSearchCondition();
    QString getEditContent(const QString &originalValue);
    void queryAndUpdateTable(const QString &condition);
};

#endif // VISITORREGISTRATIONWIDGET_H
