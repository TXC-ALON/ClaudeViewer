/**
 * @file object_manager.cpp
 * @brief Implementation of ObjectManager class.
 * @author
 * @date
 */

#include "model/object_manager.h"

#include <algorithm>

std::optional<std::shared_ptr<Inst>> ObjectManager::findInst(ObjectID id) {
    auto it = insts_.find(id);
    if (it != insts_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<Pin>> ObjectManager::findPin(ObjectID id) {
    auto it = pins_.find(id);
    if (it != pins_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<Net>> ObjectManager::findNet(ObjectID id) {
    auto it = nets_.find(id);
    if (it != nets_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::shared_ptr<Inst> ObjectManager::createInst(ObjectID id, std::string name) {
    auto inst = std::make_shared<Inst>(id);
    inst->setName(std::move(name));
    insts_[id] = inst;
    return inst;
}

std::shared_ptr<Pin> ObjectManager::createPin(ObjectID id, std::string name) {
    auto pin = std::make_shared<Pin>(id);
    pin->setName(std::move(name));
    pins_[id] = pin;
    return pin;
}

std::shared_ptr<Net> ObjectManager::createNet(ObjectID id, std::string name) {
    auto net = std::make_shared<Net>(id);
    net->setName(std::move(name));
    nets_[id] = net;
    return net;
}

void ObjectManager::removeInst(ObjectID id) {
    insts_.erase(id);
}

void ObjectManager::removePin(ObjectID id) {
    pins_.erase(id);
}

void ObjectManager::removeNet(ObjectID id) {
    nets_.erase(id);
}

std::vector<std::shared_ptr<Pin>> ObjectManager::getPinsByInst(ObjectID instId) {
    std::vector<std::shared_ptr<Pin>> result;
    for (auto& [id, pin] : pins_) {
        if (pin->getParentInstId() == instId) {
            result.push_back(pin);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Pin>> ObjectManager::getPinsByNet(ObjectID netId) {
    std::vector<std::shared_ptr<Pin>> result;
    for (auto& [id, pin] : pins_) {
        if (pin->getConnectedNetId() == netId) {
            result.push_back(pin);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Inst>> ObjectManager::getAllInsts() const {
    std::vector<std::shared_ptr<Inst>> result;
    result.reserve(insts_.size());
    for (auto& [id, inst] : insts_) {
        result.push_back(inst);
    }
    return result;
}

std::vector<std::shared_ptr<Pin>> ObjectManager::getAllPins() const {
    std::vector<std::shared_ptr<Pin>> result;
    result.reserve(pins_.size());
    for (auto& [id, pin] : pins_) {
        result.push_back(pin);
    }
    return result;
}

std::vector<std::shared_ptr<Net>> ObjectManager::getAllNets() const {
    std::vector<std::shared_ptr<Net>> result;
    result.reserve(nets_.size());
    for (auto& [id, net] : nets_) {
        result.push_back(net);
    }
    return result;
}

void ObjectManager::clear() {
    insts_.clear();
    pins_.clear();
    nets_.clear();
}
