#ifndef ANNOUNCEMENT_H
#define ANNOUNCEMENT_H

#include <QWidget>

namespace Ui {
class Announcement;
}

class Announcement : public QWidget
{
    Q_OBJECT

public:
    explicit Announcement(QWidget *parent = nullptr);
    ~Announcement();

private slots:
    void on_back_clicked();

private:
    Ui::Announcement *ui;
signals:
    void goBack3();
};

#endif // ANNOUNCEMENT_H
