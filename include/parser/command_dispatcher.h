/**
 * @file command_dispatcher.h
 * @brief Command dispatcher for schematic commands.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_PARSER_COMMAND_DISPATCHER_H
#define SCHEMATIC_VIEWER_PARSER_COMMAND_DISPATCHER_H

#include "model/object_manager.h"
#include "parser/command_parser.h"

#include <functional>
#include <optional>
#include <string>

/**
 * @brief CommandDispatcher dispatches parsed commands to ObjectManager.
 */
class CommandDispatcher {
public:
    /**
     * @brief Constructor.
     * @param objMgr Reference to ObjectManager.
     */
    explicit CommandDispatcher(ObjectManager& objMgr);

    /**
     * @brief Dispatch a parsed command.
     * @param cmd Parsed command.
     * @return true if successful.
     */
    bool dispatch(const ParsedCommand& cmd);

    /**
     * @brief Parse and dispatch a JSON command string.
     * @param jsonStr Raw JSON string.
     * @return true if successful.
     */
    bool dispatch(const std::string& jsonStr);

private:
    void dispatchLoad(const ParsedCommand& cmd);
    void dispatchUnload(const ParsedCommand& cmd);
    void dispatchAttribute(const ParsedCommand& cmd);
    void dispatchMark(const ParsedCommand& cmd);

    ObjectManager& objMgr_;
};

#endif  // SCHEMATIC_VIEWER_PARSER_COMMAND_DISPATCHER_H
