cmake_minimum_required(VERSION 3.16)
project(riscv_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(riscv_sim riscv_sim.cpp riscv_sim.h elf_loader.cpp elf_loader.h)
