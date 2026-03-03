/**
 * @file pin.h
 * @brief Pin class for schematic circuit pins.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_MODEL_PIN_H
#define SCHEMATIC_VIEWER_MODEL_PIN_H

#include "obj.h"

#include <optional>
#include <string>

/**
 * @brief Pin represents a circuit pin in the schematic.
 */
class Pin : public Obj {
public:
    /**
     * @brief Constructor.
     * @param id Pin ID.
     */
    explicit Pin(ObjectID id);

    /**
     * @brief Get object type.
     * @return ObjType::PIN.
     */
    ObjType getType() const override { return ObjType::PIN; }

    // Setters

    /**
     * @brief Set pin name.
     * @param name Pin name.
     */
    void setName(std::string name) { name_ = std::move(name); }

    /**
     * @brief Set parent instance ID.
     * @param id Parent instance ID.
     */
    void setParentInstId(ObjectID id) { parentInstId_ = id; }

    /**
     * @brief Set pin direction.
     * @param dir Pin direction (INPUT or OUTPUT).
     */
    void setDirection(PinDirection dir) { direction_ = dir; }

    /**
     * @brief Set connected net ID.
     * @param id Connected net ID (optional).
     */
    void setConnectedNetId(std::optional<ObjectID> id) { connectedNetId_ = id; }

    /**
     * @brief Set hierarchical pin ID.
     * @param id Hierarchical pin ID (optional).
     */
    void setHierPinId(std::optional<ObjectID> id) { hierPinId_ = id; }

    /**
     * @brief Set connection count.
     * @param count Number of connections.
     */
    void setConnectionCount(int count) { connectionCount_ = count; }

    // Getters

    /**
     * @brief Get pin name.
     * @return Reference to name string.
     */
    const std::string& getName() const { return name_; }

    /**
     * @brief Get parent instance ID.
     * @return Parent instance ID.
     */
    ObjectID getParentInstId() const { return parentInstId_; }

    /**
     * @brief Get pin direction.
     * @return Pin direction.
     */
    PinDirection getDirection() const { return direction_; }

    /**
     * @brief Get connected net ID.
     * @return Optional net ID.
     */
    std::optional<ObjectID> getConnectedNetId() const { return connectedNetId_; }

    /**
     * @brief Get hierarchical pin ID.
     * @return Optional hierarchical pin ID.
     */
    std::optional<ObjectID> getHierPinId() const { return hierPinId_; }

    /**
     * @brief Get connection count.
     * @return Number of connections.
     */
    int getConnectionCount() const { return connectionCount_; }

    /**
     * @brief Check if pin has a connection.
     * @return true if connectionCount > 0.
     */
    bool hasConnection() const { return connectionCount_ > 0; }

    /**
     * @brief Check if this is a hierarchical pin.
     * @return true if hierPinId has value.
     */
    bool isHierarchical() const { return hierPinId_.has_value(); }

private:
    std::string name_;                      ///< Pin name.
    ObjectID parentInstId_ = 0;              ///< Parent instance ID.
    PinDirection direction_ = PinDirection::INPUT;  ///< Pin direction (default INPUT).
    std::optional<ObjectID> connectedNetId_; ///< Connected net ID (optional).
    std::optional<ObjectID> hierPinId_;     ///< Hierarchical pin ID (optional).
    int connectionCount_ = 0;                ///< Number of connections.
};

#endif  // SCHEMATIC_VIEWER_MODEL_PIN_H
