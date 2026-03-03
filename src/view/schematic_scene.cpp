/**
 * @file schematic_scene.cpp
 * @brief Implementation of SchematicScene class.
 * @author
 * @date
 */

#include "view/schematic_scene.h"
#include "view/text_item.h"

#include <QFont>

SchematicScene::SchematicScene(ObjectManager& objMgr, LayoutManager& layoutMgr)
    : objMgr_(objMgr), layoutMgr_(layoutMgr) {
    setBackgroundBrush(QBrush(QColor(240, 240, 240)));
}

SchematicScene::~SchematicScene() = default;

void SchematicScene::rebuild() {
    clearAll();

    // Create Inst items
    auto insts = objMgr_.getAllInsts();
    for (const auto& inst : insts) {
        updateInst(inst->getId());
    }

    // Create Pin items
    auto pins = objMgr_.getAllPins();
    for (const auto& pin : pins) {
        updatePin(pin->getId());
    }
}

void SchematicScene::updateInst(ObjectID instId) {
    auto optInst = objMgr_.findInst(instId);
    if (!optInst) {
        return;
    }

    auto inst = *optInst;
    auto layout = layoutMgr_.getInstLayout(instId);

    // Update main inst item
    auto* item = getOrCreateInstItem(instId);
    item->setInst(inst);
    if (layout) {
        item->setLayout(layout);
    }

    // Update or create name label
    QString name = QString::fromStdString(inst->getName());
    if (!name.isEmpty()) {
        auto it = nameLabelItems_.find(instId);
        if (it == nameLabelItems_.end()) {
            auto label = std::make_unique<TextItem>(name);
            QFont font("Sans-serif");
            font.setPointSize(9);
            font.setBold(true);
            label->setTextFont(font);
            label->setDefaultTextColor(Qt::black);
            addItem(label.get());
            nameLabelItems_[instId] = std::move(label);
        } else {
            it->second->setText(name);
        }
    }

    // Update or create module label
    QString moduleName = QString::fromStdString(inst->getModuleName());
    if (!moduleName.isEmpty()) {
        auto it = moduleLabelItems_.find(instId);
        if (it == moduleLabelItems_.end()) {
            auto label = std::make_unique<TextItem>(moduleName);
            QFont font("Sans-serif");
            font.setPointSize(8);
            label->setTextFont(font);
            label->setDefaultTextColor(Qt::black);
            addItem(label.get());
            moduleLabelItems_[instId] = std::move(label);
        } else {
            it->second->setText(moduleName);
        }
    }

    // Update label positions based on layout
    if (layout) {
        QRectF bbox = layout->getBoundingBox();
        auto nameIt = nameLabelItems_.find(instId);
        if (nameIt != nameLabelItems_.end()) {
            // Name above the box
            nameIt->second->updatePositionCentered(bbox.center().x(), bbox.top() - 15);
        }
        auto moduleIt = moduleLabelItems_.find(instId);
        if (moduleIt != moduleLabelItems_.end()) {
            // Module name below the box
            moduleIt->second->updatePositionCentered(bbox.center().x(), bbox.bottom() + 12);
        }
    }
}

void SchematicScene::updatePin(ObjectID pinId) {
    auto optPin = objMgr_.findPin(pinId);
    if (!optPin) {
        return;
    }

    auto pin = *optPin;
    auto layout = layoutMgr_.getPinLayout(pinId);

    // Get parent Inst bounding box
    QRectF instBoundingBox;
    auto optInst = objMgr_.findInst(pin->getParentInstId());
    if (optInst) {
        auto instLayout = layoutMgr_.getInstLayout(pin->getParentInstId());
        if (instLayout) {
            instBoundingBox = instLayout->getBoundingBox();
        }
    }

    auto* item = getOrCreatePinItem(pinId);
    item->setPin(pin);
    if (layout) {
        item->setLayout(layout, instBoundingBox);
    }
}

void SchematicScene::removeInst(ObjectID instId) {
    auto it = instItems_.find(instId);
    if (it != instItems_.end()) {
        removeItem(it->second.get());
        instItems_.erase(it);
    }

    // Also remove label items
    auto nameIt = nameLabelItems_.find(instId);
    if (nameIt != nameLabelItems_.end()) {
        removeItem(nameIt->second.get());
        nameLabelItems_.erase(nameIt);
    }
    auto moduleIt = moduleLabelItems_.find(instId);
    if (moduleIt != moduleLabelItems_.end()) {
        removeItem(moduleIt->second.get());
        moduleLabelItems_.erase(moduleIt);
    }
}

void SchematicScene::removePin(ObjectID pinId) {
    auto it = pinItems_.find(pinId);
    if (it != pinItems_.end()) {
        removeItem(it->second.get());
        pinItems_.erase(it);
    }
}

void SchematicScene::clearAll() {
    // Remove all items from scene
    for (auto& [id, item] : instItems_) {
        removeItem(item.get());
    }
    for (auto& [id, item] : pinItems_) {
        removeItem(item.get());
    }
    for (auto& [id, item] : nameLabelItems_) {
        removeItem(item.get());
    }
    for (auto& [id, item] : moduleLabelItems_) {
        removeItem(item.get());
    }

    instItems_.clear();
    pinItems_.clear();
    nameLabelItems_.clear();
    moduleLabelItems_.clear();
}

void SchematicScene::runAutoLayout() {
    layoutMgr_.autoLayout();
    layoutMgr_.autoRoute();
    rebuild();
}

InstGraphicsItem* SchematicScene::getOrCreateInstItem(ObjectID instId) {
    auto it = instItems_.find(instId);
    if (it != instItems_.end()) {
        return it->second.get();
    }

    auto optInst = objMgr_.findInst(instId);
    if (!optInst) {
        return nullptr;
    }

    auto inst = *optInst;
    auto layout = layoutMgr_.getInstLayout(instId);

    auto item = std::make_unique<InstGraphicsItem>(inst, layout);
    auto* rawPtr = item.get();
    addItem(rawPtr);
    instItems_[instId] = std::move(item);

    return rawPtr;
}

PinGraphicsItem* SchematicScene::getOrCreatePinItem(ObjectID pinId) {
    auto it = pinItems_.find(pinId);
    if (it != pinItems_.end()) {
        return it->second.get();
    }

    auto optPin = objMgr_.findPin(pinId);
    if (!optPin) {
        return nullptr;
    }

    auto pin = *optPin;
    auto layout = layoutMgr_.getPinLayout(pinId);

    // Get parent Inst bounding box
    QRectF instBoundingBox;
    auto optInst = objMgr_.findInst(pin->getParentInstId());
    if (optInst) {
        auto instLayout = layoutMgr_.getInstLayout(pin->getParentInstId());
        if (instLayout) {
            instBoundingBox = instLayout->getBoundingBox();
        }
    }

    auto item = std::make_unique<PinGraphicsItem>(pin, layout, instBoundingBox);
    auto* rawPtr = item.get();
    addItem(rawPtr);
    pinItems_[pinId] = std::move(item);

    return rawPtr;
}
