//
// Created by xPC on 11.09.2023.
// Modified: 08.11.2023
//

#include "LoggerConfig.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <filesystem>
#include <fstream>
#include <iostream>

/**
 * Function to prevent crashing if it's impossible to write logs.
 * @param path Path to a test log file folder
 * @return True if it's possible to write, otherwise false.
 */
bool canWriteLog(const std::string& path)
{
    // Creates a new directory if it doesn't exist
    try {
        std::filesystem::create_directory(path);
    } catch (std::exception& e) {
        // failed to create directory
        return false;
    }

    // Try to create file
    if (std::ofstream testFile{path + "/temptestlogfile123.txt"}; testFile.fail()) {
        // failed to create file
        return false;
    }
    // Remove the test file
    std::filesystem::remove(path + "/temptestlogfile123.txt");

    // Directory and file write permissions are OK
    return true;
}

/**
 * Configures the global spdlog logger.
 */
void setupLogger()
{
    // Set up global SpdLogger - writes both to console and to file
    std::vector<spdlog::sink_ptr> sinks;
    // WARNING: Impossible to print anything to console on windows, unless running with admin privileges or with debugger attached
    // #ifndef _WIN32
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    // #endif
    // WARNING: If installed into "Program files", it's impossible to write logs
    if (canWriteLog("./log")) {
        sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("./log/logfile.txt", 23, 59));
    }
    auto combined_logger = std::make_shared<spdlog::logger>("GL", begin(sinks), end(sinks));
    combined_logger->set_level(spdlog::level::trace);
    spdlog::register_logger(combined_logger);
    spdlog::set_default_logger(combined_logger);
    spdlog::set_level(spdlog::level::trace);
}