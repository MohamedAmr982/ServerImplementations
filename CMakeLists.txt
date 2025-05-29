cmake_minimum_required(VERSION 3.16)
project(SERVER)
add_executable(server server.c src/request_data.c src/utility.c)
target_include_directories(server PUBLIC include/)