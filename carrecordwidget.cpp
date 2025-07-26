#include "carrecordwidget.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include<QHeaderView>
#include<QString>
#include<QSqlDatabase>
#include<QPushButton>
#include"databasemanager.h"


CarRecordWidget::CarRecordWidget(QWidget *parent)
    : QWidget(parent)
{

    QLabel *title = new QLabel("汤臣二品车辆进出记录", this);
    title->setAlignment(Qt::AlignCenter);
    QFont font = title->font();
    font.setPointSize(16);
    title->setFont(font);
    title->setStyleSheet("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");


    refresh=new QPushButton("刷新",this);

    table = new QTableWidget(this);
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"停车编号", "车辆身份", "车牌号", "进入时间", "离开时间", "车身颜色", "车型"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                         "QHeaderView::section { font-weight: bold; } "
                         "QTableWidget::item:selected { background: #2d8cf0; }");
    setFixedRowHeight(table, 60);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(title);
    mainLayout->addWidget(table);
    QHBoxLayout *bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch();
    bottomLayout->addWidget(refresh);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    connect(refresh, &QPushButton::clicked, this, &CarRecordWidget::onrefresh);

    refreshTable();
}

// 固定行高
void CarRecordWidget::setFixedRowHeight(QTableWidget *table, int height)
{
    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHeight(i, height);
}

CarRecordWidget::~CarRecordWidget()
{

}

void CarRecordWidget::onrefresh(){
    refreshTable();
}

void CarRecordWidget::refreshTable()
{
    table->setRowCount(0);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); // 选驱动（如 SQLite ）
    db.setDatabaseName("try.db");
    if (db.open()) {
        QString sql = "SELECT parkingid, vehiclerole, licenseplate, entrytime, exittime, color, type FROM parking";
        QSqlQuery query(sql, db);

        int row = 0;
        while (query.next()) {
            table->insertRow(row);
            for (int col = 0; col < 7; ++col) {
                QString val = query.value(col).toString();
                table->setItem(row, col, new QTableWidgetItem(val));
            }
            ++row;
        }

        // 设置单元格居中
        for (int r = 0; r < table->rowCount(); ++r) {
            for (int c = 0; c < table->columnCount(); ++c) {
                if (QTableWidgetItem* item = table->item(r, c)) {
                    item->setTextAlignment(Qt::AlignCenter);
                }
            }
        }
        setFixedRowHeight(table, 60);

        db.close();
        } else {
            qDebug() << "数据库打开失败：" << db.lastError().text();
        }
}
