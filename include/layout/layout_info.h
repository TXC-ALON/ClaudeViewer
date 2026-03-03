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
#include <QSizeF>
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

    /**
     * @brief Set name text size.
     * @param size Name text size.
     */
    void setNameTextSize(const QSizeF& size) { nameTextSize_ = size; }

    /**
     * @brief Get name text size.
     * @return Name text size.
     */
    QSizeF getNameTextSize() const { return nameTextSize_; }

    /**
     * @brief Set module text size.
     * @param size Module text size.
     */
    void setModuleTextSize(const QSizeF& size) { moduleTextSize_ = size; }

    /**
     * @brief Get module text size.
     * @return Module text size.
     */
    QSizeF getModuleTextSize() const { return moduleTextSize_; }

    /**
     * @brief Get full bounding box including text.
     * @return Full bounding rectangle including name above and module below.
     */
    QRectF getFullBoundingBox() const {
        qreal top = boundingBox_.top() - nameTextSize_.height() - TEXT_MARGIN_;
        qreal bottom = boundingBox_.bottom() + moduleTextSize_.height() + TEXT_MARGIN_;
        qreal left = boundingBox_.left();
        qreal right = boundingBox_.right();
        // Also consider text width
        if (nameTextSize_.width() > boundingBox_.width()) {
            qreal diff = nameTextSize_.width() - boundingBox_.width();
            left -= diff / 2;
            right += diff / 2;
        }
        if (moduleTextSize_.width() > boundingBox_.width()) {
            qreal diff = moduleTextSize_.width() - boundingBox_.width();
            left -= diff / 2;
            right += diff / 2;
        }
        return QRectF(left, top, right - left, bottom - top);
    }

    /**
     * @brief Set text margin.
     * @param margin Margin between text and box.
     */
    void setTextMargin(qreal margin) { TEXT_MARGIN_ = margin; }

    /**
     * @brief Get text margin.
     * @return Text margin.
     */
    qreal getTextMargin() const { return TEXT_MARGIN_; }

private:
    QRectF boundingBox_;  ///< Bounding box (x, y, width, height).
    QPointF position_;    ///< Center position.
    QSizeF nameTextSize_;    ///< Name text size.
    QSizeF moduleTextSize_;  ///< Module name text size.
    qreal TEXT_MARGIN_ = 5.0;  ///< Margin between text and box.
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
