#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>

class SettingItem : public QWidget
{
    Q_OBJECT // 必须有这个宏，才能使用 Qt 的信号槽等功能

public: 
    explicit SettingItem(QWidget *parent = nullptr);
    explicit SettingItem(const QString &name,const QString &description,const QString &value,QWidget *parent = nullptr);
    ~SettingItem();

    void setName(QString name);
    void setDescription(QString description);
    void setValue(QString value);

private:
    QLabel *nameLabel;
    QLabel *descriptionLabel;
    QLineEdit *valueEdit;
    QGroupBox *box;

signals:
    void valueEditingFinishedSignal(QString configName,QString configValue);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);

signals:
    void clocked();
};

