#ifndef SUMMARYPAGE_H
#define SUMMARYPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

class SummaryPage : public QWidget {
    Q_OBJECT
public:
    explicit SummaryPage(QWidget *parent = nullptr);

private slots:
    void refreshData();

private:
    QTableWidget* complaintTable;
    QLabel* propertyFee;
    QLabel* parkingFee;
    QLabel* salary;
    QLabel* maintenance;
    QPushButton* refreshBtn;

    void initUI();
    void loadComplaints();
    void calculateStats();
};

#endif // SUMMARYPAGE_H
