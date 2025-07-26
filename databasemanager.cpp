#include "databasemanager.h"
#include <QString>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>

DatabaseManager::DatabaseManager()
{

}

//打开数据库
bool DatabaseManager::openDB(const QString &dbPath){   //创建打开数据库的成员函数openDB，传入数据库路径
    if (m_db.isOpen()) return true;      //检查数据库是否已经打开，若打开，返回true
    m_db = QSqlDatabase::addDatabase("QSQLITE");    //为当前应用添加一个SQLite数据库驱动，并将返回的数据库连接对象赋值给 m_db
    m_db.setDatabaseName(dbPath);        //指定数据库文件的位置
    if (!m_db.open()) {                  //尝试打开，若失败，报出失败信息并返回false，否则返回true
        qWarning() << "Failed to open database:" <<m_db.lastError().text();
        return false;
    }
    return true;
}


QSqlQuery DatabaseManager::getUserByUserName(const QString &username) {   //传入username，从数据库中查询信息
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = ?");  //准备sqlite语句，？表示未知信息
    query.addBindValue(username);   //将传入的username传进sqlite语句中，并得到对应数据集
    query.exec();
    return query;
}

//发送命令并返回结果
QSqlQuery DatabaseManager::query(const QString &query) {
    QSqlQuery qu;
    qu.exec(query);
    return qu;
}
//发送命令和参数，并返回结果
QSqlQuery DatabaseManager::query(const QString &query, const QVariantList &args) {
    QSqlQuery q;
        q.prepare(query);  // 准备参数化查询
        // 遍历参数列表，逐个绑定值
        for (int i = 0; i < args.size(); ++i) {
            q.bindValue(i, args.at(i));
        }
        q.exec();  // 执行查询
        return q;  // 返回 QSqlQuery 对象，可用于获取结果、判断状态等
}

bool DatabaseManager::open(const QString& dbName) {
    // 连接名使用唯一标识
    QString connName = "conn_" + dbName;

    // 如果连接已存在，先关闭旧连接
    if (QSqlDatabase::contains(connName)) {
        QSqlDatabase existingDb = QSqlDatabase::database(connName);
        if (existingDb.isOpen()) {
            existingDb.close();
        }
        QSqlDatabase::removeDatabase(connName);
    }

    // 创建新连接
    db = QSqlDatabase::addDatabase("QSQLITE", connName);
    db.setDatabaseName(dbName);
    return db.open();
}

void DatabaseManager::close() {
    if (db.isOpen()) {
        db.close();
    }
    // 移除连接前确保没有查询在使用
    QString connName = db.connectionName();
    if (QSqlDatabase::contains(connName)) {
        QSqlDatabase::removeDatabase(connName);
    }
}

QList<QVariantList> DatabaseManager::getComplaints() {
    QList<QVariantList> data;
    if (!db.isOpen()) return data;

    QSqlQuery query(db); // 绑定当前连接
    query.exec("SELECT complaintid, description, date, status FROM complaints");
    while (query.next()) {
        data.append({
            query.value(0),
            query.value(1),
            query.value(2),
            query.value(3).toBool()
        });
    }
    return data;
}

double DatabaseManager::getIncomeSum(const QString& type) {
    if (!db.isOpen()) return 0.0;
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM income WHERE type = :type");
    query.bindValue(":type", type);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getExpenseSum(const QString& type) {
    if (!db.isOpen()) return 0.0;
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM expense WHERE type = :type");
    query.bindValue(":type", type);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

bool DatabaseManager::updateComplaintStatus(int id, bool handled, int userId, QString name) {
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return false;
    }
    QSqlQuery query(db);
    query.prepare("UPDATE complaints SET status = :handled ,handlerid = :handlerid ,handler = :name  WHERE complaintid = :id");
    query.bindValue(":handled", handled ? 1 : 0);
    query.bindValue(":handlerid", userId);
    query.bindValue(":name", name);
    query.bindValue(":id", id);
    if (query.exec()) {
        return query.numRowsAffected() > 0;
    } else {
        qDebug() << "Update failed:" << query.lastError().text();
        return false;
    }
}


DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
            m_db.close();
        }
}
void DatabaseManager::processParkingSpaceData() {
    if (!db.isOpen()) return;
    QSqlQuery query(db);
    query.exec("SELECT username, location, phonenumber, start_date, end_date FROM parkingspace");
    while (query.next()) {
        QString username = query.value(0).toString();
        QString location = query.value(1).toString();
        QString phonenumber = query.value(2).toString();
        QDate startDate = query.value(3).toDate();
        QDate endDate = query.value(4).toDate();

        int paymentPeriod;
        double payable, paid;
        int method;
        if (endDate.isNull()) {
            paymentPeriod = 12;
            payable = paid = 300000;
            method = 0;
        } else {
            int months = (endDate.year() - startDate.year()) * 12 + endDate.month() - startDate.month();
            if (endDate.day() > startDate.day()) {
                months++;
            }
            paymentPeriod = months;
            payable = paid = 600;
            method = 1;
        }

//        QSqlQuery insertQuery(db);
//        insertQuery.prepare("INSERT INTO payment (username, location, phonenumber, paymentperiod, type, payable, paid, paymenttime, method) VALUES (:username, :location, :phonenumber, :paymentPeriod, 2, :payable, :paid, :paymenttime, :method)");
//        insertQuery.bindValue(":username", username);
//        insertQuery.bindValue(":location", location);
//        insertQuery.bindValue(":phonenumber", phonenumber);
//        insertQuery.bindValue(":paymentPeriod", paymentPeriod);
//        insertQuery.bindValue(":payable", payable);
//        insertQuery.bindValue(":paid", paid);
//        insertQuery.bindValue(":paymenttime", startDate);
//        insertQuery.bindValue(":method", method);
//        insertQuery.exec();
    }
}

