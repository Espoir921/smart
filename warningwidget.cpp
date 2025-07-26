#include "warningwidget.h"
#include "ui_warningwidget.h"

WarningWidget::WarningWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WarningWidget)
{
    ui->setupUi(this);
}

WarningWidget::~WarningWidget()
{
    delete ui;
}
