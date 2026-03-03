/**
 * @file command_dispatcher.cpp
 * @brief Implementation of CommandDispatcher class.
 * @author
 * @date
 */

#include "parser/command_dispatcher.h"

#include <optional>

CommandDispatcher::CommandDispatcher(ObjectManager& objMgr)
    : objMgr_(objMgr) {}

bool CommandDispatcher::dispatch(const ParsedCommand& cmd) {
    if (cmd.action == "load") {
        dispatchLoad(cmd);
    } else if (cmd.action == "unload") {
        dispatchUnload(cmd);
    } else if (cmd.action == "attribute") {
        dispatchAttribute(cmd);
    } else if (cmd.action == "mark") {
        dispatchMark(cmd);
    } else {
        return false;
    }
    return true;
}

bool CommandDispatcher::dispatch(const std::string& jsonStr) {
    CommandParser parser;
    auto optCmd = parser.parse(jsonStr);
    if (!optCmd) {
        return false;
    }
    return dispatch(*optCmd);
}

void CommandDispatcher::dispatchLoad(const ParsedCommand& cmd) {
    if (cmd.type == "inst") {
        auto inst = objMgr_.createInst(cmd.id, "");
        if (cmd.data.contains("name")) {
            inst->setName(cmd.data["name"].get<std::string>());
        }
        if (cmd.data.contains("module_name")) {
            inst->setModuleName(cmd.data["module_name"].get<std::string>());
        }
        if (cmd.data.contains("parent_id")) {
            auto optParentId = CommandParser::parseObjectId(cmd.data["parent_id"]);
            if (optParentId) {
                inst->setParentId(*optParentId);
            }
        }
    } else if (cmd.type == "pin") {
        auto pin = objMgr_.createPin(cmd.id, "");
        if (cmd.data.contains("name")) {
            pin->setName(cmd.data["name"].get<std::string>());
        }
        if (cmd.data.contains("parent_id")) {
            auto optParentId = CommandParser::parseObjectId(cmd.data["parent_id"]);
            if (optParentId) {
                pin->setParentInstId(*optParentId);
            }
        }
        if (cmd.data.contains("direction")) {
            std::string dir = cmd.data["direction"].get<std::string>();
            if (dir == "input") {
                pin->setDirection(PinDirection::INPUT);
            } else if (dir == "output") {
                pin->setDirection(PinDirection::OUTPUT);
            }
        }
    } else if (cmd.type == "net") {
        auto net = objMgr_.createNet(cmd.id, "");
        if (cmd.data.contains("name")) {
            net->setName(cmd.data["name"].get<std::string>());
        }
    }
}

void CommandDispatcher::dispatchUnload(const ParsedCommand& cmd) {
    if (cmd.type == "inst") {
        objMgr_.removeInst(cmd.id);
    } else if (cmd.type == "pin") {
        objMgr_.removePin(cmd.id);
    } else if (cmd.type == "net") {
        objMgr_.removeNet(cmd.id);
    }
}

void CommandDispatcher::dispatchAttribute(const ParsedCommand& cmd) {
    if (cmd.type == "inst") {
        auto optInst = objMgr_.findInst(cmd.id);
        if (!optInst) {
            return;
        }
        auto inst = *optInst;
        if (cmd.data.contains("name")) {
            inst->setName(cmd.data["name"].get<std::string>());
        }
        if (cmd.data.contains("module_name")) {
            inst->setModuleName(cmd.data["module_name"].get<std::string>());
        }
        if (cmd.data.contains("parent_id")) {
            auto optParentId = CommandParser::parseObjectId(cmd.data["parent_id"]);
            if (optParentId) {
                inst->setParentId(*optParentId);
            }
        }
    } else if (cmd.type == "pin") {
        auto optPin = objMgr_.findPin(cmd.id);
        if (!optPin) {
            return;
        }
        auto pin = *optPin;
        if (cmd.data.contains("name")) {
            pin->setName(cmd.data["name"].get<std::string>());
        }
        if (cmd.data.contains("parent_id")) {
            auto optParentId = CommandParser::parseObjectId(cmd.data["parent_id"]);
            if (optParentId) {
                pin->setParentInstId(*optParentId);
            }
        }
        if (cmd.data.contains("direction")) {
            std::string dir = cmd.data["direction"].get<std::string>();
            if (dir == "input") {
                pin->setDirection(PinDirection::INPUT);
            } else if (dir == "output") {
                pin->setDirection(PinDirection::OUTPUT);
            }
        }
        if (cmd.data.contains("connectionnum")) {
            pin->setConnectionCount(cmd.data["connectionnum"].get<int>());
        }
        if (cmd.data.contains("hier_pin_id")) {
            auto optHierPinId = CommandParser::parseObjectId(cmd.data["hier_pin_id"]);
            if (optHierPinId) {
                pin->setHierPinId(*optHierPinId);
            }
        }
    } else if (cmd.type == "net") {
        auto optNet = objMgr_.findNet(cmd.id);
        if (!optNet) {
            return;
        }
        auto net = *optNet;
        if (cmd.data.contains("name")) {
            net->setName(cmd.data["name"].get<std::string>());
        }
    }
}

void CommandDispatcher::dispatchMark(const ParsedCommand& cmd) {
    // Mark is handled by the graphics layer for visual highlighting.
    // For now, we just validate the command.
    // TODO: Add mark handling (e.g., color, highlight state).
}
