add_library(game RoomStructs.cpp)
target_include_directories(game PUBLIC include)
target_link_libraries(game eventfd_queue protocol)