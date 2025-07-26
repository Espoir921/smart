#include "chargingstandardsettingwidget.h"
#include "ui_chargingstandardsettingwidget.h"
#include <QFrame>
#include <QPalette>
#include <QMargins>
#include <QHBoxLayout>
#include <QTimer>
#include <QFile>
#include <QTextStream>

ChargingStandardSettingWidget::ChargingStandardSettingWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::ChargingStandardSettingWidget) {
    ui->setupUi(this);
    initStyle();              // 初始化整体样式
    toolBox = new QToolBox(this);  // 创建工具盒
    toolBox->setFrameShape(QFrame::NoFrame);  // 去除边框，更美观

    // 设置QToolBox标题框样式
    toolBox->setStyleSheet("QToolBox::tab {"
                           "height: 60px;"
                           "font-size: 20px;"
                           "background-color: #e0f0ff;"
                           "border: 1px solid #b3d9ff;"
                           "border-radius: 5px;"
                           "padding-left: 20px;"
                           "color: #003366;"
                           "}"
                           "QToolBox::tab:selected {"
                           "background-color: #b3d9ff;"
                           "}");

    // 初始化两页内容
    initPropertyFeePage();
    initParkingFeePage();

    // 将两页添加到ToolBox
    toolBox->addItem(propertyScrollArea, "物业费收费标准");
    toolBox->addItem(parkingScrollArea, "车位收费标准");
    // 设置QToolBox标签页的字体大小
    toolBox->setStyleSheet("QToolBox::tab { font-size: 24px; }");

    // 设置主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);  // 主窗口边距
    mainLayout->addWidget(toolBox);
    setLayout(mainLayout);
}

ChargingStandardSettingWidget::~ChargingStandardSettingWidget() {
    delete ui;
}

// 初始化整体样式（浅蓝色系主题）
void ChargingStandardSettingWidget::initStyle() {
    // 主窗口背景（浅蓝色渐变）
    setStyleSheet("QWidget { "
                  "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                  "stop:0 #f0f7ff, stop:1 #e6f2ff); "
                  "}");
}

// 初始化物业费收费标准页面
void ChargingStandardSettingWidget::initPropertyFeePage() {
    // 滚动区域（内容过长时可滚动）
    propertyScrollArea = new QScrollArea;
    propertyScrollArea->setWidgetResizable(true);
    propertyScrollArea->setStyleSheet("background-color: transparent; border: none;");

    // 内容容器
    propertyFeeWidget = new QWidget;
    propertyFeeWidget->setStyleSheet("background-color: #ffffff; border: 1px solid #b3d9ff; border-radius: 5px; padding: 15px;");
    propertyScrollArea->setWidget(propertyFeeWidget);

    // 垂直布局
    propertyFeeLayout = new QVBoxLayout(propertyFeeWidget);
    propertyFeeLayout->setContentsMargins(30, 20, 30, 20);  // 边距调整
    propertyFeeLayout->setSpacing(15);

    // 标题
    QLabel *propertyTitle = new QLabel("汤臣二品小区物业服务收费标准");
    QFont titleFont = propertyTitle->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    propertyTitle->setFont(titleFont);
    propertyTitle->setStyleSheet("color: #003366; margin-bottom: 10px;");
    propertyTitle->setAlignment(Qt::AlignCenter);

    // 内容文本
    QString propertyText = loadContentFromFile("property_fee.txt");
    if (propertyText.isEmpty()) {
        propertyText = "    生效日期：2025年7月6日\n"
                       "    适用对象：本小区所有住宅业主\n"
                       "    制定依据：《物业服务收费管理办法》及业主大会决议\n\n"
                       "    一、基础物业服务费\n"
                       "    标准：￥8.5 - 12元/㎡/月（按房产证建筑面积计收）\n"
                       "    服务包含：\n"
                       "        • 24小时五星级安防系统（人脸识别门禁 + 电子巡更）\n"
                       "        • 公共区域及电梯轿厢每日3次专业保洁\n"
                       "        • 名贵绿化景观月度养护（≥4次专业修剪）\n"
                       "        • 恒温泳池/健身房设备日常维护\n"
                       "        • 管家式投诉响应（30分钟内现场处理）\n"
                       "        • 户内紧急维修\n\n"
                       "    二、专项服务费\n"
                       "    1. 地下专属车位管理费：￥180元/月/个（含智能充电桩基础维护）\n"
                       "    2. 高端快递代收保管：￥60元/件（奢侈品/冷链物品专项服务）\n\n"
                       "    三、费用管理规则\n"
                       "    1. 缴费周期：按季度预缴（每季度首月10日前支付）\n"
                       "    2. 滞纳金：逾期每日收取应缴费用的0.05%（上限为总额10%）\n"
                       "    3. 空置房优惠：连续空置超3个月，经备案后按70%收取\n"
                       "    4. 公共收益分配：电梯广告/场地租赁收益60%自动划入维修基金\n\n"
                       "    四、服务监督机制\n"
                       "    1. 每月发布《服务质量白皮书》公示保洁/安防执行记录\n"
                       "    2. 第三方机构年度2次业主满意度盲测\n"
                       "    3. 维修基金动用须经专有部分面积及人数双三分之二以上业主书面同意\n"
                       "    监督电话：区房管局 0532 - 82869290\n"
                       "    电子平台：业主专属APP实时查询账单及服务进度\n\n"
                       "    重要提示：\n"
                       "    1. 本标准通过业主大会表决程序（表决文件编号：001）\n"
                       "    2. 中央空调/水体维护等能耗分摊按《公共能耗计算规则》执行\n"
                       "    3. 价格构成已由会计师事务所出具成本审计报告（备查号：002）\n\n"
                       "    制定单位：汤臣二品小区业主委员会\n"
                       "    物业服务方：国际物业管理有限公司（国家一级资质）\n"
                       "    公示期限：2025年7月10日至7月31日（意见反馈至业委会邮箱）\n"
                       "    高端服务基准依据《住宅物业服务等级规范》（GB/T 20647 - 2024）AAA级认证标准制定";
    }
    propertyFeeContentLabel = new QLabel(propertyText);
    propertyFeeContentLabel->setWordWrap(true);  // 自动换行
    propertyFeeContentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);  // 支持文本选择

    // 设置正文文字样式为四号宋体
    QFont textFont("宋体", 14);
    propertyFeeContentLabel->setFont(textFont);
    propertyFeeContentLabel->setStyleSheet("color: #333333;");

    // 文本编辑控件（默认隐藏）
    propertyContentEditor = new QTextEdit;
    propertyContentEditor->setFont(textFont);
    propertyContentEditor->setStyleSheet("QTextEdit {"
                                         "color: #333333;"
                                         "background-color: transparent;"
                                         "border: none;"
                                         "}");
    propertyContentEditor->setWordWrapMode(QTextOption::WordWrap);
    propertyContentEditor->hide();

    // 设置文本编辑器的大小策略
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    propertyContentEditor->setSizePolicy(sizePolicy);

    // 确保编辑器内部布局立即更新
    propertyContentEditor->document()->setDocumentMargin(0);

    // 新增：设置文本编辑框的最小高度为标签高度的2倍（经验值）
    propertyContentEditor->setMinimumHeight(propertyFeeContentLabel->height() * 2);

    // 初始化编辑按钮
    initEditButton(propertyFeeWidget, propertyFeeLayout, propertyEditButton, propertyFeeContentLabel, propertyContentEditor, "property_fee.txt");

    // 添加控件到布局
    propertyFeeLayout->addWidget(propertyTitle);
    propertyFeeLayout->addWidget(propertyFeeContentLabel);
    propertyFeeLayout->addWidget(propertyContentEditor);
    propertyFeeLayout->addStretch();  // 底部留白
}

