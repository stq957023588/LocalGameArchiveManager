#pragma once


#include <QLabel>
#include <QString>

class ElidedLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ElidedLabel(const QString &text, QWidget *parent = nullptr);

    void setFullText(const QString &text);
    QString getFullText() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QString fullText;
    void setElidedText();
    void setElidedText(Qt::TextElideMode mode);

};

