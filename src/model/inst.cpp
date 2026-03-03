/**
 * @file inst.cpp
 * @brief Implementation of Inst class.
 * @author
 * @date
 */

#include "model/inst.h"

#include <algorithm>

Inst::Inst(ObjectID id) : Obj(id) {}

void Inst::removePin(ObjectID pinId) {
    pins_.erase(
        std::remove(pins_.begin(), pins_.end(), pinId),
        pins_.end()
    );
}
