add_subdirectory(utils)
add_subdirectory(tcp_client)
add_subdirectory(tcp_server)
add_subdirectory(game_logic)

add_executable(main main.cpp)
target_link_libraries(main config)
target_link_libraries(main server_controller)