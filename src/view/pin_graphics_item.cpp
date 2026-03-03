/**
 * @file pin_graphics_item.cpp
 * @brief Implementation of PinGraphicsItem class.
 * @author
 * @date
 */

#include "view/pin_graphics_item.h"

#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QStyleOption>
#include <QPainterPath>

// Static color constants
const QColor PinGraphicsItem::ONE_CONNECTION_COLOR_(Qt::green);
const QColor PinGraphicsItem::MULTI_CONNECTION_COLOR_(255, 165, 0);  // Orange
const QColor PinGraphicsItem::NO_CONNECTION_COLOR_(Qt::red);

PinGraphicsItem::PinGraphicsItem(std::shared_ptr<Pin> pin, PinLayout* layout, const QRectF& instBoundingBox)
    : pin_(pin), layout_(layout), instBoundingBox_(instBoundingBox) {
    // Initialize visual properties
    inputColor_ = Qt::black;
    outputColor_ = Qt::black;

    pen_ = QPen(Qt::black);
    pen_.setWidth(1);

    nameFont_ = QFont("Sans-serif");
    nameFont_.setPointSize(7);

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);

    setZValue(20);  // Pin layer (above Inst)
}

QRectF PinGraphicsItem::boundingRect() const {
    if (!layout_) {
        return QRectF();
    }

    auto [innerPos, outerPos] = computeLinePoints();

    // Bounding rect covers the line, name, and marker
    qreal minX = std::min(innerPos.x(), outerPos.x()) - MARKER_SIZE_;
    qreal minY = std::min(innerPos.y(), outerPos.y()) - MARKER_SIZE_;
    qreal maxX = std::max(innerPos.x(), outerPos.x()) + MARKER_SIZE_;
    qreal maxY = std::max(innerPos.y(), outerPos.y()) + MARKER_SIZE_;

    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

void PinGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!pin_ || !layout_) {
        return;
    }

    auto [innerPos, outerPos] = computeLinePoints();

    // Set pen color based on direction
    pen_.setColor(pin_->getDirection() == PinDirection::INPUT ? inputColor_ : outputColor_);
    painter->setPen(pen_);

    // Draw line segment
    painter->drawLine(QLineF(innerPos, outerPos));

    // Draw small rectangle at inner end (connection point to Inst)
    QRectF innerRect(innerPos.x() - 2, innerPos.y() - 2, 4, 4);
    painter->fillRect(innerRect, QBrush(Qt::black));

    // Draw connection marker at outer end
    drawConnectionMarker(painter, outerPos);

    // Draw pin name (outside the marker)
    QString name = QString::fromStdString(pin_->getName());
    if (!name.isEmpty()) {
        painter->setFont(nameFont_);
        QPointF namePos = outerPos;
        qreal nameOffset = MARKER_SIZE_ + 2;

        if (layout_->getSide() == PinSide::LEFT) {
            namePos.setX(outerPos.x() - nameOffset);
            namePos.setY(outerPos.y() + 3);
        } else if (layout_->getSide() == PinSide::RIGHT) {
            namePos.setX(outerPos.x() + nameOffset);
            namePos.setY(outerPos.y() + 3);
        }

        painter->drawText(namePos, name);
    }
}

void PinGraphicsItem::setLayout(PinLayout* layout, const QRectF& instBoundingBox) {
    layout_ = layout;
    instBoundingBox_ = instBoundingBox;
    update();
}

void PinGraphicsItem::setPin(std::shared_ptr<Pin> pin) {
    pin_ = pin;
    update();
}

void PinGraphicsItem::setSelected(bool selected) {
    QGraphicsItem::setSelected(selected);
    update();
}

QPainterPath PinGraphicsItem::shape() const {
    QPainterPath path;
    if (layout_) {
        auto [innerPos, outerPos] = computeLinePoints();
        path.moveTo(innerPos);
        path.lineTo(outerPos);
    }
    return path;
}

void PinGraphicsItem::drawConnectionMarker(QPainter* painter, const QPointF& pos) const {
    int connectionCount = pin_->getConnectionCount();

    if (connectionCount == 0) {
        // Red circle
        painter->setBrush(NO_CONNECTION_COLOR_);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(pos, CIRCLE_RADIUS_, CIRCLE_RADIUS_);
    } else if (connectionCount == 1) {
        // Green diamond
        painter->setBrush(ONE_CONNECTION_COLOR_);
        painter->setPen(Qt::NoPen);
        QPolygonF diamond;
        diamond << QPointF(pos.x(), pos.y() - MARKER_SIZE_ / 2)
                << QPointF(pos.x() + MARKER_SIZE_ / 2, pos.y())
                << QPointF(pos.x(), pos.y() + MARKER_SIZE_ / 2)
                << QPointF(pos.x() - MARKER_SIZE_ / 2, pos.y());
        painter->drawPolygon(diamond);
    } else {
        // Orange diamond (multiple connections)
        painter->setBrush(MULTI_CONNECTION_COLOR_);
        painter->setPen(Qt::NoPen);
        QPolygonF diamond;
        diamond << QPointF(pos.x(), pos.y() - MARKER_SIZE_ / 2)
                << QPointF(pos.x() + MARKER_SIZE_ / 2, pos.y())
                << QPointF(pos.x(), pos.y() + MARKER_SIZE_ / 2)
                << QPointF(pos.x() - MARKER_SIZE_ / 2, pos.y());
        painter->drawPolygon(diamond);
    }
}

std::pair<QPointF, QPointF> PinGraphicsItem::computeLinePoints() const {
    if (!layout_) {
        return {QPointF(0, 0), QPointF(0, 0)};
    }

    QPointF anchorPos = layout_->getAnchorPos();
    PinSide side = layout_->getSide();

    QPointF innerPos = anchorPos;
    QPointF outerPos = anchorPos;

    switch (side) {
        case PinSide::LEFT:
            outerPos.setX(anchorPos.x() - LINE_LENGTH_);
            break;
        case PinSide::RIGHT:
            outerPos.setX(anchorPos.x() + LINE_LENGTH_);
            break;
        case PinSide::TOP:
            outerPos.setY(anchorPos.y() - LINE_LENGTH_);
            break;
        case PinSide::BOTTOM:
            outerPos.setY(anchorPos.y() + LINE_LENGTH_);
            break;
    }

    return {innerPos, outerPos};
}
