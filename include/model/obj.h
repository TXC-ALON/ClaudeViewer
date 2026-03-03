/**
 * @file obj.h
 * @brief Object base class for schematic elements.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_MODEL_OBJ_H
#define SCHEMATIC_VIEWER_MODEL_OBJ_H

#include <cstdint>
#include <memory>

/**
 * @brief Object ID type alias.
 */
using ObjectID = uint64_t;

/**
 * @brief Object type enumeration.
 */
enum class ObjType { INST, PIN, NET };

/**
 * @brief Pin direction enumeration.
 * @note INPUT pins are on the left side of Inst, OUTPUT pins are on the right side.
 */
enum class PinDirection { INPUT, OUTPUT };

/**
 * @brief Base class for all schematic objects.
 */
class Obj {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Obj() = default;

    /**
     * @brief Get the object type.
     * @return ObjType enum value.
     */
    virtual ObjType getType() const = 0;

    /**
     * @brief Get the object ID.
     * @return ObjectID value.
     */
    ObjectID getId() const { return id_; }

protected:
    /**
     * @brief Constructor with ID.
     * @param id Object ID.
     */
    explicit Obj(ObjectID id) : id_(id) {}

    ObjectID id_;  ///< Object unique identifier.
};

#endif  // SCHEMATIC_VIEWER_MODEL_OBJ_H
