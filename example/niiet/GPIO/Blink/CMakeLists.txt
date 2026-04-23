cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../niiet_cmake/cmake/niiet_gcc.cmake)

project(blink CXX C ASM)

niiet_fetch_cmsis(K1921)
find_package(CMSIS COMPONENTS K1921VG015 REQUIRED)

add_executable(blink src/main.cpp)
target_compile_features(blink PRIVATE cxx_std_20)
target_include_directories(blink PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../include)
target_link_libraries(blink PRIVATE
    CMSIS::K1921VG015
    NIIET::Nano
    NIIET::NoSys)

niiet_k1921vg015_target_link_flash(blink)
niiet_print_size_of_target(blink)
niiet_generate_hex_file(blink)
