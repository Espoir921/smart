#include "noticewidget.h"
#include "ui_noticewidget.h"

NoticeWidget::NoticeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoticeWidget)
{
    ui->setupUi(this);
}

NoticeWidget::~NoticeWidget()
{
    delete ui;
}
