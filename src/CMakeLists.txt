cmake_minimum_required(VERSION 3.20)

set(SOURCE_LIB ipcclient.cpp dbreader.cpp appInfo.cpp)

add_library(src-lib STATIC ${SOURCE_LIB})

find_package(Qt6 REQUIRED COMPONENTS Network)

target_link_libraries(src-lib PRIVATE Qt6::Core Qt6::Network)
