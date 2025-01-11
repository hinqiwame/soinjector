cmake_minimum_required(VERSION 3.26)
project(soinjector C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(inject injector.c)
add_executable(inject-gui injector-gui.c)

target_link_libraries(inject-gui ${GTK3_LIBRARIES})
