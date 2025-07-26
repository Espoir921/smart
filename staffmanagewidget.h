#ifndef STAFFMANAGEWIDGET_H
#define STAFFMANAGEWIDGET_H

#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QRadioButton>

namespace Ui {
class StaffManageWidget;
}

class StaffManageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StaffManageWidget(QWidget *parent = nullptr);
    ~StaffManageWidget();

private:
    Ui::StaffManageWidget *ui;

private slots:
    void onAdd();
    void onEdit();
    void onDelete();
    void onSearch();
    void refreshTable();

private:
    QTableWidget* table;
    QPushButton* btnAdd;
    QPushButton* btnEdit;
    QPushButton* btnDelete;
    QPushButton* btnSearch;
    QLineEdit* Lineedit;
    QRadioButton* radioId;
    QRadioButton* radioUsername;
    QRadioButton* radioName;
    QRadioButton* radioPosition;
    QRadioButton* radioDepartment;
    QRadioButton* radioStatus;
    QButtonGroup* searchGroup;
    void loadStaff();
    void setFixedRowHeight(int height);

    int choose;

};

#endif // STAFFMANAGEWIDGET_H
