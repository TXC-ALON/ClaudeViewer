/**
 * @file layout_info.h
 * @brief Layout information classes for schematic elements.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_LAYOUT_LAYOUT_INFO_H
#define SCHEMATIC_VIEWER_LAYOUT_LAYOUT_INFO_H

#include "model/obj.h"

#include <QPointF>
#include <QRectF>
#include <vector>

/**
 * @brief Pin side enumeration.
 * @note LEFT corresponds to INPUT pins, RIGHT corresponds to OUTPUT pins.
 */
enum class PinSide { TOP, BOTTOM, LEFT, RIGHT };

/**
 * @brief Base class for layout information.
 */
class LayoutInfo {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~LayoutInfo() = default;

    /**
     * @brief Get the object ID.
     * @return ObjectID.
     */
    ObjectID getObjectId() const { return objectId_; }

protected:
    /**
     * @brief Constructor.
     * @param objectId Object ID.
     */
    explicit LayoutInfo(ObjectID objectId) : objectId_(objectId) {}

    ObjectID objectId_;  ///< Object ID.
};

/**
 * @brief Inst layout information.
 */
class InstLayout : public LayoutInfo {
public:
    /**
     * @brief Constructor.
     * @param instId Inst ID.
     */
    explicit InstLayout(ObjectID instId);

    /**
     * @brief Set bounding box.
     * @param rect Bounding rectangle.
     */
    void setBoundingBox(const QRectF& rect) { boundingBox_ = rect; }

    /**
     * @brief Set position (center point).
     * @param pos Position.
     */
    void setPosition(const QPointF& pos) { position_ = pos; }

    /**
     * @brief Get bounding box.
     * @return Bounding rectangle.
     */
    QRectF getBoundingBox() const { return boundingBox_; }

    /**
     * @brief Get position (center point).
     * @return Position.
     */
    QPointF getPosition() const { return position_; }

private:
    QRectF boundingBox_;  ///< Bounding box (x, y, width, height).
    QPointF position_;    ///< Center position.
};

/**
 * @brief Pin layout information.
 */
class PinLayout : public LayoutInfo {
public:
    /**
     * @brief Constructor.
     * @param pinId Pin ID.
     */
    explicit PinLayout(ObjectID pinId);

    /**
     * @brief Set anchor position on Inst boundary.
     * @param pos Anchor position.
     */
    void setAnchorPos(const QPointF& pos) { anchorPos_ = pos; }

    /**
     * @brief Set side (which side of Inst).
     * @param side Pin side.
     */
    void setSide(PinSide side) { side_ = side; }

    /**
     * @brief Set offset on the side.
     * @param offset Offset value.
     */
    void setOffset(qreal offset) { offset_ = offset; }

    /**
     * @brief Get anchor position.
     * @return Anchor position.
     */
    QPointF getAnchorPos() const { return anchorPos_; }

    /**
     * @brief Get side.
     * @return Pin side.
     */
    PinSide getSide() const { return side_; }

    /**
     * @brief Get offset.
     * @return Offset value.
     */
    qreal getOffset() const { return offset_; }

private:
    QPointF anchorPos_;  ///< Anchor position on Inst boundary.
    PinSide side_;       ///< Which side of Inst.
    qreal offset_;       ///< Offset on the side.
};

/**
 * @brief Net layout information.
 */
class NetLayout : public LayoutInfo {
public:
    /**
     * @brief Constructor.
     * @param netId Net ID.
     */
    explicit NetLayout(ObjectID netId);

    /**
     * @brief Set path points.
     * @param points Vector of path points.
     */
    void setPathPoints(std::vector<QPointF> points) { pathPoints_ = std::move(points); }

    /**
     * @brief Add a path point.
     * @param point Path point to add.
     */
    void addPathPoint(const QPointF& point) { pathPoints_.push_back(point); }

    /**
     * @brief Clear all path points.
     */
    void clearPathPoints() { pathPoints_.clear(); }

    /**
     * @brief Get path points.
     * @return Reference to path points vector.
     */
    const std::vector<QPointF>& getPathPoints() const { return pathPoints_; }

private:
    std::vector<QPointF> pathPoints_;  ///< Path points (bend points).
};

#endif  // SCHEMATIC_VIEWER_LAYOUT_LAYOUT_INFO_H
