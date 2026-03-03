/**
 * @file pipeline_cli.cpp
 * @brief Command line tool for processing schematic commands.
 * @author
 * @date
 */

#include "layout/layout_manager.h"
#include "model/object_manager.h"
#include "parser/command_dispatcher.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief Process commands from a file or stdin.
 */
int main(int argc, char* argv[]) {
    ObjectManager objMgr;
    LayoutManager layoutMgr(objMgr);
    CommandDispatcher dispatcher(objMgr);

    std::vector<std::string> commands;

    if (argc > 1) {
        // Read from file
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << argv[1] << std::endl;
            return 1;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                commands.push_back(line);
            }
        }
    } else {
        // Read from stdin
        std::cout << "Enter JSON commands (one per line, empty line to execute):" << std::endl;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) {
                break;
            }
            commands.push_back(line);
        }
    }

    // Process commands
    for (const auto& cmd : commands) {
        std::cout << "Processing: " << cmd << std::endl;
        if (!dispatcher.dispatch(cmd)) {
            std::cerr << "Error processing command" << std::endl;
        }
    }

    // Run auto layout
    layoutMgr.autoLayout();

    // Print summary
    std::cout << "\n=== Schematic Summary ===" << std::endl;
    std::cout << "Instances: " << objMgr.getAllInsts().size() << std::endl;
    std::cout << "Pins: " << objMgr.getAllPins().size() << std::endl;
    std::cout << "Nets: " << objMgr.getAllNets().size() << std::endl;

    return 0;
}
