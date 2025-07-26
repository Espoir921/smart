
#include "announcementwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QDateTime>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>

class AddAnnouncementDialog : public QDialog
{
    Q_OBJECT
public:
    AddAnnouncementDialog(QWidget *parent = nullptr) : QDialog(parent) { setupUI(); }
    QString getSelectedType() const { return typeComboBox->currentText(); }
    QString getContent() const { return contentEdit->toPlainText(); }

private:
    QComboBox *typeComboBox;
    QTextEdit *contentEdit;
    void setupUI() {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        // 类型选择布局
        QHBoxLayout *typeLayout = new QHBoxLayout();
        typeComboBox = new QComboBox(this);
        typeComboBox->addItems({"通知", "公示", "警示", "倡议"});
        typeLayout->addWidget(new QLabel("类型：", this));
        typeLayout->addWidget(typeComboBox);
        mainLayout->addLayout(typeLayout);
        // 内容编辑
        contentEdit = new QTextEdit(this);
        contentEdit->setPlaceholderText("请输入公告内容...");
        mainLayout->addWidget(contentEdit);
        // 按钮布局
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *ok = new QPushButton("确定", this);
        QPushButton *cancel = new QPushButton("取消", this);
        buttonLayout->addWidget(ok);
        buttonLayout->addWidget(cancel);
        mainLayout->addLayout(buttonLayout);
        // 信号连接
        connect(ok, &QPushButton::clicked, this, &QDialog::accept);
        connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    }
};

// 包含moc生成的代码（必须，否则vtable错误）
#include "announcementwindow.moc"

AnnouncementWindow::AnnouncementWindow(int adminId, const QString& adminName, QWidget *parent)
    : QMainWindow(parent), currentEditIndex(-1), m_adminId(adminId), m_adminName(adminName)
{
    setupUI();
    // 打开公告数据库
    if (!m_db.openDB("announcement.db")) {
        QMessageBox::critical(this, "数据库错误", "无法打开公告数据库！");
    } else {
        // 初始化时检查并创建表（首次运行时）
        // 在数据库初始化时添加唯一约束
        m_db.query("CREATE TABLE IF NOT EXISTS announcements ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "type TEXT NOT NULL, "
                   "content TEXT NOT NULL, "
                   "creator TEXT NOT NULL, "
                   "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                   "UNIQUE(type, content, creator)"
                   ")", QVariantList());
    }
}

AnnouncementWindow::~AnnouncementWindow()
{
    // 自动释放子控件
}

void AnnouncementWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QVBoxLayout(centralWidget);

    // 1. 操作按钮区域
    buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("增加公告", this);
    editButton = new QPushButton("编辑公告", this);
    deleteButton = new QPushButton("删除公告", this);
    refreshButton = new QPushButton("刷新", this); // 新增刷新按钮
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(refreshButton); // 添加刷新按钮到布局
    mainLayout->addLayout(buttonLayout);

    // 2. 查询框单独一行
    queryLayout = new QHBoxLayout();
    queryLineEdit = new QLineEdit(this);
    queryLineEdit->setPlaceholderText("输入关键词查询...");
    queryButton = new QPushButton("查询公告", this);
    queryLayout->addWidget(queryLineEdit);
    queryLayout->addWidget(queryButton);
    mainLayout->addLayout(queryLayout);

    // 3. 公告列表区域
    listWidget = new QListWidget(this);
    mainLayout->addWidget(listWidget);

    // 信号槽连接
    connect(addButton, &QPushButton::clicked, this, &AnnouncementWindow::onAddAnnouncementClicked);
    connect(editButton, &QPushButton::clicked, this, &AnnouncementWindow::onEditAnnouncementClicked);
    connect(deleteButton, &QPushButton::clicked, this, &AnnouncementWindow::onDeleteAnnouncementClicked);
    connect(queryButton, &QPushButton::clicked, this, &AnnouncementWindow::onQueryAnnouncementClicked);
    connect(refreshButton, &QPushButton::clicked, this, &AnnouncementWindow::onRefreshButtonClicked); // 连接刷新按钮信号槽

    // 默认加载通知类公告
    loadNotifications();
}

