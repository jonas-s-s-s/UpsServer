add_executable(server_test testBaseServer.h testBasicServerFunctionality.cpp)
AddTests(server_test)

target_link_libraries(server_test PRIVATE server_controller tcp_client)

