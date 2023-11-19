add_library(game RoomStructs.cpp Game.cpp GameGraph.cpp)
target_include_directories(game PUBLIC include)
target_link_libraries(game eventfd_queue protocol epoll_lib)