#include "SettingItem.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QWidget>
#include <QPalette>
#include <QStyleOption>

SettingItem::SettingItem(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    nameLabel = new QLabel(this);
    descriptionLabel = new QLabel(this);
    valueEdit = new QLineEdit(this);

    layout->addWidget(nameLabel);
    layout->addWidget(descriptionLabel);
    layout->addWidget(valueEdit);
}

SettingItem::SettingItem(const QString &name, const QString &description, const QString &value, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(7);

    nameLabel = new QLabel(name, this);
    nameLabel->setStyleSheet("font-weight: bold");
    descriptionLabel = new QLabel(description, this);
    descriptionLabel->setStyleSheet("font-size: 12px; font-weight: lighter");
    valueEdit = new QLineEdit(value, this);

    layout->addWidget(nameLabel);
    layout->addWidget(descriptionLabel);
    layout->addWidget(valueEdit);


    connect(valueEdit, &QLineEdit::editingFinished, this, [=]()
            { emit valueEditingFinishedSignal(nameLabel->text(), valueEdit->text()); });
}

SettingItem::~SettingItem()
{
}

void SettingItem::setName(QString name)
{
    nameLabel->setText(name);
}

void SettingItem::setDescription(QString description)
{
    descriptionLabel->setText(description);
}

void SettingItem::setValue(QString value)
{
    valueEdit->setText(value);
}

void SettingItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QWidget *parentWidget = this->parentWidget();
        if (parentWidget)
        {

            // 获取父组件下所有的 QWidget 子组件
            QList<SettingItem *> children = parentWidget->findChildren<SettingItem *>(Qt::FindDirectChildrenOnly);

            // 打印每个子组件的名称和类名
            for (SettingItem *child : children)
            {
            }
        }
    }
    // 调用基类的事件处理函数
    QWidget::mouseReleaseEvent(event);
}

void SettingItem::enterEvent(QEvent *event)
{
}
void SettingItem::leaveEvent(QEvent *event)
{
}

void SettingItem::paintEvent(QPaintEvent *event)
{
    //  QPainter p(this);
    // p.setPen(QColor("green")); //设置画笔记颜色
    // p.drawRect(0, 0, width() -1, height() -1); //绘制边框

    // QStyleOption opt;
    // opt.initFrom(this);
    // QPainter p(this);
    // style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}