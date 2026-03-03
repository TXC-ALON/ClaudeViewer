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

// Helper struct for layout calculation
namespace {
struct InstBox {
    ObjectID id;
    QRectF box;           // Main box
    QRectF fullBox;       // Box + text areas
};
}

void LayoutManager::autoLayout() {
    constexpr qreal INST_WIDTH = 80.0;
    constexpr qreal INST_HEIGHT = 60.0;
    constexpr qreal MIN_SPACING_X = 100.0;  // Minimum horizontal spacing
    constexpr qreal MIN_SPACING_Y = 80.0;   // Minimum vertical spacing
    constexpr qreal TEXT_MARGIN = 5.0;       // Margin between text and box
    constexpr qreal NAME_TEXT_HEIGHT = 15.0;  // Estimated name text height
    constexpr qreal MODULE_TEXT_HEIGHT = 12.0; // Estimated module text height
    constexpr qreal TOP_PADDING = 40.0;      // Padding inside TOP inst for children

    // Get all Insts
    auto insts = objMgr_.getAllInsts();

    // Step 1: Handle nested hierarchy - place children inside parent TOP inst
    // Collect TOP insts and their children
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

        auto children = childrenMap[topId];
        if (!children.empty()) {
            // Layout children in a grid inside the TOP inst area
            int childCol = 0;
            int childRow = 0;
            constexpr int MAX_CHILD_COLS = 3;

            // First pass: place children
            for (ObjectID childId : children) {
                qreal childX = topX + TOP_PADDING + childCol * MIN_SPACING_X;
                qreal childY = topY + TOP_PADDING + NAME_TEXT_HEIGHT + TEXT_MARGIN + childRow * MIN_SPACING_Y;

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
            qreal expandedWidth = childrenBounds.width() + TOP_PADDING * 2;
            qreal expandedHeight = childrenBounds.height() + TOP_PADDING * 2 + NAME_TEXT_HEIGHT + MODULE_TEXT_HEIGHT + TEXT_MARGIN * 2;

            QRectF topBox(topX, topY, std::max(expandedWidth, INST_WIDTH), std::max(expandedHeight, INST_HEIGHT));
            updateInstLayout(topId, topBox);
        } else {
            // No children, use default size
            QRectF topBox(topX, topY, INST_WIDTH, INST_HEIGHT + NAME_TEXT_HEIGHT + MODULE_TEXT_HEIGHT + TEXT_MARGIN * 2);
            updateInstLayout(topId, topBox);
        }

        computeInstPinLayout(topId);

        topCol++;
        if (topCol >= MAX_TOP_COLS) {
            topCol = 0;
        }
    }

    // Step 2: Handle standalone non-TOP insts with non-overlapping algorithm
    // Find insts that are not placed yet (orphans or children of non-existent parents)
    std::vector<InstBox> orphanBoxes;

    for (const auto& inst : insts) {
        // Skip if already placed (as TOP or child)
        if (getInstLayout(inst->getId())) continue;

        InstBox ib;
        ib.id = inst->getId();

        // Estimate text width based on name/module length
        qreal nameWidth = inst->getName().length() * 7.0;
        qreal moduleWidth = inst->getModuleName().length() * 6.0;
        qreal textWidth = std::max({INST_WIDTH, nameWidth, moduleWidth});

        ib.box = QRectF(0, 0, INST_WIDTH, INST_HEIGHT);
        ib.fullBox = QRectF(
            - (textWidth - INST_WIDTH) / 2,
            -NAME_TEXT_HEIGHT - TEXT_MARGIN,
            textWidth,
            INST_HEIGHT + NAME_TEXT_HEIGHT + MODULE_TEXT_HEIGHT + TEXT_MARGIN * 2
        );

        orphanBoxes.push_back(ib);
    }

    if (!orphanBoxes.empty()) {
        // Sort by size for better packing
        std::sort(orphanBoxes.begin(), orphanBoxes.end(),
            [](const InstBox& a, const InstBox& b) {
                return a.fullBox.width() * a.fullBox.height() > b.fullBox.width() * b.fullBox.height();
            });

        // Place boxes using greedy algorithm
        std::vector<QRectF> placedBoxes;
        qreal currentX = 50.0;
        qreal currentY = 350.0;  // Place below TOP insts
        qreal rowHeight = 0;
        qreal maxX = 0;

        for (auto& ib : orphanBoxes) {
            // Check if we need to start a new row
            if (currentX + ib.fullBox.width() > 800.0) {
                currentX = 50.0;
                currentY += rowHeight + MIN_SPACING_Y;
                rowHeight = 0;
            }

            // Check for collision and find non-overlapping position
            bool placed = false;
            qreal testY = currentY;

            while (!placed) {
                QRectF testBox(currentX + ib.fullBox.left(),
                              testY + ib.fullBox.top(),
                              ib.fullBox.width(),
                              ib.fullBox.height());

                bool collision = false;
                for (const auto& placedBox : placedBoxes) {
                    if (testBox.intersects(placedBox)) {
                        collision = true;
                        break;
                    }
                }

                if (!collision) {
                    QRectF actualBox(currentX, testY + NAME_TEXT_HEIGHT + TEXT_MARGIN,
                                   INST_WIDTH, INST_HEIGHT);
                    updateInstLayout(ib.id, actualBox);
                    computeInstPinLayout(ib.id);

                    placedBoxes.push_back(testBox);

                    currentX += ib.fullBox.width() + MIN_SPACING_X;
                    rowHeight = std::max(rowHeight, ib.fullBox.height());
                    maxX = std::max(maxX, currentX);
                    placed = true;
                } else {
                    testY += 10;
                    if (testY > 2000.0) {
                        currentX = maxX + MIN_SPACING_X;
                        currentY = 50.0;
                        testY = currentY;
                        placedBoxes.clear();
                    }
                }
            }
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
