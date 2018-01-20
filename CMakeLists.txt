cmake_minimum_required(VERSION 3.9)
project(ProjektSO C)

set(CMAKE_C_STANDARD 11)

add_executable(ProjektSO init.c proc1.c proc2.c proc3.c proc1.h proc2.h proc3.h utils.c utils.h)