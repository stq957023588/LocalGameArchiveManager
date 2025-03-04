#include "ElidedLabel.h"
#include <QFontMetrics>

ElidedLabel::ElidedLabel(const QString &text, QWidget *parent)
    : QLabel(parent), fullText(text) {
    setElidedText();
}

void ElidedLabel::setFullText(const QString &text) {
    fullText = text;
    setElidedText();
}

QString ElidedLabel::getFullText() const {
    return fullText;
}

void ElidedLabel::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    setElidedText();
}

void ElidedLabel::setElidedText() {
    setElidedText(Qt::ElideMiddle);
}

void ElidedLabel::setElidedText(Qt::TextElideMode mode) {
    QFontMetrics metrics(font());
    QString elidedText = metrics.elidedText(fullText, mode, width());
    setText(elidedText);
    setToolTip(fullText);
}