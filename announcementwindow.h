
#ifndef ANNOUNCEMENTWINDOW_H
#define ANNOUNCEMENTWINDOW_H
#include <QMainWindow>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QDialog> // 新增
#include "databasemanager.h"  // 数据库类

class AddAnnouncementDialog; // 新增

class AnnouncementWindow : public QMainWindow
{
    Q_OBJECT
public:
    // 构造函数：接收管理员ID和名称（用于记录创建人）
    explicit AnnouncementWindow(int adminId, const QString& adminName, QWidget *parent = nullptr);
    ~AnnouncementWindow();

private slots:
    void onAddAnnouncementClicked();
    void onEditAnnouncementClicked();
    void onDeleteAnnouncementClicked();
    void onQueryAnnouncementClicked();
    void onSaveAnnouncementClicked();
    void onRefreshButtonClicked(); // 新增刷新按钮槽函数
    void onAddAnnouncementConfirmed(const QString& type, const QString& content); // 新增

public:
    void loadNotifications();
    void loadPublicity();
    void loadWarnings();
    void loadInitiatives();

private:
    QListWidget *listWidget;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *queryButton;
    QPushButton *saveButton;
    QPushButton *refreshButton; // 新增刷新按钮
    QLineEdit *queryLineEdit;
    QComboBox *typeComboBox;
    QTextEdit *contentEdit;
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    QHBoxLayout *editLayout;
    QHBoxLayout *queryLayout;
    int currentEditIndex;  // 当前编辑的公告索引

    // 数据库相关
    DatabaseManager m_db;         // 数据库实例
    int m_adminId;         // 管理员ID
    QString m_adminName;   // 管理员名称（创建人）

    void setupUI();
    void addAnnouncement(const QString& type, const QString& content);
    void editAnnouncement(const QString& type, int index, const QString& newContent);
    void deleteAnnouncement(const QString& type, int index);
    void queryAnnouncement(const QString& keyword);
};
#endif // ANNOUNCEMENTWINDOW_H
