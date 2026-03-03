/**
 * @file schematic_scene.h
 * @brief Schematic scene for managing all graphics items.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_VIEW_SCHEMATIC_SCENE_H
#define SCHEMATIC_VIEWER_VIEW_SCHEMATIC_SCENE_H

#include "model/obj.h"
#include "layout/layout_manager.h"
#include "model/object_manager.h"
#include "view/inst_graphics_item.h"
#include "view/text_item.h"
#include "view/pin_graphics_item.h"

#include <QGraphicsScene>
#include <memory>
#include <unordered_map>

/**
 * @brief SchematicScene manages all graphics items for the schematic.
 */
class SchematicScene : public QGraphicsScene {
public:
    /**
     * @brief Constructor.
     * @param objMgr Reference to ObjectManager.
     * @param layoutMgr Reference to LayoutManager.
     */
    explicit SchematicScene(ObjectManager& objMgr, LayoutManager& layoutMgr);

    /**
     * @brief Destructor.
     */
    ~SchematicScene() override;

    /**
     * @brief Rebuild all graphics items from ObjectManager and LayoutManager.
     */
    void rebuild();

    /**
     * @brief Add or update a single Inst.
     * @param instId Inst ID.
     */
    void updateInst(ObjectID instId);

    /**
     * @brief Add or update a single Pin.
     * @param pinId Pin ID.
     */
    void updatePin(ObjectID pinId);

    /**
     * @brief Remove an Inst.
     * @param instId Inst ID.
     */
    void removeInst(ObjectID instId);

    /**
     * @brief Remove a Pin.
     * @param pinId Pin ID.
     */
    void removePin(ObjectID pinId);

    /**
     * @brief Clear all items.
     */
    void clearAll();

    /**
     * @brief Run auto layout and rebuild.
     */
    void runAutoLayout();

private:
    /**
     * @brief Get or create InstGraphicsItem.
     * @param instId Inst ID.
     * @return Pointer to InstGraphicsItem.
     */
    InstGraphicsItem* getOrCreateInstItem(ObjectID instId);

    /**
     * @brief Get or create PinGraphicsItem.
     * @param pinId Pin ID.
     * @return Pointer to PinGraphicsItem.
     */
    PinGraphicsItem* getOrCreatePinItem(ObjectID pinId);

    ObjectManager& objMgr_;
    LayoutManager& layoutMgr_;

    std::unordered_map<ObjectID, std::unique_ptr<InstGraphicsItem>> instItems_;
    std::unordered_map<ObjectID, std::unique_ptr<PinGraphicsItem>> pinItems_;

    // Label items for inst name and module name
    std::unordered_map<ObjectID, std::unique_ptr<TextItem>> nameLabelItems_;
    std::unordered_map<ObjectID, std::unique_ptr<TextItem>> moduleLabelItems_;
};

#endif  // SCHEMATIC_VIEWER_VIEW_SCHEMATIC_SCENE_H
