# timer/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(timer)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加头文件路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include  # 如果依赖上级目录的头文件
)

# 生成静态库
add_library(timer STATIC
    src/timer.cc
    src/heaptimer.cc
)

# 添加测试可执行文件
if(BUILD_TESTING)
    enable_testing()
    
    # 测试用例配置
    add_executable(TimerTest 
        test/TimerTest.cc
    )
    
    # 链接库和依赖
    target_link_libraries(TimerTest
        timer
    )
    
    # 添加测试到CTest
    add_test(NAME TimerTest 
            COMMAND TimerTest)
endif()