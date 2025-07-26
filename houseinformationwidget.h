#ifndef HOUSEINFORMATIONWIDGET_H
#define HOUSEINFORMATIONWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QButtonGroup>

namespace Ui {
class HouseInformationWidget;
}

class HouseInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HouseInformationWidget(QWidget *parent = nullptr);
    ~HouseInformationWidget();

private slots:
    void onSearch();
    void onAdd();
    void onEdit();
    void onDelete();
    void onRadioButtonChanged(int id);
    void refreshTable();

private:
    Ui::HouseInformationWidget *ui;
    QTableWidget* table;
    QPushButton* btnAdd;
    QPushButton* btnEdit;
    QPushButton* btnDelete;
    QPushButton* btnSearch;
    QLineEdit* lineEdit;
    QRadioButton* radioAll;
    QRadioButton* radioEmpty;
    QRadioButton* radioOccupied;
    QButtonGroup* radioGroup;
    void populateTable();
    bool isValidRoomNumber(const QString& roomNumber);
};

#endif // HOUSEINFORMATIONWIDGET_H
