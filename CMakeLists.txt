cmake_minimum_required(VERSION 3.5)

project(LiMS VERSION 0 LANGUAGES C)
set(CMAKE_BUILD_TYPE Debug)

add_executable(main main.c book.c library.c
                    fileops.c logger.c)
