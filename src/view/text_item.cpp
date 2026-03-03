/**
 * @file text_item.cpp
 * @brief Implementation of TextItem class.
 * @author
 * @date
 */

#include "view/text_item.h"

#include <QFontMetricsF>

TextItem::TextItem(const QString& text, QGraphicsItem* parent)
    : QGraphicsTextItem(text, parent) {
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setTextInteractionFlags(Qt::NoTextInteraction);
    setZValue(25);  // Above pins (20)
}

void TextItem::setText(const QString& text) {
    setPlainText(text);
}

QSizeF TextItem::getTextSize() const {
    QFontMetricsF fm(font());
    return QSizeF(fm.horizontalAdvance(toPlainText()), fm.height());
}

void TextItem::setTextFont(const QFont& font) {
    textFont_ = font;
    setFont(font);
}

void TextItem::updatePositionCentered(qreal centerX, qreal y) {
    QSizeF size = getTextSize();
    setPos(centerX - size.width() / 2.0, y);
}

void TextItem::updatePosition(qreal refX, qreal refY, qreal offsetX, qreal offsetY) {
    QSizeF size = getTextSize();
    setPos(refX + offsetX, refY + offsetY - size.height());
}