// 初始化车位收费标准页面
void ChargingStandardSettingWidget::initParkingFeePage() {
    // 滚动区域
    parkingScrollArea = new QScrollArea;
    parkingScrollArea->setWidgetResizable(true);
    parkingScrollArea->setStyleSheet("background-color: transparent; border: none;");

    // 内容容器
    parkingFeeWidget = new QWidget;
    parkingFeeWidget->setStyleSheet("background-color: #ffffff; border: 1px solid #b3d9ff; border-radius: 5px; padding: 15px;");
    parkingScrollArea->setWidget(parkingFeeWidget);

    // 垂直布局
    parkingFeeLayout = new QVBoxLayout(parkingFeeWidget);
    parkingFeeLayout->setContentsMargins(30, 20, 30, 20);
    parkingFeeLayout->setSpacing(15);

    // 标题
    QLabel *parkingTitle = new QLabel("汤臣二品小区车位收费标准");
    QFont titleFont = parkingTitle->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    parkingTitle->setFont(titleFont);
    parkingTitle->setStyleSheet("color: #003366; margin-bottom: 10px;");
    parkingTitle->setAlignment(Qt::AlignCenter);

    // 内容文本
    QString parkingText = loadContentFromFile("parking_fee.txt");
    if (parkingText.isEmpty()) {
        parkingText = "    一、车位租金标准\n"
                      "    1. 月租费用：500元人民币/月（固定车位使用权）\n"
                      "    2. 年租优惠：一次性缴纳全年租金，可享“年卡优惠”价格5000元人民币/年（相当于节省1个月费用）\n\n"
                      "    二、车位产权出售标准\n"
                      "    1. 产权定价：30万元人民币/个（产权永久归属购买者）\n"
                      "    2. 付款方式：需一次性付清全款，不支持分期付款（注：具体产权转移手续按国家规定办理）\n\n"
                      "    三、其他说明\n"
                      "    1. 生效日期：本收费标准自2025年XX月XX日起正式执行\n"
                      "    2. 办理地点：业主需携带身份证、车辆行驶证至物业服务中心签订租赁/购买协议\n"
                      "    3. 费用范围：租金与产权费用均不含车辆管理费（管理费标准详见物业服务合同）\n\n"
                      "    特别提示：\n"
                      "    新能源车辆充电车位租金另计，详情咨询物业\n\n"
                      "    四、监督与咨询\n"
                      "    如有异议或需进一步了解细则，请致电物业服务中心：\n"
                      "    电话：0532 - 250719\n"
                      "    办公时间：每日8:00 - 16:00";
    }
    parkingFeeContentLabel = new QLabel(parkingText);
    parkingFeeContentLabel->setWordWrap(true);
    parkingFeeContentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // 设置正文文字样式为四号宋体
    QFont textFont("宋体", 14);
    parkingFeeContentLabel->setFont(textFont);
    parkingFeeContentLabel->setStyleSheet("color: #333333;");

    // 文本编辑控件（默认隐藏）
    parkingContentEditor = new QTextEdit;
    parkingContentEditor->setFont(textFont);
    parkingContentEditor->setStyleSheet("QTextEdit {"
                                        "color: #333333;"
                                        "background-color: transparent;"
                                        "border: none;"
                                        "}");
    parkingContentEditor->setWordWrapMode(QTextOption::WordWrap);
    parkingContentEditor->hide();

    // 设置文本编辑器的大小策略
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    parkingContentEditor->setSizePolicy(sizePolicy);

    // 确保编辑器内部布局立即更新
    parkingContentEditor->document()->setDocumentMargin(0);

    // 新增：设置文本编辑框的最小高度为标签高度的2倍（经验值）
    parkingContentEditor->setMinimumHeight(parkingFeeContentLabel->height() * 2);

    // 初始化编辑按钮
    initEditButton(parkingFeeWidget, parkingFeeLayout, parkingEditButton, parkingFeeContentLabel, parkingContentEditor, "parking_fee.txt");

    // 添加控件到布局
    parkingFeeLayout->addWidget(parkingTitle);
    parkingFeeLayout->addWidget(parkingFeeContentLabel);
    parkingFeeLayout->addWidget(parkingContentEditor);
    parkingFeeLayout->addStretch();
}

