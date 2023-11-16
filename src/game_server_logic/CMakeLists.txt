# Everything that is needed for the game server to operate
add_library(game_server TcpGameServer.cpp IdleUsersRoom.cpp)
target_include_directories(game_server PUBLIC include)
target_link_libraries(game_server tcp_client tcp_server network_utils protocol eventfd_queue epoll_lib game)

# Controller links to the game_server, and other configuration utilities
add_library(server_controller ServerController.cpp)
target_include_directories(server_controller PUBLIC include)
target_link_libraries(server_controller config game_server)

add_subdirectory(protocol)
add_subdirectory(game)