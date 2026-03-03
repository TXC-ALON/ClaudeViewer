/**
 * @file net.h
 * @brief Net class for schematic circuit nets (signals).
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_MODEL_NET_H
#define SCHEMATIC_VIEWER_MODEL_NET_H

#include "obj.h"

#include <string>
#include <vector>

/**
 * @brief Net represents a circuit net (signal) in the schematic.
 * @note In Verilog, a net connects multiple pins. Each pin can only connect to one net.
 */
class Net : public Obj {
public:
    /**
     * @brief Constructor.
     * @param id Net ID.
     */
    explicit Net(ObjectID id);

    /**
     * @brief Get object type.
     * @return ObjType::NET.
     */
    ObjType getType() const override { return ObjType::NET; }

    // Setters

    /**
     * @brief Set net name.
     * @param name Net name.
     */
    void setName(std::string name) { name_ = std::move(name); }

    // Getters

    /**
     * @brief Get net name.
     * @return Reference to name string.
     */
    const std::string& getName() const { return name_; }

    /**
     * @brief Get all connected pin IDs.
     * @return Reference to pin ID vector.
     */
    const std::vector<ObjectID>& getConnectedPins() const { return connectedPins_; }

    /**
     * @brief Get number of connected pins.
     * @return Number of connected pins.
     */
    std::size_t getConnectionCount() const { return connectedPins_.size(); }

    // Pin management

    /**
     * @brief Add a pin to this net.
     * @param pinId Pin ID to add.
     */
    void addPin(ObjectID pinId) { connectedPins_.push_back(pinId); }

    /**
     * @brief Remove a pin from this net.
     * @param pinId Pin ID to remove.
     */
    void removePin(ObjectID pinId);

    /**
     * @brief Check if a pin is connected to this net.
     * @param pinId Pin ID to check.
     * @return true if pin is connected.
     */
    bool isPinConnected(ObjectID pinId) const;

private:
    std::string name_;                    ///< Net name.
    std::vector<ObjectID> connectedPins_;  ///< List of connected pin IDs.
};

#endif  // SCHEMATIC_VIEWER_MODEL_NET_H
