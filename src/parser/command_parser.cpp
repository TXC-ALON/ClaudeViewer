/**
 * @file command_parser.cpp
 * @brief Implementation of CommandParser class.
 * @author
 * @date
 */

#include "parser/command_parser.h"

#include <sstream>

std::optional<ParsedCommand> CommandParser::parse(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);

        // Check required fields
        if (!j.contains("action") || !j.contains("type") || !j.contains("id")) {
            return std::nullopt;
        }

        ParsedCommand cmd;

        // Parse action
        if (!j["action"].is_string()) {
            return std::nullopt;
        }
        cmd.action = j["action"].get<std::string>();

        // Parse type
        if (!j["type"].is_string()) {
            return std::nullopt;
        }
        cmd.type = j["type"].get<std::string>();

        // Parse id
        auto optId = parseObjectId(j["id"]);
        if (!optId) {
            return std::nullopt;
        }
        cmd.id = *optId;

        // Collect remaining fields
        json data;
        for (auto& item : j.items()) {
            if (item.key() != "action" && item.key() != "type" && item.key() != "id") {
                data[item.key()] = item.value();
            }
        }
        cmd.data = data;

        // Validate
        if (!validate(cmd)) {
            return std::nullopt;
        }

        return cmd;

    } catch (const json::parse_error&) {
        return std::nullopt;
    } catch (const json::type_error&) {
        return std::nullopt;
    }
}

bool CommandParser::validate(const ParsedCommand& cmd) const {
    if (!isValidAction(cmd.action)) {
        return false;
    }
    if (!isValidType(cmd.type)) {
        return false;
    }
    return true;
}

std::optional<ObjectID> CommandParser::parseObjectId(const json& val) {
    try {
        if (val.is_string()) {
            std::string str = val.get<std::string>();
            return std::stoull(str);
        } else if (val.is_number_integer()) {
            return val.get<ObjectID>();
        }
        return std::nullopt;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

bool CommandParser::isValidAction(const std::string& action) const {
    return action == "load" || action == "unload"
        || action == "attribute" || action == "mark";
}

bool CommandParser::isValidType(const std::string& type) const {
    return type == "inst" || type == "pin" || type == "net";
}
