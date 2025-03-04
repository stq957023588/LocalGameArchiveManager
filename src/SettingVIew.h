#pragma once


#include <QWidget>

class SettingView : public QWidget
{
    Q_OBJECT // 必须有这个宏，才能使用 Qt 的信号槽等功能

private:

private slots:
    void onConfigValueEditFinished(QString configName,QString configValue);

public:
    explicit SettingView(QWidget *parent = nullptr);
    ~SettingView();
};

