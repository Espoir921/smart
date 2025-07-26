#ifndef ANNOUNCEMENTSWIDGET_H
#define ANNOUNCEMENTSWIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateTime>

namespace Ui {
class AnnouncementsWidget;
}

class AnnouncementsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnnouncementsWidget(QWidget *parent = nullptr);
    ~AnnouncementsWidget();

private slots:
    void on_publishBtn_clicked();
    void on_editBtn_clicked();
    void on_deleteBtn_clicked();
    void on_refreshBtn_clicked();
    void onread();

private:
    Ui::AnnouncementsWidget *ui;
    QSqlDatabase db;

    void initDatabase();
    void loadDataToTableWidget();
    int getSelectedRow() const;
};

// 发布/编辑公告的弹窗
class PublishDialog : public QDialog
{
    Q_OBJECT
public:
    PublishDialog(QWidget *parent = nullptr, bool isEditMode = false);
    QString type, content, creator, createdTime;
};

#endif // ANNOUNCEMENTSWIDGET_H
