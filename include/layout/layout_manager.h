/**
 * @file layout_manager.h
 * @brief Layout manager for schematic layout information.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_LAYOUT_LAYOUT_MANAGER_H
#define SCHEMATIC_VIEWER_LAYOUT_LAYOUT_MANAGER_H

#include "layout/layout_info.h"
#include "model/object_manager.h"

#include <memory>
#include <unordered_map>
#include <vector>

/**
 * @brief LayoutManager manages layout information for all schematic objects.
 */
class LayoutManager {
public:
    /**
     * @brief Constructor.
     * @param objMgr Reference to ObjectManager.
     */
    explicit LayoutManager(ObjectManager& objMgr);

    // Layout queries

    /**
     * @brief Get Inst layout.
     * @param instId Inst ID.
     * @return Pointer to InstLayout, nullptr if not found.
     */
    InstLayout* getInstLayout(ObjectID instId);

    /**
     * @brief Get Pin layout.
     * @param pinId Pin ID.
     * @return Pointer to PinLayout, nullptr if not found.
     */
    PinLayout* getPinLayout(ObjectID pinId);

    /**
     * @brief Get Net layout.
     * @param netId Net ID.
     * @return Pointer to NetLayout, nullptr if not found.
     */
    NetLayout* getNetLayout(ObjectID netId);

    // Layout updates

    /**
     * @brief Update Inst layout.
     * @param instId Inst ID.
     * @param boundingBox Bounding box.
     */
    void updateInstLayout(ObjectID instId, const QRectF& boundingBox);

    /**
     * @brief Update Pin layout.
     * @param pinId Pin ID.
     * @param anchorPos Anchor position.
     * @param side Pin side.
     * @param offset Offset.
     */
    void updatePinLayout(ObjectID pinId, const QPointF& anchorPos, PinSide side, qreal offset);

    /**
     * @brief Update Net layout.
     * @param netId Net ID.
     * @param pathPoints Path points.
     */
    void updateNetLayout(ObjectID netId, const std::vector<QPointF>& pathPoints);

    // Auto layout

    /**
     * @brief Automatically compute layout for all Insts and Pins.
     */
    void autoLayout();

    /**
     * @brief Automatically compute routing for all Nets.
     */
    void autoRoute();

    /**
     * @brief Clear all layout information.
     */
    void clear();

private:
    /**
     * @brief Compute Pin side based on direction.
     * @param direction Pin direction.
     * @return Pin side.
     */
    PinSide computePinSide(PinDirection direction) const;

    /**
     * @brief Compute Pin layout for an Inst.
     * @param instId Inst ID.
     */
    void computeInstPinLayout(ObjectID instId);

    ObjectManager& objMgr_;
    std::unordered_map<ObjectID, std::unique_ptr<InstLayout>> instLayouts_;
    std::unordered_map<ObjectID, std::unique_ptr<PinLayout>> pinLayouts_;
    std::unordered_map<ObjectID, std::unique_ptr<NetLayout>> netLayouts_;
};

#endif  // SCHEMATIC_VIEWER_LAYOUT_LAYOUT_MANAGER_H
