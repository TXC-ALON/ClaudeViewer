/**
 * @file object_manager.h
 * @brief Object manager for schematic objects.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_MODEL_OBJECT_MANAGER_H
#define SCHEMATIC_VIEWER_MODEL_OBJECT_MANAGER_H

#include "model/inst.h"
#include "model/net.h"
#include "model/obj.h"
#include "model/pin.h"

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

/**
 * @brief ObjectManager manages all schematic objects (Inst, Pin, Net).
 */
class ObjectManager {
public:
    /**
     * @brief Default constructor.
     */
    ObjectManager() = default;

    // Object query - returns std::optional

    /**
     * @brief Find an instance by ID.
     * @param id Instance ID.
     * @return Optional containing shared_ptr to Inst if found.
     */
    std::optional<std::shared_ptr<Inst>> findInst(ObjectID id);

    /**
     * @brief Find a pin by ID.
     * @param id Pin ID.
     * @return Optional containing shared_ptr to Pin if found.
     */
    std::optional<std::shared_ptr<Pin>> findPin(ObjectID id);

    /**
     * @brief Find a net by ID.
     * @param id Net ID.
     * @return Optional containing shared_ptr to Net if found.
     */
    std::optional<std::shared_ptr<Net>> findNet(ObjectID id);

    // Object creation

    /**
     * @brief Create a new instance.
     * @param id Instance ID.
     * @param name Instance name.
     * @return Shared pointer to created Inst.
     */
    std::shared_ptr<Inst> createInst(ObjectID id, std::string name);

    /**
     * @brief Create a new pin.
     * @param id Pin ID.
     * @param name Pin name.
     * @return Shared pointer to created Pin.
     */
    std::shared_ptr<Pin> createPin(ObjectID id, std::string name);

    /**
     * @brief Create a new net.
     * @param id Net ID.
     * @param name Net name.
     * @return Shared pointer to created Net.
     */
    std::shared_ptr<Net> createNet(ObjectID id, std::string name);

    // Object removal

    /**
     * @brief Remove an instance by ID.
     * @param id Instance ID.
     */
    void removeInst(ObjectID id);

    /**
     * @brief Remove a pin by ID.
     * @param id Pin ID.
     */
    void removePin(ObjectID id);

    /**
     * @brief Remove a net by ID.
     * @param id Net ID.
     */
    void removeNet(ObjectID id);

    // Relationship queries

    /**
     * @brief Get all pins belonging to an instance.
     * @param instId Instance ID.
     * @return Vector of shared_ptr to Pin objects.
     */
    std::vector<std::shared_ptr<Pin>> getPinsByInst(ObjectID instId);

    /**
     * @brief Get all pins connected to a net.
     * @param netId Net ID.
     * @return Vector of shared_ptr to Pin objects.
     */
    std::vector<std::shared_ptr<Pin>> getPinsByNet(ObjectID netId);

    /**
     * @brief Get all instances.
     * @return Vector of shared_ptr to Inst objects.
     */
    std::vector<std::shared_ptr<Inst>> getAllInsts() const;

    /**
     * @brief Get all pins.
     * @return Vector of shared_ptr to Pin objects.
     */
    std::vector<std::shared_ptr<Pin>> getAllPins() const;

    /**
     * @brief Get all nets.
     * @return Vector of shared_ptr to Net objects.
     */
    std::vector<std::shared_ptr<Net>> getAllNets() const;

    /**
     * @brief Clear all objects.
     */
    void clear();

private:
    std::unordered_map<ObjectID, std::shared_ptr<Inst>> insts_;
    std::unordered_map<ObjectID, std::shared_ptr<Pin>> pins_;
    std::unordered_map<ObjectID, std::shared_ptr<Net>> nets_;
};

#endif  // SCHEMATIC_VIEWER_MODEL_OBJECT_MANAGER_H
