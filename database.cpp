#include "database.h"
#include <QString>
#include <QSqlError>
#include <QDebug>
#include <QSqlRecord>
#include <QVariant>



DataBase::DataBase()
{

}
bool DataBase::openDB (const QString& dbPath){          //定义成员函数，接受数据库文件路径dbPath
    if(m_db.isOpen())return true;                       //如果成员变量已经打开，则直接返回true，避免重复连接
    m_db = QSqlDatabase::addDatabase("QSQLITE");        //调用方法addDatabase，创建连接，赋值给m_db
    m_db.setDatabaseName(dbPath);                       //设置数据库文件路径，关联到传入的dbPath
    if(!m_db.open()){
        qWarning() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }
    return true;
}
QSqlQuery DataBase::getUserByUsername(const QString& username){             //定义成员函数
    QSqlQuery query;                                                        //创建QSqlQuery对象用来执行SQL语句
    query.prepare("SELECT * FROM user WHERE username = ?");                //准备查询
    query.addBindValue(username);                                           //传参
    query.exec();                                                           //执行查询
    return query;
}

QSqlQuery DataBase::query(const QString& sql, const QVariantList& args)
{  //用SQL语句查询
    QSqlQuery query;
    query.prepare(sql); //预处理
    for (const auto& v : args) query.addBindValue(v); //遍历args中的每个元素并将其添加
    query.exec();
    return query;
}
DataBase :: ~DataBase(){

}

