/**
 * @file inst.h
 * @brief Inst (instance) class for schematic circuit instances.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_MODEL_INST_H
#define SCHEMATIC_VIEWER_MODEL_INST_H

#include "obj.h"

#include <string>
#include <vector>

/**
 * @brief Inst represents a circuit instance in the schematic.
 */
class Inst : public Obj {
public:
    /**
     * @brief Constructor.
     * @param id Instance ID.
     */
    explicit Inst(ObjectID id);

    /**
     * @brief Get object type.
     * @return ObjType::INST.
     */
    ObjType getType() const override { return ObjType::INST; }

    // Setters

    /**
     * @brief Set instance name.
     * @param name Instance name.
     */
    void setName(std::string name) { name_ = std::move(name); }

    /**
     * @brief Set module name.
     * @param name Module name.
     */
    void setModuleName(std::string name) { moduleName_ = std::move(name); }

    /**
     * @brief Set parent instance ID.
     * @param id Parent instance ID (0 means TOP).
     */
    void setParentId(ObjectID id) { parentId_ = id; }

    // Getters

    /**
     * @brief Get instance name.
     * @return Reference to name string.
     */
    const std::string& getName() const { return name_; }

    /**
     * @brief Get module name.
     * @return Reference to module name string.
     */
    const std::string& getModuleName() const { return moduleName_; }

    /**
     * @brief Get parent instance ID.
     * @return Parent ID (0 means TOP).
     */
    ObjectID getParentId() const { return parentId_; }

    /**
     * @brief Check if this is a TOP instance.
     * @return true if parentId is 0 or equals own ID.
     */
    bool isTop() const { return parentId_ == 0 || parentId_ == id_; }

    /**
     * @brief Get all pin IDs belonging to this instance.
     * @return Reference to pin ID vector.
     */
    const std::vector<ObjectID>& getPins() const { return pins_; }

    // Pin management

    /**
     * @brief Add a pin to this instance.
     * @param pinId Pin ID to add.
     */
    void addPin(ObjectID pinId) { pins_.push_back(pinId); }

    /**
     * @brief Remove a pin from this instance.
     * @param pinId Pin ID to remove.
     */
    void removePin(ObjectID pinId);

private:
    std::string name_;           ///< Instance name.
    std::string moduleName_;     ///< Module name.
    ObjectID parentId_ = 0;      ///< Parent instance ID (0 = TOP).
    std::vector<ObjectID> pins_; ///< List of pin IDs belonging to this instance.
};

#endif  // SCHEMATIC_VIEWER_MODEL_INST_H
