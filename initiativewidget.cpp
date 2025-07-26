#include "initiativewidget.h"
#include "ui_initiativewidget.h"

InitiativeWidget::InitiativeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InitiativeWidget)
{
    ui->setupUi(this);
}

InitiativeWidget::~InitiativeWidget()
{
    delete ui;
}
