#ifndef PUBLICITYWIDGET_H
#define PUBLICITYWIDGET_H

#include <QWidget>

namespace Ui {
class PublicityWidget;
}

class PublicityWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PublicityWidget(QWidget *parent = nullptr);
    ~PublicityWidget();

private:
    Ui::PublicityWidget *ui;
};

#endif // PUBLICITYWIDGET_H
