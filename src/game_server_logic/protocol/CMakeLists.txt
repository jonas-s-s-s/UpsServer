add_library(protocol BufferedParser.cpp serializer.cpp ProtocolData.cpp)
target_include_directories(protocol PUBLIC include)
add_executable(testprot main.cpp)
target_link_libraries(testprot protocol)
