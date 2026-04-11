cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../stm32-cmake/cmake/stm32_gcc.cmake)

project(zhele_examples CXX C ASM)

add_subdirectory(stm32)
