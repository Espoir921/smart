#include "announcement.h"
#include "ui_announcement.h"

Announcement::Announcement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Announcement)
{
    ui->setupUi(this);
}

Announcement::~Announcement()
{
    delete ui;
}

void Announcement::on_back_clicked()
{
    emit goBack3();
}
