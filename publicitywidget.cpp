#include "publicitywidget.h"
#include "ui_publicitywidget.h"

PublicityWidget::PublicityWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PublicityWidget)
{
    ui->setupUi(this);
}

PublicityWidget::~PublicityWidget()
{
    delete ui;
}
