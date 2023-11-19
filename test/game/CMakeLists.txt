add_executable(game_test graphTestBase.h TestGraph.cpp)
AddTests(game_test)

target_link_libraries(game_test PRIVATE game)

