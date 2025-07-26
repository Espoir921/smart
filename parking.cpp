#include "parking.h"
#include "ui_parking.h"
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QPixmap>
#include <QDialog>
#include <QVBoxLayout>
#include <QMenu>
#include <QDialog>
#include <QFileDialog>
#include <QStandardPaths>
#include <QScreen>
#include <QScrollArea>


Parking::Parking(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Parking)
{
    ui->setupUi(this);


    QPixmap parkingMap("");
    if (!parkingMap.isNull()) {
        ui->parkingMapLabel->setPixmap(parkingMap.scaled(
            ui->parkingMapLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        ));
    }

    initDatabase();
    setupTable();
}

Parking::~Parking()
{
    delete ui;
}


void Parking::initDatabase()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("try.db");
    }

    if (!db.open()) {
        QMessageBox::critical(this, "错误", "数据库连接失败：" + db.lastError().text());
    }
}


void Parking::setupTable()
{

    model = new QSqlTableModel(this, db);
    model->setTable("parkingspace");


    model->setHeaderData(0, Qt::Horizontal, "车位编号");
    model->setHeaderData(2, Qt::Horizontal, "车牌号");
    model->setHeaderData(3, Qt::Horizontal, "用户名");
    model->setHeaderData(7, Qt::Horizontal, "开始日期");
    model->setHeaderData(8, Qt::Horizontal, "截止日期");


    for (int col = 0; col < model->columnCount(); ++col) {
       // bool shouldHide = !(col == 0 || col == 2 || col == 3 || col == 7 || col == 8);
        if (!(col == 0 || col == 2 || col == 3 || col == 7 || col == 8)){
        ui->resultTable->setColumnHidden(col,1);}
    }

    ui->resultTable->setModel(model);
    ui->resultTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultTable->setAlternatingRowColors(true);
    ui->resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);


    model->setFilter("1 = 0");
    model->select();

    // 在数据加载完成后隐藏特定列
       for (int col = 0; col < model->columnCount(); ++col) {
           if (!(col == 0 || col == 2 || col == 3 || col == 7 || col == 8)){
               ui->resultTable->setColumnHidden(col, true);
           }
       }

       ui->resultTable->setModel(model);
       ui->resultTable->setAlternatingRowColors(true);
       ui->resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
       ui->resultTable->resizeColumnsToContents();
}


void Parking::on_queryButton_clicked()
{
    QString licensePlate = ui->licensePlateEdit->text().trimmed();
    QString username = ui->usernameEdit->text().trimmed();

    if (licensePlate.isEmpty() && username.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "车牌号和用户名至少填一项！");

        model->setFilter("1 = 0");
        model->select();
        return;
    }


    QString filter = "status IN (1, 2)";
    if (!licensePlate.isEmpty()) {
        filter += QString(" AND licenseplate LIKE '%%1%'").arg(licensePlate);
    }
    if (!username.isEmpty()) {
        filter += QString(" AND username LIKE '%%1%'").arg(username);
    }

    model->setFilter(filter);
    model->select();


    for (int col = 0; col < model->columnCount(); ++col) {
        //bool shouldHide = !(col == 0 || col == 2 || col == 3 || col == 7 || col == 8);
        if (!(col == 0 || col == 2 || col == 3 || col == 7 || col == 8)){
        ui->resultTable->setColumnHidden(col,1);}
    }

    ui->resultTable->resizeColumnsToContents();
}


void Parking::on_showBigMapButton_clicked()
{

    QDialog *dialog = new QDialog(this, Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
    dialog->setWindowTitle("车位地图 - 大图模式");


    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    QScrollArea *scrollArea = new QScrollArea(dialog);
    QLabel *mapLabel = new QLabel(scrollArea);
    mapLabel->setAlignment(Qt::AlignCenter);


    QPixmap bigMap(":/parkingspace.jpg");
    if (!bigMap.isNull()) {

        mapLabel->setPixmap(bigMap);
        mapLabel->resize(bigMap.size());


        int dialogWidth = qMin(bigMap.width(), static_cast<int>(screenGeometry.width() * 0.8));
        int dialogHeight = qMin(bigMap.height(), static_cast<int>(screenGeometry.height() * 0.8));
        dialog->resize(dialogWidth, dialogHeight);


        dialog->setWindowState(dialog->windowState() | Qt::WindowMaximized);
    } else {
        mapLabel->setText("图片加载失败！");
        dialog->resize(400, 300);
    }


    scrollArea->setWidget(mapLabel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);


    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(scrollArea);

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();


    mapLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mapLabel, &QLabel::customContextMenuRequested, this, [=](const QPoint &pos){
        QMenu menu;
        QAction *saveAction = menu.addAction("保存图片");
        if (menu.exec(mapLabel->mapToGlobal(pos)) == saveAction) {
            QString filePath = QFileDialog::getSaveFileName(dialog, "保存图片", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), "图片文件 (*.jpg)");
            if (!filePath.isEmpty()) {
                bigMap.save(filePath);
            }
        }
    });
}
void Parking::on_pushButton_2_clicked()
{
    emit parkinggoBack();
}
