// DatabaseHelper.h
#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QSqlDatabase>
#include <QSqlQuery>

class DatabaseHelper {
public:
    static QSqlDatabase getDatabase();
private:
    static QSqlDatabase db;
};

#endif // DATABASEHELPER_H
