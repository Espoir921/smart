#ifndef FACILITYINFORMATIONWIDGET_H
#define FACILITYINFORMATIONWIDGET_H
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QPushButton>  // 新增：编辑按钮
#include <QTextEdit>    // 新增：文本编辑控件
#include <QFile>        // 新增：文件操作头文件
#include <QTextStream>  // 新增：文本流头文件

namespace Ui {
class FacilityInformationWidget;
}

class FacilityInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FacilityInformationWidget(QWidget *parent = nullptr);
    ~FacilityInformationWidget();

private slots:
    void onEditButtonClicked();  // 新增：编辑按钮点击事件

private:
    Ui::FacilityInformationWidget *ui;
    QScrollArea *facilityScrollArea;
    QWidget *facilityWidget;
    QVBoxLayout *facilityLayout;
    QLabel *facilityTitle;       // 调整：改为成员变量（原局部变量）
    QLabel *facilityContentLabel;// 内容标签（用于展示）
    QTextEdit *contentEditor;    // 新增：文本编辑控件（用于修改）
    QPushButton *editButton;     // 新增：编辑功能键

    void initStyle();
    void initFacilityInfoPage();
    void initEditButton();       // 新增：初始化编辑按钮

    void saveContentToFile(const QString& content); // 新增：保存内容到文件
    QString readContentFromFile();                  // 新增：从文件读取内容
};

#endif // FACILITYINFORMATIONWIDGET_H
