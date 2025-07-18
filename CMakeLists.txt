cmake_minimum_required(VERSION 3.10)

project(Reactor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

aux_source_directory(./test SRC_TEST)
aux_source_directory(./src SRC)

add_executable(EchoServerTest ${SRC} ${SRC_TEST})

include_directories(./include ./threadpool/include ./timer/include)

add_subdirectory(threadpool)
add_subdirectory(timer)

target_link_libraries(EchoServerTest 
    threadpool 
    timer
    pthread
)

option(BUILD_TESTING "Build tests" ON)
# target_include_directories(TcpServerTest 
#     PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/threadpool/include
# )