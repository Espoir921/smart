#include "facilityinformationwidget.h"
#include "ui_facilityinformationwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>

FacilityInformationWidget::FacilityInformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FacilityInformationWidget)
{
    ui->setupUi(this);
    initStyle();
    initEditButton();
    initFacilityInfoPage();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->addWidget(facilityScrollArea);
    setLayout(mainLayout);
}


FacilityInformationWidget::~FacilityInformationWidget()
{
    delete ui;
}

void FacilityInformationWidget::initStyle() {
    // 原有样式不变
    setStyleSheet("QWidget { "
                  "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                  "stop:0 #f0f7ff, stop:1 #e6f2ff); "
                  "}");
}

// 初始化编辑按钮（标题上方）
void FacilityInformationWidget::initEditButton() {
    editButton = new QPushButton("编辑");

    // 调整按钮大小和样式
    editButton->setFixedSize(80, 50);  // 设置固定宽度80和高度50
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
    connect(editButton, &QPushButton::clicked, this, &FacilityInformationWidget::onEditButtonClicked);
}

// 修改编辑按钮点击事件：使用定时器确保布局完全更新
void FacilityInformationWidget::onEditButtonClicked() {
    if (contentEditor->isHidden()) {
        // 切换到编辑状态
        contentEditor->setPlainText(facilityContentLabel->text());
        facilityContentLabel->hide();
        contentEditor->show();
        editButton->setText("保存");

        // 强制文档重新计算大小
        contentEditor->document()->adjustSize();

        // 使用单次定时器延迟执行布局更新，确保UI有足够时间渲染
        QTimer::singleShot(100, this, [this]() {
            // 计算并设置编辑器的理想高度
            int idealHeight = contentEditor->document()->size().toSize().height() +
                             contentEditor->frameWidth() * 2;
            contentEditor->setMinimumHeight(idealHeight);

            // 强制布局更新
            facilityWidget->adjustSize();
            facilityScrollArea->updateGeometry();

            // 确保滚动条位置正确
            facilityScrollArea->verticalScrollBar()->setValue(0);
        });
    } else {
        // 切换到展示状态
        facilityContentLabel->setText(contentEditor->toPlainText());
        contentEditor->hide();
        facilityContentLabel->show();
        editButton->setText("编辑");

        // 保存修改后的内容到文件
        saveContentToFile(contentEditor->toPlainText());

        // 重置编辑器最小高度
        contentEditor->setMinimumHeight(0);

        // 强制布局更新
        facilityWidget->adjustSize();
        facilityScrollArea->updateGeometry();

        // 确保滚动条位置正确
        facilityScrollArea->verticalScrollBar()->setValue(0);
    }
}

