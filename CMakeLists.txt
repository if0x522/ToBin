cmake_minimum_required(VERSION 3.0)

# 工程名
project(ToBin)

# 设定工具链
set(CMAKE_C_COMPILER gcc)

# 头文件路径
include_directories(
    ${PROJECT_SOURCE_DIR}/inc 
    )
# C 语言代码
aux_source_directory(${PROJECT_SOURCE_DIR}/src test_SRC)
#生成二进制文件
add_executable(${PROJECT_NAME} main.c ${test_SRC})

