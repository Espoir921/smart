#ifndef WARNINGWIDGET_H
#define WARNINGWIDGET_H

#include <QWidget>

namespace Ui {
class WarningWidget;
}

class WarningWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WarningWidget(QWidget *parent = nullptr);
    ~WarningWidget();

private:
    Ui::WarningWidget *ui;
};

#endif // WARNINGWIDGET_H
