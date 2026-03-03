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
    constexpr qreal MIN_SPACING_X = 100.0;  // Minimum horizontal spacing
    constexpr qreal MIN_SPACING_Y = 80.0;   // Minimum vertical spacing
    constexpr qreal TEXT_MARGIN = 5.0;       // Margin between text and box
    constexpr qreal NAME_TEXT_HEIGHT = 15.0; // Estimated name text height
    constexpr qreal MODULE_TEXT_HEIGHT = 12.0; // Estimated module text height

    // Get all Insts
    auto insts = objMgr_.getAllInsts();

    // Calculate full bounding box for each inst including text
    struct InstBox {
        ObjectID id;
        QRectF box;           // Main box
        QRectF fullBox;       // Box + text areas
    };
    std::vector<InstBox> instBoxes;

    // First pass: calculate boxes for all insts
    for (const auto& inst : insts) {
        InstBox ib;
        ib.id = inst->getId();

        // Estimate text width based on name/module length
        qreal nameWidth = inst->getName().length() * 7.0;  // ~7 pixels per char
        qreal moduleWidth = inst->getModuleName().length() * 6.0;  // ~6 pixels per char

        qreal textWidth = std::max({INST_WIDTH, nameWidth, moduleWidth});

        // Main box
        ib.box = QRectF(0, 0, INST_WIDTH, INST_HEIGHT);

        // Full box including text above and below
        ib.fullBox = QRectF(
            - (textWidth - INST_WIDTH) / 2,  // Expand left if text is wider
            -NAME_TEXT_HEIGHT - TEXT_MARGIN,
            textWidth,
            INST_HEIGHT + NAME_TEXT_HEIGHT + MODULE_TEXT_HEIGHT + TEXT_MARGIN * 2
        );

        instBoxes.push_back(ib);
    }

    // Sort by size (largest first) for better packing
    std::sort(instBoxes.begin(), instBoxes.end(),
        [](const InstBox& a, const InstBox& b) {
            return a.fullBox.width() * a.fullBox.height() > b.fullBox.width() * b.fullBox.height();
        });

    // Place boxes using simple greedy algorithm
    std::vector<QRectF> placedBoxes;
    qreal currentX = 50.0;
    qreal currentY = 50.0;
    qreal rowHeight = 0;
    qreal maxX = 0;

    for (auto& ib : instBoxes) {
        // Check if we need to start a new row
        if (currentX + ib.fullBox.width() > 800.0) {  // Wrap at 800 pixels
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
            for (const auto& placed : placedBoxes) {
                if (testBox.intersects(placed)) {
                    collision = true;
                    break;
                }
            }

            if (!collision) {
                // Position the actual box (not fullBox)
                QRectF actualBox(currentX, testY + NAME_TEXT_HEIGHT + TEXT_MARGIN,
                               INST_WIDTH, INST_HEIGHT);
                updateInstLayout(ib.id, actualBox);
                computeInstPinLayout(ib.id);

                // Store placed box (full box for collision detection)
                placedBoxes.push_back(testBox);

                currentX += ib.fullBox.width() + MIN_SPACING_X;
                rowHeight = std::max(rowHeight, ib.fullBox.height());
                maxX = std::max(maxX, currentX);
                placed = true;
            } else {
                testY += 10;  // Move down to avoid collision
                if (testY > 2000.0) {  // Safety limit
                    // Start new area
                    currentX = maxX + MIN_SPACING_X;
                    currentY = 50.0;
                    testY = currentY;
                    placedBoxes.clear();
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
