
#include "databasehelper.h"
#include <QDebug>
#include <QSqlError>
QSqlDatabase DatabaseHelper::db;

QSqlDatabase DatabaseHelper::getDatabase() {
    if (db.isValid() && db.isOpen()) {
        return db;
    }
    db = QSqlDatabase::addDatabase("QSQLITE", "try_db");
    db.setDatabaseName("try.db");
    if (!db.open()) {
        qDebug() << "Database open error:" << db.lastError().text();
    }
    return db;
}
