add_subdirectory(config)
add_subdirectory(linux_commons_lib)
add_subdirectory(network_commons_lib)
add_subdirectory(game_logic)

add_executable(main main.cpp)
target_link_libraries(main config)
target_link_libraries(main server_controller)