void DatabaseManager::transferPaymentToIncome() {
    if (!db.isOpen()) return;
    QSqlQuery query(db);
    query.exec("SELECT paymenttime, type, paid FROM payment");
    while (query.next()) {
        QDate date = query.value(0).toDate();
        int type = query.value(1).toInt();
        double amount = query.value(2).toDouble();
        QString incomeType;
        if (type == 1) {
            incomeType = "物业费";
        } else if (type == 2) {
            incomeType = "车位费";
        }

        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO income (date, type, amount) VALUES (:date, :type, :amount)");
        insertQuery.bindValue(":date", date);
        insertQuery.bindValue(":type", incomeType);
        insertQuery.bindValue(":amount", amount);
        insertQuery.exec();
    }
}

void DatabaseManager::transferAttendanceToExpense() {
    if (!db.isOpen()) return;
    QSqlQuery query(db);
    query.exec("SELECT month, status, leavenumber, absentnumber FROM attendances");
    while (query.next()) {
        QDate month = query.value(0).toDate();
        int status = query.value(1).toInt();
        int leaveNumber = query.value(2).toInt();
        int absentNumber = query.value(3).toInt();

        QDate paymentDate = month.addMonths(1).addDays(14); // 下一个月的15号
        double amount;
        if (status == 1) {
            amount = month.daysInMonth() * 300 + 900;
        } else {
            amount = (month.daysInMonth() - leaveNumber) * 300 - 600 * absentNumber;
        }

        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO expense (date, type, amount) VALUES (:date, '工资', :amount)");
        insertQuery.bindValue(":date", paymentDate);
        insertQuery.bindValue(":amount", amount);
        insertQuery.exec();
    }
}

