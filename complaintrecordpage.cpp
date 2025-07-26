
#include "complaintrecordpage.h"
#include "ui_complaintrecordpage.h"
#include "databasemanager.h"
#include <QScrollBar>
#include <QLabel>

ComplaintRecordPage::ComplaintRecordPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComplaintRecordPage)
{
    ui->setupUi(this);
    initUI(); // 初始化UI和布局
    loadComplaintsFromDatabase(); // 加载数据
}

ComplaintRecordPage::~ComplaintRecordPage()
{
    delete ui;
}

// 修改 initUI 方法
void ComplaintRecordPage::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 1. 标题（最上方）
    QLabel* titleLabel = new QLabel("汤臣二品投诉记录", this);
    titleLabel->setStyleSheet("font-family: '楷体'; color: #55aaff; font-size: 64px; margin-top: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 2. 表格（中间，占据大部分空间）
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"ID", "内容", "时间", "已处理"});
    tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 自适应扩展
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->setVisible(false); // 隐藏行号
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setStyleSheet("QTableWidget { border: none; background: #f8f8f8; }"
                             "QHeaderView::section { font-weight: bold; } "
                             "QTableWidget::item:selected { background: #2d8cf0; }");
        setFixedRowHeight(tableWidget, 60);
    mainLayout->addWidget(tableWidget, 10); // 权重设为10，确保占据大部分空间

    // 3. 按钮（表格正下方）
    btnMark = new QPushButton("标记选中项为已处理", this);
    btnMark->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(btnMark, &QPushButton::clicked, this, &ComplaintRecordPage::onMarkHandled);
    mainLayout->addWidget(btnMark, 0, Qt::AlignCenter); // 居中显示在表格下方

    // 布局间距
    mainLayout->setSpacing(10);
}

// 固定行高
void ComplaintRecordPage::setFixedRowHeight(QTableWidget *table, int height)
{
    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHeight(i, height);
}

//获取用户id
void ComplaintRecordPage::setUserId(int userId){
    m_userId = userId;
    DatabaseManager db;
    if (db.openDB(databasePath)){
        QSqlQuery q;
        q.prepare("SELECT name FROM users WHERE id = ?");
        q.addBindValue(userId);
        q.exec();
        while (q.next()) {
            m_name = q.value(0).toString();
        }
    }

}

void ComplaintRecordPage::loadComplaintsFromDatabase() {
        DatabaseManager dbManager;
        if (dbManager.open("try.db")) {
            QList<QVariantList> complaints = dbManager.getComplaints();
            tableWidget->setRowCount(complaints.size());

            for (int i = 0; i < complaints.size(); ++i) {
                const QVariantList& complaint = complaints[i];
                tableWidget->setItem(i, 0, new QTableWidgetItem(complaint[0].toString()));
                tableWidget->setItem(i, 1, new QTableWidgetItem(complaint[1].toString()));
                tableWidget->setItem(i, 2, new QTableWidgetItem(complaint[2].toString()));

                QCheckBox* handledCheck = new QCheckBox();
                handledCheck->setChecked(complaint[3].toBool());
                tableWidget->setCellWidget(i, 3, handledCheck);
            }
            // 设置单元格居中
            for (int r = 0; r < tableWidget->rowCount(); ++r) {
                for (int c = 0; c < tableWidget->columnCount(); ++c) {
                    if (QTableWidgetItem* item = tableWidget->item(r, c)) {
                        item->setTextAlignment(Qt::AlignCenter);
                    }
                }
            }
            setFixedRowHeight(tableWidget, 60);
            dbManager.close();
        }
}

void ComplaintRecordPage::onMarkHandled() {
        DatabaseManager dbManager;
        if (dbManager.open("try.db")) {
            for (int i = 0; i < tableWidget->rowCount(); ++i) {
                if (tableWidget->item(i, 0)->isSelected()) {
                    int id = tableWidget->item(i, 0)->text().toInt();

                    QCheckBox* handledCheck = qobject_cast<QCheckBox*>(tableWidget->cellWidget(i, 3));
                    bool handled = handledCheck ? handledCheck->isChecked() : false;

                    dbManager.updateComplaintStatus(id, handled, m_userId, m_name);
                }
            }
            dbManager.close();
            QMessageBox::information(this, "提示", "选中的投诉状态已更新！");
        }
}
