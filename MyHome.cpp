#include "MyHome.h"
#include "ui_MyHome.h"
#include "databasemanager.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QInputDialog>
#include <QLineEdit>
#include <QDebug>
#include <QCloseEvent>


MyHome::MyHome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyHome),
    m_userId(-1)
{

    ui->setupUi(this);
    initDatabase();
    setupTableModel();
//   on_refreshButton_clicked();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

MyHome::~MyHome()
{
    delete ui;
}


void MyHome::initDatabase()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("try.db");
    }

    if (!db.open()) {
        qCritical() << "数据库打开失败：" << db.lastError();
        QMessageBox::critical(this, "错误", "无法连接数据库，请检查路径或权限！");
    }

}


void MyHome::setupTableModel()
{
    initDatabase();  // 确保db已初始化
//    QSqlQuery q;  // 确保使用与刷新相同的db对象
//    q.exec("SELECT name FROM sqlite_master WHERE type='table'");
//    while (q.next()) {
//        qDebug() << "Existing table:" << q.value(0).toString();
//    }
    model = new QSqlTableModel(this);

        ui->tableView->setModel(model);
        ui->tableView->setAlternatingRowColors(true);
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

        model->setTable("users");
        // 使用 m_userLocation 构建过滤条件，注意处理 m_userLocation 为空的情况
        QString filter = "role = 2";
     //   qDebug()<<"haode";
        if (!m_userLocation.isEmpty()) {
            filter += QString(" AND address = '%1'").arg(m_userLocation);
            qDebug()<<"Sql语句是"<<filter;
        }
        model->setFilter(filter);

        if (model->select()) {
            setupHeadersAndHiddenColumns();
            ui->tableView->resizeColumnsToContents();
        } else {
            QMessageBox::warning(this, "错误", "刷新数据失败: " + model->lastError().text());
        }

}
void MyHome::on_addButton_clicked()
{
    initDatabase();
        if (!db.isOpen()) {
            QMessageBox::critical(this, "错误", "数据库未连接！");
            return;
        }

        QSqlRecord newRecord = model->record();

        bool ok;
        QString username = QInputDialog::getText(this, "新增记录", "用户名:", QLineEdit::Normal, "", &ok);
        if (!ok || username.isEmpty()) return;

        QString password = QInputDialog::getText(this, "新增记录", "密码:", QLineEdit::Password, "", &ok);
        if (!ok) return;

        int role = 2;

        QString name = QInputDialog::getText(this, "新增记录", "姓名:", QLineEdit::Normal, "", &ok);
        if (!ok) name = "";

        QString phonenumber = QInputDialog::getText(this, "新增记录", "电话号码:", QLineEdit::Normal, "", &ok);
        if (!ok) phonenumber = "";


        newRecord.setValue("username", username);
        newRecord.setValue("password", password);
        newRecord.setValue("name", name);
        newRecord.setValue("department", "");
        newRecord.setValue("position", "");
        newRecord.setValue("phonenumber", phonenumber);
        newRecord.setValue("status", 1);
        newRecord.setValue("role", role);
        newRecord.setValue("address", m_userLocation);
//        qDebug() << newRecord;

        if (model->insertRecord(-1, newRecord)) {
            if (model->submitAll()) {
                QMessageBox::information(this, "成功",
                    QString("记录添加成功！"));
                on_refreshButton_clicked();
            } else {
                QMessageBox::warning(this, "错误", "无法提交更改: " + model->lastError().text());
            }
        } else {
            QMessageBox::warning(this, "错误", "无法插入新记录: " + model->lastError().text());
        }
}


void MyHome::on_deleteButton_clicked()
{
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) return;

    int row = selectedRows.first().row();
    QString name = model->data(model->index(row, 3)).toString();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if (QMessageBox::question(this, "确认删除",
                             QString("确定要删除 '%1' 的记录吗?").arg(name),
                             QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    if (model->removeRow(row)) {
        if (model->submitAll()) {
            QMessageBox::information(this, "成功", "记录删除成功");
            on_refreshButton_clicked();
        } else {
            QMessageBox::warning(this, "错误", "无法提交更改: " + model->lastError().text());
        }
    } else {
        QMessageBox::warning(this, "错误", "无法删除记录: " + model->lastError().text());
    }
}


void MyHome::setUserId(const int userId)
{
    m_userId = userId;
    // 执行数据库查询，根据 userId 获取对应的 address
        QSqlQuery query(db);
        // 编写 SQL 查询语句，假设 id 是 users 表的主键，通过 id 查找用户
        query.prepare("SELECT address FROM users WHERE id = :userId");
        query.bindValue(":userId", userId);
        if (query.exec()) {
            if (query.next()) { // 判断是否查询到结果
                m_userLocation = query.value("address").toString();
//                qDebug() << "查询到用户地址：" << m_userLocation;
                // 查询到地址后，重新设置表格模型的过滤条件并刷新
                setupTableModel();
//                qDebug()<<"zheli";
            } else {
                qWarning() << "未找到用户 ID 为" << userId << "的记录";
                m_userLocation.clear();
            }
        } else {
            qCritical() << "查询用户地址失败：" << query.lastError().text();
            m_userLocation.clear();
        }
}


void MyHome::on_tableSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    ui->deleteButton->setEnabled(!selected.isEmpty());
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void MyHome::setupHeadersAndHiddenColumns()
{

    model->setHeaderData(1, Qt::Horizontal, "用户名");
    model->setHeaderData(2, Qt::Horizontal, "密码");
    model->setHeaderData(3, Qt::Horizontal, "姓名");
    model->setHeaderData(6, Qt::Horizontal, "电话号码");
    model->setHeaderData(9, Qt::Horizontal, "地址");


    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(4, true);
    ui->tableView->setColumnHidden(5, true);
    ui->tableView->setColumnHidden(7, true);
    ui->tableView->setColumnHidden(8, true);
}


void MyHome::on_refreshButton_clicked()
{
    if (model->select()) {
            setupHeadersAndHiddenColumns();            
            ui->tableView->resizeColumnsToContents();
            ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        } else {
            QMessageBox::warning(this, "错误", "刷新数据失败: " + model->lastError().text());
        }
}