// 初始化编辑按钮
void ChargingStandardSettingWidget::initEditButton(QWidget *widget, QVBoxLayout *layout, QPushButton *&editButton, QLabel *contentLabel, QTextEdit *contentEditor, const QString& filePath) {
    editButton = new QPushButton("编辑");

    // 调整按钮大小和样式
    editButton->setFixedSize(80, 50);  // 设置固定宽度80和高度30
    editButton->setStyleSheet("QPushButton {"
                              "background-color: #b3d9ff;"
                              "color: #003366;"
                              "border: 1px solid #80bfff;"
                              "border-radius: 3px;"
                              "font-size: 10pt;"  // 设置字体大小为10pt
                              "}"
                              "QPushButton:hover {"
                              "background-color: #80bfff;"
                              "}");

    // 绑定点击事件
    connect(editButton, &QPushButton::clicked, [this, contentLabel, contentEditor, editButton, filePath]() {
        if (contentEditor->isHidden()) {
            // 切换到编辑状态
            contentEditor->setPlainText(contentLabel->text());
            contentLabel->hide();
            contentEditor->show();
            editButton->setText("保存");

            // 强制文档重新计算大小
            contentEditor->document()->adjustSize();

            // 使用单次定时器延迟执行布局更新，确保UI有足够时间渲染
/*            QTimer::singleShot(100, this, [this, contentEditor, widget]() {
                // 计算并设置编辑器的理想高度
                int idealHeight = contentEditor->document()->size().toSize().height() +
                                  contentEditor->frameWidth() * 2;
                contentEditor->setMinimumHeight(idealHeight);

                // 强制布局更新
                widget->adjustSize();
                widget->parentWidget()->updateGeometry();

                // 确保滚动条位置正确
                QScrollArea *scrollArea = qobject_cast<QScrollArea*>(widget->parentWidget());
                if (scrollArea) {
                    scrollArea->verticalScrollBar()->setValue(0);
                }
            });  */
        } else {
            // 切换到展示状态
            contentLabel->setText(contentEditor->toPlainText());
            contentEditor->hide();
            contentLabel->show();
            editButton->setText("编辑");

            // 保存内容到文件
            saveContentToFile(contentEditor->toPlainText(), filePath);

            // 重置编辑器最小高度
            contentEditor->setMinimumHeight(0);

            // 强制布局更新
/*            widget->adjustSize();
            widget->parentWidget()->updateGeometry();

            // 确保滚动条位置正确
            QScrollArea *scrollArea = qobject_cast<QScrollArea*>(widget->parentWidget());
            if (scrollArea) {
                scrollArea->verticalScrollBar()->setValue(0);
            } */
        }
    });

    // 新增：顶部布局（包含编辑按钮）
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(0, 0, 0, 0);

    // 右侧对齐并设置右边距30
    topLayout->addStretch();
    editButton->setContentsMargins(0, 0, 30, 0);  // 设置按钮右边距
    topLayout->addWidget(editButton);

    // 将顶部布局添加到主布局
    layout->insertLayout(0, topLayout);
}

// 保存内容到文件
void ChargingStandardSettingWidget::saveContentToFile(const QString& content, const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << content;
        file.close();
    }
}

// 从文件加载内容
QString ChargingStandardSettingWidget::loadContentFromFile(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();
        return content;
    }
    return "";
}
