cmake_minimum_required(VERSION 3.10)
project(xsetroot-daemon)

add_executable(xsetrootd init.c)

target_link_libraries(xsetrootd -lX11)