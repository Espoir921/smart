#ifndef CHARGINGSTANDARDSETTINGWIDGET_H
#define CHARGINGSTANDARDSETTINGWIDGET_H

#include <QWidget>
#include <QToolBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFont>
#include <QPushButton>  // 新增：编辑按钮
#include <QTextEdit>    // 新增：文本编辑控件

namespace Ui {
class ChargingStandardSettingWidget;
}

class ChargingStandardSettingWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChargingStandardSettingWidget(QWidget *parent = nullptr);
    ~ChargingStandardSettingWidget();

private:
    Ui::ChargingStandardSettingWidget *ui;
    QToolBox *toolBox;  // 工具盒控件，用于切换两页内容

    // 物业费相关控件
    QScrollArea *propertyScrollArea;  // 支持内容滚动
    QWidget *propertyFeeWidget;       // 物业费内容容器
    QVBoxLayout *propertyFeeLayout;   // 物业费布局
    QLabel *propertyFeeContentLabel;  // 物业费文本内容
    QTextEdit *propertyContentEditor; // 新增：物业费文本编辑控件
    QPushButton *propertyEditButton;  // 新增：物业费编辑按钮

    // 车位费相关控件
    QScrollArea *parkingScrollArea;   // 支持内容滚动
    QWidget *parkingFeeWidget;        // 车位费内容容器
    QVBoxLayout *parkingFeeLayout;    // 车位费布局
    QLabel *parkingFeeContentLabel;   // 车位费文本内容
    QTextEdit *parkingContentEditor;  // 新增：车位费文本编辑控件
    QPushButton *parkingEditButton;   // 新增：车位费编辑按钮

    void initStyle();         // 初始化整体样式（浅蓝色系）
    void initPropertyFeePage();  // 初始化物业费页面
    void initParkingFeePage();   // 初始化车位费页面
    void initEditButton(QWidget *widget, QVBoxLayout *layout, QPushButton *&editButton, QLabel *contentLabel, QTextEdit *contentEditor, const QString& filePath); // 新增：初始化编辑按钮

    void saveContentToFile(const QString& content, const QString& filePath);
    QString loadContentFromFile(const QString& filePath);
};

#endif // CHARGINGSTANDARDSETTINGWIDGET_H
