/**
 * @file layout_manager.cpp
 * @brief Implementation of LayoutManager class.
 * @author
 * @date
 */

#include "layout/layout_manager.h"

#include <cmath>

LayoutManager::LayoutManager(ObjectManager& objMgr)
    : objMgr_(objMgr) {}

InstLayout* LayoutManager::getInstLayout(ObjectID instId) {
    auto it = instLayouts_.find(instId);
    if (it != instLayouts_.end()) {
        return it->second.get();
    }
    return nullptr;
}

PinLayout* LayoutManager::getPinLayout(ObjectID pinId) {
    auto it = pinLayouts_.find(pinId);
    if (it != pinLayouts_.end()) {
        return it->second.get();
    }
    return nullptr;
}

NetLayout* LayoutManager::getNetLayout(ObjectID netId) {
    auto it = netLayouts_.find(netId);
    if (it != netLayouts_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void LayoutManager::updateInstLayout(ObjectID instId, const QRectF& boundingBox) {
    auto it = instLayouts_.find(instId);
    if (it == instLayouts_.end()) {
        auto layout = std::make_unique<InstLayout>(instId);
        layout->setBoundingBox(boundingBox);
        layout->setPosition(boundingBox.center());
        instLayouts_[instId] = std::move(layout);
    } else {
        it->second->setBoundingBox(boundingBox);
        it->second->setPosition(boundingBox.center());
    }
    // Recompute pin layouts for this inst
    computeInstPinLayout(instId);
}

void LayoutManager::updatePinLayout(ObjectID pinId, const QPointF& anchorPos, PinSide side, qreal offset) {
    auto it = pinLayouts_.find(pinId);
    if (it == pinLayouts_.end()) {
        auto layout = std::make_unique<PinLayout>(pinId);
        layout->setAnchorPos(anchorPos);
        layout->setSide(side);
        layout->setOffset(offset);
        pinLayouts_[pinId] = std::move(layout);
    } else {
        it->second->setAnchorPos(anchorPos);
        it->second->setSide(side);
        it->second->setOffset(offset);
    }
}

void LayoutManager::updateNetLayout(ObjectID netId, const std::vector<QPointF>& pathPoints) {
    auto it = netLayouts_.find(netId);
    if (it == netLayouts_.end()) {
        auto layout = std::make_unique<NetLayout>(netId);
        layout->setPathPoints(pathPoints);
        netLayouts_[netId] = std::move(layout);
    } else {
        it->second->setPathPoints(pathPoints);
    }
}

void LayoutManager::autoLayout() {
    constexpr qreal INST_WIDTH = 80.0;
    constexpr qreal INST_HEIGHT = 60.0;
    constexpr qreal INST_SPACING_X = 120.0;
    constexpr qreal INST_SPACING_Y = 100.0;
    constexpr qreal PIN_OFFSET = 15.0;

    // Get all Insts
    auto insts = objMgr_.getAllInsts();

    // Simple grid layout
    int col = 0;
    int row = 0;
    constexpr int MAX_COLS = 5;

    for (const auto& inst : insts) {
        qreal x = col * INST_SPACING_X + 50.0;
        qreal y = row * INST_SPACING_Y + 50.0;

        QRectF boundingBox(x, y, INST_WIDTH, INST_HEIGHT);
        updateInstLayout(inst->getId(), boundingBox);

        // Compute pin positions
        computeInstPinLayout(inst->getId());

        col++;
        if (col >= MAX_COLS) {
            col = 0;
            row++;
        }
    }
}

void LayoutManager::autoRoute() {
    // TODO: Implement routing algorithm
    // For now, just create empty layouts for all nets
    auto nets = objMgr_.getAllNets();
    for (const auto& net : nets) {
        if (netLayouts_.find(net->getId()) == netLayouts_.end()) {
            netLayouts_[net->getId()] = std::make_unique<NetLayout>(net->getId());
        }
    }
}

void LayoutManager::clear() {
    instLayouts_.clear();
    pinLayouts_.clear();
    netLayouts_.clear();
}

PinSide LayoutManager::computePinSide(PinDirection direction) const {
    return (direction == PinDirection::INPUT) ? PinSide::LEFT : PinSide::RIGHT;
}

void LayoutManager::computeInstPinLayout(ObjectID instId) {
    auto optInst = objMgr_.findInst(instId);
    if (!optInst) {
        return;
    }

    auto instLayout = getInstLayout(instId);
    if (!instLayout) {
        return;
    }

    QRectF bbox = instLayout->getBoundingBox();
    const qreal PIN_EXT_OFFSET = 10.0;  // Pin extends outside Inst

    // Get pins for this inst
    auto pins = objMgr_.getPinsByInst(instId);

    // Separate INPUT and OUTPUT pins
    std::vector<ObjectID> inputPins;
    std::vector<ObjectID> outputPins;

    for (const auto& pin : pins) {
        if (pin->getDirection() == PinDirection::INPUT) {
            inputPins.push_back(pin->getId());
        } else {
            outputPins.push_back(pin->getId());
        }
    }

    // Layout INPUT pins on LEFT side
    qreal leftOffset = 0.0;
    qreal leftSpacing = bbox.height() / (inputPins.size() + 1);
    for (ObjectID pinId : inputPins) {
        qreal y = bbox.top() + leftSpacing * (++leftOffset);
        QPointF anchorPos(bbox.left(), y);
        updatePinLayout(pinId, anchorPos, PinSide::LEFT, leftOffset * leftSpacing);
    }

    // Layout OUTPUT pins on RIGHT side
    qreal rightOffset = 0.0;
    qreal rightSpacing = bbox.height() / (outputPins.size() + 1);
    for (ObjectID pinId : outputPins) {
        qreal y = bbox.top() + rightSpacing * (++rightOffset);
        QPointF anchorPos(bbox.right(), y);
        updatePinLayout(pinId, anchorPos, PinSide::RIGHT, rightOffset * rightSpacing);
    }
}