// 增加公告（弹出对话框）
void AnnouncementWindow::onAddAnnouncementClicked()
{
    AddAnnouncementDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString type = dialog.getSelectedType();
        QString content = dialog.getContent();
        onAddAnnouncementConfirmed(type, content);
    }
}

// 处理新增公告确认
void AnnouncementWindow::onAddAnnouncementConfirmed(const QString& type, const QString& content)
{
    if (content.isEmpty()) {
        QMessageBox::warning(this, "警告", "公告内容不能为空！");
        return;
    }

    // 使用INSERT OR REPLACE确保唯一
    QSqlQuery query = m_db.query(
        "INSERT OR REPLACE INTO announcements (type, content, creator) VALUES (?, ?, ?)",
        QVariantList() << type << content << m_adminName
    );

    if (query.exec()) {
        QMessageBox::information(this, "成功", "公告操作已完成！");
        // 直接刷新列表，确保与数据库一致
        onRefreshButtonClicked();
    } else {
        QMessageBox::warning(this, "失败", "数据库操作失败！");
    }
}

// 编辑公告（加载选中内容到编辑框）
void AnnouncementWindow::onEditAnnouncementClicked()
{
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要编辑的公告！");
        return;
    }
    currentEditIndex = listWidget->row(selectedItems.first());
    contentEdit->setText(selectedItems.first()->text().split("：").last()); // 提取内容部分
    // 解析现有类型并选中
    QString text = selectedItems.first()->text();
    if (text.startsWith("通知")) typeComboBox->setCurrentIndex(0);
    else if (text.startsWith("公示")) typeComboBox->setCurrentIndex(1);
    else if (text.startsWith("警示")) typeComboBox->setCurrentIndex(2);
}

// 保存公告（新增/更新）
void AnnouncementWindow::onSaveAnnouncementClicked()
{
    QString type = typeComboBox->currentText();
    QString content = contentEdit->toPlainText();
    if (content.isEmpty()) {
        QMessageBox::warning(this, "警告", "公告内容不能为空！");
        return;
    }

    if (currentEditIndex == -1) {
        // 使用INSERT OR REPLACE确保唯一
        QSqlQuery query = m_db.query(
            "INSERT OR REPLACE INTO announcements (type, content, creator) VALUES (?, ?, ?)",
            QVariantList() << type << content << m_adminName
        );

        if (query.exec()) {
            QMessageBox::information(this, "成功", "公告操作已完成！");
            // 直接刷新列表，确保与数据库一致
            onRefreshButtonClicked();
        } else {
            QMessageBox::warning(this, "失败", "数据库操作失败！");
        }
    } else {
        // 编辑逻辑保持不变...
        QString originalText = listWidget->item(currentEditIndex)->text();
        QSqlQuery query = m_db.query(
            "UPDATE announcements SET type = ?, content = ? WHERE content = ? AND creator = ?",
            QVariantList() << type << content << originalText.split("] ").last() << m_adminName
        );
        if (query.exec()) {
            onRefreshButtonClicked(); // 刷新列表
            QMessageBox::information(this, "成功", "公告已更新！");
        } else {
            QMessageBox::warning(this, "失败", "数据库更新失败！");
        }
    }
}

// 删除公告
void AnnouncementWindow::onDeleteAnnouncementClicked()
{
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要删除的公告！");
        return;
    }

    // 解析选中项的内容（用于数据库查询）
    QString originalText = selectedItems.first()->text();
    QString content = originalText.split("] ").last();  // 提取原始内容
    QString type = originalText.split("][").first().remove("[");  // 提取类型

    // 从数据库删除
    QSqlQuery query = m_db.query(
        "DELETE FROM announcements WHERE type = ? AND content = ? AND creator = ?",
        QVariantList() << type << content << m_adminName
    );
    if (query.exec()) {
        delete listWidget->takeItem(listWidget->row(selectedItems.first()));
        QMessageBox::information(this, "成功", "公告已从数据库删除！");
    } else {
        QMessageBox::warning(this, "失败", "数据库删除失败！");
    }
}

