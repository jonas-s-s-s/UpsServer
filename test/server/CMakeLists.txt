add_executable(server_test testBaseServerOneClient.h TestServerOneClient.cpp testBaseServerGame.h TestServerGame.cpp)
AddTests(server_test)

target_link_libraries(server_test PRIVATE server_controller tcp_client)

