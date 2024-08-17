cmake_minimum_required(VERSION 3.10)
project(xsetroot-daemon)

add_executable(xsetrootd init.c)

target_link_libraries(xsetrootd -lX11)

add_executable(test_notify notify.c)
include_directories(test_notify PRIVATE /usr/include/glib-2.0/ /usr/lib/glib-2.0/include/ /usr/include/gdk-pixbuf-2.0/)
target_link_libraries(test_notify -lnotify)
