/**
 * @file inst_label_item.cpp
 * @brief Implementation of InstLabelItem class.
 * @author
 * @date
 */

#include "view/inst_label_item.h"

#include <QPainter>
#include <QFontMetricsF>

InstLabelItem::InstLabelItem(const QString& text, QGraphicsItem* parent)
    : QGraphicsTextItem(text, parent) {
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setTextInteractionFlags(Qt::NoTextInteraction);
}

void InstLabelItem::setLabelText(const QString& text) {
    setPlainText(text);
}

QSizeF InstLabelItem::getTextSize() const {
    QFontMetricsF fm(font());
    return QSizeF(fm.horizontalAdvance(toPlainText()), fm.height());
}

void InstLabelItem::setLabelFont(const QFont& font) {
    labelFont_ = font;
    setFont(font);
}

void InstLabelItem::updatePosition(qreal centerX, qreal boxTop, qreal boxBottom, bool isName) {
    QSizeF size = getTextSize();
    qreal x = centerX - size.width() / 2.0;
    qreal y;

    if (isName) {
        // Name goes above the box
        y = boxTop - size.height() - 5;
    } else {
        // Module name goes below the box
        y = boxBottom + 5;
    }

    setPos(x, y);
}
