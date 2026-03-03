/**
 * @file inst_graphics_item.h
 * @brief Inst graphics item for schematic display.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_VIEW_INST_GRAPHICS_ITEM_H
#define SCHEMATIC_VIEWER_VIEW_INST_GRAPHICS_ITEM_H

#include "layout/layout_info.h"
#include "model/inst.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsItem>
#include <QPen>

/**
 * @brief InstGraphicsItem renders an Inst as a rectangle with labels.
 */
class InstGraphicsItem : public QGraphicsItem {
public:
    /**
     * @brief Constructor.
     * @param inst Shared pointer to Inst.
     * @param layout Pointer to InstLayout.
     */
    explicit InstGraphicsItem(std::shared_ptr<Inst> inst, InstLayout* layout);

    /**
     * @brief Get the bounding rectangle.
     * @return QRectF bounding rectangle.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Paint the Inst.
     * @param painter QPainter.
     * @param option Style options.
     * @param widget Parent widget.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    /**
     * @brief Update layout.
     * @param layout New layout.
     */
    void setLayout(InstLayout* layout);

    /**
     * @brief Update Inst data.
     * @param inst New Inst.
     */
    void setInst(std::shared_ptr<Inst> inst);

    /**
     * @brief Check if this is a TOP instance.
     * @return true if TOP.
     */
    bool isTop() const { return inst_ && inst_->isTop(); }

    /**
     * @brief Set selection state.
     * @param selected Selection state.
     */
    void setSelected(bool selected);

protected:
    /**
     * @brief Get the shape path for collision detection.
     * @return QPainterPath.
     */
    QPainterPath shape() const override;

private:
    std::shared_ptr<Inst> inst_;     ///< Inst object.
    InstLayout* layout_ = nullptr;    ///< Layout information.

    // Visual properties
    QPen pen_;
    QBrush brush_;
    QFont nameFont_;
    QFont moduleFont_;
    QColor defaultPenColor_;
    QColor selectedPenColor_;
    QColor fillColor_;
};

#endif  // SCHEMATIC_VIEWER_VIEW_INST_GRAPHICS_ITEM_H
