cmake_minimum_required(VERSION 3.16)
project(SERVER)

set(SOURCE_DIR src)
file(GLOB C_FILES "${SOURCE_DIR}/*.c")

add_executable(server main.c ${C_FILES})
target_include_directories(server PUBLIC include/)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)