// 加载通知类公告
void AnnouncementWindow::loadNotifications()
{
    listWidget->clear();
    // 从数据库查询类型为"通知"的公告
    QSqlQuery query = m_db.query(
        "SELECT content, creator, created_at FROM announcements WHERE type = '通知' ORDER BY created_at DESC",
        QVariantList()
    );
    while (query.next()) {
        QString content = query.value(0).toString();
        QString creator = query.value(1).toString();
        QString time = query.value(2).toString();
        // 显示格式：[通知][创建人 时间] 内容
        listWidget->addItem(QString("[通知][%1 %2] %3").arg(creator).arg(time).arg(content));
    }
}

// 加载公示类公告（同理修改）
void AnnouncementWindow::loadPublicity()
{
    listWidget->clear();
    QSqlQuery query = m_db.query(
        "SELECT content, creator, created_at FROM announcements WHERE type = '公示' ORDER BY created_at DESC",
        QVariantList()
    );
    while (query.next()) {
        QString content = query.value(0).toString();
        QString creator = query.value(1).toString();
        QString time = query.value(2).toString();
        listWidget->addItem(QString("[公示][%1 %2] %3").arg(creator).arg(time).arg(content));
    }
}

// 加载警示类公告
void AnnouncementWindow::loadWarnings()
{
    listWidget->clear();
    QSqlQuery query = m_db.query(
        "SELECT content, creator, created_at FROM announcements WHERE type = '警示' ORDER BY created_at DESC",
        QVariantList()
    );
    while (query.next()) {
        QString content = query.value(0).toString();
        QString creator = query.value(1).toString();
        QString time = query.value(2).toString();
        listWidget->addItem(QString("[警示][%1 %2] %3").arg(creator).arg(time).arg(content));
    }
}

// 加载倡议类公告
void AnnouncementWindow::loadInitiatives()
{
    listWidget->clear();
    QSqlQuery query = m_db.query(
        "SELECT content, creator, created_at FROM announcements WHERE type = '倡议' ORDER BY created_at DESC",
        QVariantList()
    );
    while (query.next()) {
        QString content = query.value(0).toString();
        QString creator = query.value(1).toString();
        QString time = query.value(2).toString();
        listWidget->addItem(QString("[倡议][%1 %2] %3").arg(creator).arg(time).arg(content));
    }
}

// 刷新公告列表
void AnnouncementWindow::onRefreshButtonClicked()
{
    QString type = typeComboBox->currentText();
    listWidget->clear();

    QSqlQuery query = m_db.query(
        "SELECT content, creator, created_at FROM announcements WHERE type = ? ORDER BY created_at DESC",
        QVariantList() << type
    );

    while (query.next()) {
        QString content = query.value(0).toString();
        QString creator = query.value(1).toString();
        QString time = query.value(2).toString();

        // 确保时间格式一致
        QDateTime dateTime = QDateTime::fromString(time, Qt::ISODate);
        QString formattedTime = dateTime.toString("yyyy-MM-dd HH:mm");

        listWidget->addItem(QString("[%1][%2 %3] %4").arg(type).arg(creator).arg(formattedTime).arg(content));
    }
}

// 查询公告
void AnnouncementWindow::onQueryAnnouncementClicked()
{
    QString keyword = queryLineEdit->text().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入查询关键词！");
        return;
    }

    listWidget->clear();
    QString type = typeComboBox->currentText();
    QSqlQuery query = m_db.query(
        "SELECT content, creator, created_at FROM announcements WHERE type = ? AND content LIKE ? ORDER BY created_at DESC",
        QVariantList() << type << "%" + keyword + "%"
    );
    while (query.next()) {
        QString content = query.value(0).toString();
        QString creator = query.value(1).toString();
        QString time = query.value(2).toString();
        listWidget->addItem(QString("[%1][%2 %3] %4").arg(type).arg(creator).arg(time).arg(content));
    }
}
