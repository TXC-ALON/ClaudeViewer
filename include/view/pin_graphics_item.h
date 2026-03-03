/**
 * @file pin_graphics_item.h
 * @brief Pin graphics item for schematic display.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_VIEW_PIN_GRAPHICS_ITEM_H
#define SCHEMATIC_VIEWER_VIEW_PIN_GRAPHICS_ITEM_H

#include "layout/layout_info.h"
#include "model/pin.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsItem>
#include <QPen>

/**
 * @brief PinGraphicsItem renders a Pin with line and connection status marker.
 *
 * Display rules:
 * - INPUT: line segment on LEFT side of Inst, pointing inward
 * - OUTPUT: line segment on RIGHT side of Inst, pointing outward
 * - Connection marker (at outer end):
 *   - 1 connection: green diamond (8x8)
 *   - multiple connections: orange diamond (8x8)
 *   - no connection: red circle (diameter 6)
 */
class PinGraphicsItem : public QGraphicsItem {
public:
    /**
     * @brief Constructor.
     * @param pin Shared pointer to Pin.
     * @param layout Pointer to PinLayout.
     * @param instBoundingBox Bounding box of parent Inst.
     */
    explicit PinGraphicsItem(std::shared_ptr<Pin> pin, PinLayout* layout, const QRectF& instBoundingBox);

    /**
     * @brief Get the bounding rectangle.
     * @return QRectF bounding rectangle.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Paint the Pin.
     * @param painter QPainter.
     * @param option Style options.
     * @param widget Parent widget.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    /**
     * @brief Update layout.
     * @param layout New layout.
     * @param instBoundingBox New Inst bounding box.
     */
    void setLayout(PinLayout* layout, const QRectF& instBoundingBox);

    /**
     * @brief Update Pin data.
     * @param pin New Pin.
     */
    void setPin(std::shared_ptr<Pin> pin);

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
    std::shared_ptr<Pin> pin_;       ///< Pin object.
    PinLayout* layout_ = nullptr;     ///< Layout information.
    QRectF instBoundingBox_;          ///< Parent Inst bounding box.

    // Visual properties
    QPen pen_;
    QFont nameFont_;
    QColor inputColor_;
    QColor outputColor_;

    // Connection marker colors
    static const QColor ONE_CONNECTION_COLOR_;
    static const QColor MULTI_CONNECTION_COLOR_;
    static const QColor NO_CONNECTION_COLOR_;

    // Sizes
    static constexpr qreal LINE_LENGTH_ = 10.0;
    static constexpr qreal MARKER_SIZE_ = 8.0;
    static constexpr qreal CIRCLE_RADIUS_ = 3.0;

    /**
     * @brief Draw connection marker based on connection count.
     * @param painter QPainter.
     * @param pos Position to draw marker.
     */
    void drawConnectionMarker(QPainter* painter, const QPointF& pos) const;

    /**
     * @brief Compute the line segment points.
     * @return Pair of (inner point, outer point).
     */
    std::pair<QPointF, QPointF> computeLinePoints() const;
};

#endif  // SCHEMATIC_VIEWER_VIEW_PIN_GRAPHICS_ITEM_H
