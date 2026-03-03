/**
 * @file net.cpp
 * @brief Implementation of Net class.
 * @author
 * @date
 */

#include "model/net.h"

#include <algorithm>

Net::Net(ObjectID id) : Obj(id) {}

void Net::removePin(ObjectID pinId) {
    connectedPins_.erase(
        std::remove(connectedPins_.begin(), connectedPins_.end(), pinId),
        connectedPins_.end()
    );
}

bool Net::isPinConnected(ObjectID pinId) const {
    return std::find(connectedPins_.begin(), connectedPins_.end(), pinId)
           != connectedPins_.end();
}