void FacilityInformationWidget::initFacilityInfoPage() {
    // 滚动区域设置
    facilityScrollArea = new QScrollArea;
    facilityScrollArea->setWidgetResizable(true);
    facilityScrollArea->setStyleSheet("background-color: transparent; border: none;");

    // 内容容器
    facilityWidget = new QWidget;
    facilityWidget->setStyleSheet("background-color: #ffffff; border: 1px solid #b3d9ff; border-radius: 5px; padding: 15px;");
    facilityScrollArea->setWidget(facilityWidget);

    // 主布局
    facilityLayout = new QVBoxLayout(facilityWidget);
    facilityLayout->setContentsMargins(30, 20, 30, 20);
    facilityLayout->setSpacing(15);

    // 新增：顶部布局（包含编辑按钮）
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(0, 0, 0, 0);

    // 右侧对齐并设置右边距30
    topLayout->addStretch();
    editButton->setContentsMargins(0, 0, 30, 0);  // 设置按钮右边距
    topLayout->addWidget(editButton);

    // 将顶部布局添加到主布局
    facilityLayout->addLayout(topLayout);

    // 标题设置（保持原有样式）
    facilityTitle = new QLabel("汤臣二品小区公共设施设置单");
    QFont titleFont = facilityTitle->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    facilityTitle->setFont(titleFont);
    facilityTitle->setStyleSheet("color: #003366;");
    facilityTitle->setAlignment(Qt::AlignCenter);

    // 标题添加到主布局（下移一行）
    facilityLayout->addWidget(facilityTitle);


    // 文本展示标签（保持原有样式）
    facilityContentLabel = new QLabel;
    facilityContentLabel->setWordWrap(true);
    facilityContentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QFont textFont("宋体", 14);
    facilityContentLabel->setFont(textFont);
    facilityContentLabel->setStyleSheet("color: #333333;");

    // 文本编辑控件（默认隐藏）
    contentEditor = new QTextEdit;
    contentEditor->setFont(textFont);
    contentEditor->setStyleSheet("QTextEdit {"
                                "color: #333333;"
                                "background-color: transparent;"
                                "border: none;"
                                "}");
    contentEditor->setWordWrapMode(QTextOption::WordWrap);
    contentEditor->hide();

    // 设置文本编辑器的大小策略
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentEditor->setSizePolicy(sizePolicy);

    // 确保编辑器内部布局立即更新
    contentEditor->document()->setDocumentMargin(0);

    // 新增：设置文本编辑框的最小高度为标签高度的2倍（经验值）
    contentEditor->setMinimumHeight(facilityContentLabel->height() * 2);

    // 从文件读取内容
    QString facilityText = readContentFromFile();
    if (facilityText.isEmpty()) {
        facilityText = "    一、基础规模配置\n"
                       "    - 建筑布局:共12栋住宅楼,采用高层封闭式管理;\n"
                       "    - 单栋结构:每栋20层,首层为全架空设计(功能定位:公共活动/设备间),标准层实行一梯两户格局;\n"
                       "    - 居住规模:总计456户,社区容积率≤2.0;\n"
                       "    - 占地面积:约20,000平方米(含绿化及公共道路)。\n\n"
                       "    二、消防安全系统配置\n"
                       "    严格遵循国家消防规范,实施分级防控\n"
                       "    - 灭火装置:\n"
                       "        - 楼栋区域:480具(每层2具,分布于楼梯间及电梯厅);\n"
                       "        - 地下停车场:300具(集中设置于车道及单元入口);\n"
                       "    - 消防栓网络:\n"
                       "        - 室内消防栓:240个(各楼层公共区全覆盖);\n"
                       "        - 室外消防栓:167个(沿小区主干道环状布设);\n"
                       "    - 应急引导系统:\n"
                       "        - 安全出口标志:67处(疏散通道间隔≤15米);\n"
                       "        - 楼梯间标识:240套(含声光报警及荧光地标);\n"
                       "    - 智能火灾预警:\n"
                       "        - 烟感探测器:100个(覆盖大堂、走廊等公共区域);\n"
                       "        - 温感探测器:188个(重点部署车库、配电室、设备间);\n"
                       "        - 联动控制:实时连接物业中控室及消防部门。\n\n"
                       "    三、生活服务设施\n"
                       "    基于建筑结构的功能衍生\n"
                       "    1. 垂直交通:\n"
                       "        - 每栋配备高速客用电梯1部(载重≥1000kg,带断电应急平层功能);\n"
                       "    2. 公共空间利用:\n"
                       "        - 架空层:设置便民储物柜、共享工具箱、休闲座椅;\n"
                       "        - 单元入口:无障碍坡道+防滑地砖(宽度≥1.5米);\n"
                       "    3. 基础配套:\n"
                       "        - 垃圾分类点:每栋楼外侧独立封闭式站点;\n"
                       "        - 非机动车棚:集中充电位(灭火器覆盖半径内)。\n\n"
                       "    四、管理维护标准\n"
                       "    - 消防设施:季度巡检灭火器压力值,年度测试消防栓水压;\n"
                       "    - 电梯维保:15日/次常规保养,应急响应≤30分钟;\n"
                       "    - 公共照明:楼道LED声控灯双电路冗余供电;\n"
                       "    - 绿化养护:乔木与灌木比例1:3,定期防治病虫害。\n\n"
                       "    注:所有设施数据源自小区规划图纸(2023版),实际管理需同步业主公约及政府指导文件。\n\n"
                       "    编制单位:汤臣二品物业管理中心\n"
                       "    生效日期:2025年7月1日";
    }
    facilityContentLabel->setText(facilityText);

    // 添加文本控件到布局
    facilityLayout->addWidget(facilityContentLabel);
    facilityLayout->addWidget(contentEditor);
    facilityLayout->addStretch();
}

// 保存内容到文件
void FacilityInformationWidget::saveContentToFile(const QString& content) {
    QFile file("facility_info.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << content;
        file.close();
    }
}

// 从文件读取内容
QString FacilityInformationWidget::readContentFromFile() {
    QFile file("facility_info.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();
        return content;
    }
    return "";
}
