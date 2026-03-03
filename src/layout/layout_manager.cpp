/**
 * @file layout_manager.cpp
 * @brief Implementation of LayoutManager class.
 * @author
 * @date
 */

#include "layout/layout_manager.h"

#include <cmath>
#include <map>

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
    constexpr qreal TOP_INST_PADDING = 30.0;  // Padding inside TOP inst for children

    // Get all Insts
    auto insts = objMgr_.getAllInsts();

    // First pass: collect TOP insts and their children
    std::vector<ObjectID> topInsts;
    std::map<ObjectID, std::vector<ObjectID>> childrenMap;  // parentId -> childIds

    for (const auto& inst : insts) {
        ObjectID parentId = inst->getParentId();
        if (inst->isTop()) {
            topInsts.push_back(inst->getId());
        } else {
            childrenMap[parentId].push_back(inst->getId());
        }
    }

    // Layout each TOP inst and its children
    int topCol = 0;
    constexpr int MAX_TOP_COLS = 3;

    for (ObjectID topId : topInsts) {
        // Start position for this TOP inst
        qreal topX = topCol * 400.0 + 50.0;
        qreal topY = 50.0;

        // Layout children first inside a temporary area
        auto children = childrenMap[topId];
        if (!children.empty()) {
            // Layout children in a grid inside the TOP inst area
            int childCol = 0;
            int childRow = 0;
            constexpr int MAX_CHILD_COLS = 3;

            for (ObjectID childId : children) {
                qreal childX = topX + TOP_INST_PADDING + childCol * INST_SPACING_X;
                qreal childY = topY + TOP_INST_PADDING + childRow * INST_SPACING_Y;

                QRectF childBox(childX, childY, INST_WIDTH, INST_HEIGHT);
                updateInstLayout(childId, childBox);
                computeInstPinLayout(childId);

                childCol++;
                if (childCol >= MAX_CHILD_COLS) {
                    childCol = 0;
                    childRow++;
                }
            }

            // Calculate bounding box to contain all children
            QRectF childrenBounds;
            for (ObjectID childId : children) {
                auto* layout = getInstLayout(childId);
                if (layout) {
                    if (childrenBounds.isNull()) {
                        childrenBounds = layout->getBoundingBox();
                    } else {
                        childrenBounds = childrenBounds.united(layout->getBoundingBox());
                    }
                }
            }

            // Expand to include padding and text space
            qreal expandedWidth = childrenBounds.width() + TOP_INST_PADDING * 2;
            qreal expandedHeight = childrenBounds.height() + TOP_INST_PADDING * 2 + 40;  // +40 for text

            QRectF topBox(topX, topY, std::max(expandedWidth, INST_WIDTH), std::max(expandedHeight, INST_HEIGHT));
            updateInstLayout(topId, topBox);
        } else {
            // No children, use default size
            QRectF topBox(topX, topY, INST_WIDTH, INST_HEIGHT);
            updateInstLayout(topId, topBox);
        }

        computeInstPinLayout(topId);

        topCol++;
        if (topCol >= MAX_TOP_COLS) {
            topCol = 0;
        }
    }

    // Handle standalone non-TOP insts (orphans) with simple grid layout
    std::vector<ObjectID> orphanInsts;
    for (const auto& inst : insts) {
        if (!inst->isTop() && childrenMap[inst->getParentId()].empty()) {
            // This inst's parent is not in our inst list or has no children placed
            orphanInsts.push_back(inst->getId());
        }
    }

    // Simple grid for any remaining insts
    int col = 0;
    int row = 0;
    constexpr int MAX_COLS = 5;

    for (ObjectID orphanId : orphanInsts) {
        // Check if already laid out as child
        if (getInstLayout(orphanId)) continue;

        qreal x = col * INST_SPACING_X + 50.0;
        qreal y = row * INST_SPACING_Y + 300.0;  // Place below TOP insts

        QRectF boundingBox(x, y, INST_WIDTH, INST_HEIGHT);
        updateInstLayout(orphanId, boundingBox);
        computeInstPinLayout(orphanId);

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
