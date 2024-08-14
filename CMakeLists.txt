cmake_minimum_required(VERSION 3.20)

project(OS_LABS C CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include(FindThreads)

set(INC_DIR include)
set(SRC_DIR src)

add_executable(game_server ${SRC_DIR}/server.cpp ${SRC_DIR}/utils.cpp 
            ${INC_DIR}/utils.h ${SRC_DIR}/shared_memory.cpp ${INC_DIR}/shared_memory.h)
target_include_directories(game_server PRIVATE ${INC_DIR})

add_executable(game_client ${SRC_DIR}/client.cpp ${SRC_DIR}/utils.cpp 
            ${INC_DIR}/utils.h ${SRC_DIR}/shared_memory.cpp ${INC_DIR}/shared_memory.h)
target_include_directories(game_client PRIVATE ${INC_DIR})

