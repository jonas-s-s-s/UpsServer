cmake_minimum_required(VERSION 3.20.0)
project(UpsServer VERSION 1.0.0 DESCRIPTION "Game server KIV/UPS ZS2023" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/module")

# Download all external dependencies into the extern folder
set(FETCHCONTENT_BASE_DIR "${PROJECT_SOURCE_DIR}/extern/cmake_deps")

# Disable in-source builds
include(NoInSourceBuilds)

# Set up the spdlog logging framework
# By using link_libraries(), the spdlog library is
# linked to ever target created after this call
include(GetSpdlog)
link_libraries(spdlog::spdlog)

# Copies the server_config file to output build directory
configure_file(./server_config.txt bin/server_config.txt COPYONLY)

add_subdirectory(src bin)
# TODO: Add tests only when debug release is selected
add_subdirectory(test)