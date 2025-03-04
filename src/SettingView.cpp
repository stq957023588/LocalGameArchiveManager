#include <QWidget>
#include "SettingView.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QEvent>
#include <QMouseEvent>
#include <QSqlQuery>
#include <QSqlError>
#include "SettingItem.h"
#include "SettingRepository.h"
#include <QMessageBox>

SettingView::SettingView(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(9);
    layout->setAlignment(Qt::AlignTop);

    QList<Setting> settings = SettingRepository::select();
    for (Setting setting : settings)
    {
        SettingItem *settingItem = new SettingItem(setting.name, setting.description, setting.value, this);
        connect(settingItem, SIGNAL(valueEditingFinishedSignal(QString, QString)), this, SLOT(onConfigValueEditFinished(QString, QString)));
        layout->addWidget(settingItem);
    }
}

void SettingView::onConfigValueEditFinished(QString settingName, QString settingValue)
{

    bool result = SettingRepository::updateSetting(settingName, settingValue);
    if (!result)
    {
        QMessageBox::critical(this, "程序异常", "更新配置失败！");
    }
}

SettingView::~SettingView()
{
}