double DatabaseManager::getYesterdayIncome() {
    if (!db.isOpen()) return 0.0;
    QDate yesterday = QDate::currentDate().addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM income WHERE date = :date");
    query.bindValue(":date", yesterday);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getYesterdayPropertyFeeIncome() {
    if (!db.isOpen()) return 0.0;
    QDate yesterday = QDate::currentDate().addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM income WHERE date = :date AND type = '物业费'");
    query.bindValue(":date", yesterday);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getYesterdayParkingFeeIncome() {
    if (!db.isOpen()) return 0.0;
    QDate yesterday = QDate::currentDate().addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM income WHERE date = :date AND type = '车位费'");
    query.bindValue(":date", yesterday);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastMonthIncome() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastMonth = currentDate.addMonths(-1).addDays(-currentDate.day() + 1);
    QDate endOfLastMonth = startOfLastMonth.addMonths(1).addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM income WHERE date BETWEEN :start AND :end");
    query.bindValue(":start", startOfLastMonth);
    query.bindValue(":end", endOfLastMonth);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastMonthPropertyFeeIncome() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastMonth = currentDate.addMonths(-1).addDays(-currentDate.day() + 1);
    QDate endOfLastMonth = startOfLastMonth.addMonths(1).addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM income WHERE date BETWEEN :start AND :end AND type = '物业费'");
    query.bindValue(":start", startOfLastMonth);
    query.bindValue(":end", endOfLastMonth);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastMonthParkingFeeIncome() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastMonth = currentDate.addMonths(-1).addDays(-currentDate.day() + 1);
    QDate endOfLastMonth = startOfLastMonth.addMonths(1).addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM income WHERE date BETWEEN :start AND :end AND type = '车位费'");
    query.bindValue(":start", startOfLastMonth);
    query.bindValue(":end", endOfLastMonth);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastYearIncome() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastYear = QDate(currentDate.year() - 1, 1, 1);
    QDate endOfLastYear = QDate(currentDate.year() - 1, 12, 31);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM income WHERE date BETWEEN :start AND :end");
    query.bindValue(":start", startOfLastYear);
    query.bindValue(":end", endOfLastYear);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastYearPropertyFeeIncome() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastYear = QDate(currentDate.year() - 1, 1, 1);
    QDate endOfLastYear = QDate(currentDate.year() - 1, 12, 31);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM income WHERE date BETWEEN :start AND :end AND type = '物业费'");
    query.bindValue(":start", startOfLastYear);
    query.bindValue(":end", endOfLastYear);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastYearParkingFeeIncome() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastYear = QDate(currentDate.year() - 1, 1, 1);
    QDate endOfLastYear = QDate(currentDate.year() - 1, 12, 31);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM income WHERE date BETWEEN :start AND :end AND type = '车位费'");
    query.bindValue(":start", startOfLastYear);
    query.bindValue(":end", endOfLastYear);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getYesterdayExpense() {
    if (!db.isOpen()) return 0.0;
    QDate yesterday = QDate::currentDate().addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM expense WHERE date = :date");
    query.bindValue(":date", yesterday);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getYesterdayMaintenanceExpense() {
    if (!db.isOpen()) return 0.0;
    QDate yesterday = QDate::currentDate().addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM expense WHERE date = :date AND type = '维修费'");
    query.bindValue(":date", yesterday);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getYesterdaySalaryExpense() {
    if (!db.isOpen()) return 0.0;
    if (QDate::currentDate().day() != 16) return 0.0;
    QDate yesterday = QDate::currentDate().addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM expense WHERE date = :date AND type = '工资'");
    query.bindValue(":date", yesterday);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastMonthExpense() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastMonth = currentDate.addMonths(-1).addDays(-currentDate.day() + 1);
    QDate endOfLastMonth = startOfLastMonth.addMonths(1).addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM expense WHERE date BETWEEN :start AND :end");
    query.bindValue(":start", startOfLastMonth);
    query.bindValue(":end", endOfLastMonth);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastMonthMaintenanceExpense() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastMonth = currentDate.addMonths(-1).addDays(-currentDate.day() + 1);
    QDate endOfLastMonth = startOfLastMonth.addMonths(1).addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM expense WHERE date BETWEEN :start AND :end AND type = '维修费'");
    query.bindValue(":start", startOfLastMonth);
    query.bindValue(":end", endOfLastMonth);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastMonthSalaryExpense() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastMonth = currentDate.addMonths(-1).addDays(-currentDate.day() + 1);
    QDate endOfLastMonth = startOfLastMonth.addMonths(1).addDays(-1);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM expense WHERE date BETWEEN :start AND :end AND type = '工资'");
    query.bindValue(":start", startOfLastMonth);
    query.bindValue(":end", endOfLastMonth);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastYearExpense() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastYear = QDate(currentDate.year() - 1, 1, 1);
    QDate endOfLastYear = QDate(currentDate.year() - 1, 12, 31);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM expense WHERE date BETWEEN :start AND :end");
    query.bindValue(":start", startOfLastYear);
    query.bindValue(":end", endOfLastYear);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastYearMaintenanceExpense() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastYear = QDate(currentDate.year() - 1, 1, 1);
    QDate endOfLastYear = QDate(currentDate.year() - 1, 12, 31);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM expense WHERE date BETWEEN :start AND :end AND type = '维修费'");
    query.bindValue(":start", startOfLastYear);
    query.bindValue(":end", endOfLastYear);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}

double DatabaseManager::getLastYearSalaryExpense() {
    if (!db.isOpen()) return 0.0;
    QDate currentDate = QDate::currentDate();
    QDate startOfLastYear = QDate(currentDate.year() - 1, 1, 1);
    QDate endOfLastYear = QDate(currentDate.year() - 1, 12, 31);
    QSqlQuery query(db);
    query.prepare("SELECT SUM(amount) FROM expense WHERE date BETWEEN :start AND :end AND type = '工资'");
    query.bindValue(":start", startOfLastYear);
    query.bindValue(":end", endOfLastYear);
    query.exec();
    query.next();
    return query.value(0).toDouble();
}
