/**
 * @file command_parser.h
 * @brief Command parser for schematic JSON commands.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_PARSER_COMMAND_PARSER_H
#define SCHEMATIC_VIEWER_PARSER_COMMAND_PARSER_H

#include "model/obj.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using json = nlohmann::json;

/**
 * @brief Parsed command structure.
 */
struct ParsedCommand {
    std::string action;   ///< Action: load, unload, attribute, mark.
    std::string type;     ///< Object type: inst, pin, net.
    ObjectID id;          ///< Object ID.
    json data;            ///< Additional parameters.
};

/**
 * @brief CommandParser parses JSON command strings into ParsedCommand objects.
 */
class CommandParser {
public:
    /**
     * @brief Parse a JSON command string.
     * @param jsonStr Raw JSON string.
     * @return ParsedCommand if successful, std::nullopt if parsing fails.
     */
    std::optional<ParsedCommand> parse(const std::string& jsonStr);

    /**
     * @brief Validate the parsed command.
     * @param cmd Parsed command to validate.
     * @return true if valid.
     */
    bool validate(const ParsedCommand& cmd) const;

    /**
     * @brief Parse ObjectID from JSON value.
     * @param val JSON value.
     * @return ObjectID if valid.
     */
    static std::optional<ObjectID> parseObjectId(const json& val);

private:
    /**
     * @brief Validate action field.
     * @param action Action string.
     * @return true if valid.
     */
    bool isValidAction(const std::string& action) const;

    /**
     * @brief Validate type field.
     * @param type Type string.
     * @return true if valid.
     */
    bool isValidType(const std::string& type) const;
};

#endif  // SCHEMATIC_VIEWER_PARSER_COMMAND_PARSER_H
