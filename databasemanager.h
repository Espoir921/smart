#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QObject>
#include <QVariantList>
#include <QSqlQuery>
#include <QList>
#include <QVariant>
#include <QSqlError>
#include <QDate>

class DatabaseManager
{
public:

    static DatabaseManager& getInstance() {
            static DatabaseManager instance;
            return instance;
        }

    DatabaseManager();
    ~DatabaseManager();
    bool open(const QString& dbName);
    bool openDB(const QString& dbPath);
    QSqlQuery getUserByUserName(const QString& username);
    QSqlQuery query(const QString &query);
    QSqlQuery query(const QString &query, const QVariantList& args);
    void close();
    QList<QVariantList> getComplaints();
    double getIncomeSum(const QString& type);
    double getExpenseSum(const QString& type);
    bool updateComplaintStatus(int id, bool handled, int userId, QString name);
    QString lastError() const {
              return m_errorMsg;
          }
    void processParkingSpaceData();
        void transferPaymentToIncome();
        void transferAttendanceToExpense();
        double getYesterdayIncome();
        double getYesterdayPropertyFeeIncome();
        double getYesterdayParkingFeeIncome();
        double getLastMonthIncome();
        double getLastMonthPropertyFeeIncome();
        double getLastMonthParkingFeeIncome();
        double getLastYearIncome();
        double getLastYearPropertyFeeIncome();
        double getLastYearParkingFeeIncome();
        double getYesterdayExpense();
        double getYesterdayMaintenanceExpense();
        double getYesterdaySalaryExpense();
        double getLastMonthExpense();
        double getLastMonthMaintenanceExpense();
        double getLastMonthSalaryExpense();
        double getLastYearExpense();
        double getLastYearMaintenanceExpense();
        double getLastYearSalaryExpense();

    QSqlDatabase db;

public:
    QSqlDatabase m_db;

private:
    QString m_errorMsg;

};

const QString databasePath = "try.db";

#endif // DATABASEMANAGER_H
