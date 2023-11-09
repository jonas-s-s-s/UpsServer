add_library(protocol BufferedParser.cpp serializer.cpp ProtocolData.cpp)
target_include_directories(protocol PUBLIC include)
