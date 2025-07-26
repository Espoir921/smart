#ifndef INITIATIVEWIDGET_H
#define INITIATIVEWIDGET_H

#include <QWidget>

namespace Ui {
class InitiativeWidget;
}

class InitiativeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InitiativeWidget(QWidget *parent = nullptr);
    ~InitiativeWidget();

private:
    Ui::InitiativeWidget *ui;
};

#endif // INITIATIVEWIDGET_H
