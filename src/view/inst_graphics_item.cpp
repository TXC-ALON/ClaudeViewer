/**
 * @file inst_graphics_item.cpp
 * @brief Implementation of InstGraphicsItem class.
 * @author
 * @date
 */

#include "view/inst_graphics_item.h"

#include <QPainter>
#include <QStyleOption>
#include <QFontMetrics>

InstGraphicsItem::InstGraphicsItem(std::shared_ptr<Inst> inst, InstLayout* layout)
    : inst_(inst), layout_(layout) {
    // Initialize visual properties
    defaultPenColor_ = Qt::black;
    selectedPenColor_ = Qt::blue;
    fillColor_ = Qt::white;

    pen_ = QPen(defaultPenColor_);
    pen_.setWidth(1);

    brush_ = QBrush(fillColor_);

    nameFont_ = QFont("Sans-serif");
    nameFont_.setPointSize(9);
    nameFont_.setBold(true);

    moduleFont_ = QFont("Sans-serif");
    moduleFont_.setPointSize(8);

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, false);  // Static layout
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);

    setZValue(20);  // Inst layer
}

QRectF InstGraphicsItem::boundingRect() const {
    if (!layout_) {
        return QRectF();
    }
    // Expand bounding rect to include text above and below (with more offset)
    return layout_->getBoundingBox().adjusted(-10, -30, 10, 35);
}

void InstGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!inst_ || !layout_) {
        return;
    }

    QRectF bbox = layout_->getBoundingBox();

    // Set pen style: dashed for TOP inst, solid for regular inst
    if (inst_->isTop()) {
        pen_.setStyle(Qt::DashLine);
    } else {
        pen_.setStyle(Qt::SolidLine);
    }

    // Highlight if selected
    if (isSelected()) {
        pen_.setColor(selectedPenColor_);
        pen_.setWidth(2);
    } else {
        pen_.setColor(defaultPenColor_);
        pen_.setWidth(1);
    }

    painter->setPen(pen_);
    painter->setBrush(brush_);

    // Draw rectangle
    painter->drawRect(bbox);

    // Draw name ABOVE the rectangle (outside)
    QString name = QString::fromStdString(inst_->getName());
    if (!name.isEmpty()) {
        painter->setFont(nameFont_);
        QFontMetrics fm(nameFont_);
        int textWidth = fm.horizontalAdvance(name);
        qreal nameX = bbox.center().x() - textWidth / 2.0;
        qreal nameY = bbox.top() - 15;  // More offset above
        painter->drawText(static_cast<int>(nameX), static_cast<int>(nameY), name);
    }

    // Draw module name BELOW the rectangle (outside)
    QString moduleName = QString::fromStdString(inst_->getModuleName());
    if (!moduleName.isEmpty()) {
        painter->setFont(moduleFont_);
        QFontMetrics fm(moduleFont_);
        int textWidth = fm.horizontalAdvance(moduleName);
        qreal moduleX = bbox.center().x() - textWidth / 2.0;
        qreal moduleY = bbox.bottom() + 20;  // More offset below
        painter->drawText(static_cast<int>(moduleX), static_cast<int>(moduleY), moduleName);
    }
}

void InstGraphicsItem::setLayout(InstLayout* layout) {
    layout_ = layout;
    update();
}

void InstGraphicsItem::setInst(std::shared_ptr<Inst> inst) {
    inst_ = inst;
    update();
}

void InstGraphicsItem::setSelected(bool selected) {
    QGraphicsItem::setSelected(selected);
    update();
}

QPainterPath InstGraphicsItem::shape() const {
    QPainterPath path;
    if (layout_) {
        path.addRect(layout_->getBoundingBox());
    }
    return path;
}
