# cmake version setting
cmake_minimum_required(VERSION 3.5)

# project settings
project(LiMS VERSION 0 LANGUAGES C)
set(CMAKE_BUILD_TYPE Debug)

# find gtk4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# compiler settings
set(CMAKE_BUILD_TYPE Debug)
include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

# create main executable
add_executable(main main.c book.c library.c
                    fileops.c logger.c)
target_link_libraries(main ${GTK4_LIBRARIES})
