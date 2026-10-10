cmake_minimum_required(VERSION 3.16)

set (CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../stm32-cmake/cmake/stm32_gcc.cmake)
set (CMAKE_CXX_STANDARD 26)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-freflection>)

project(nucleo_f031k6 CXX C ASM)

# Populate CMSIS using stm32-cmake project (Commented for use in github actions, uncomment if you want to build example alone)
#stm32_fetch_cmsis(F0)
#find_package(CMSIS COMPONENTS STM32F0 REQUIRED)

# Add zhele as include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../include)

# NUCLEO-F031K6: LD3 on PB3, ST-LINK VCP on PA2 (TX) / PA15 (RX), no crystal (HSI 8 MHz)

# Self-checking hardware test: prints PASS/FAIL per peripheral to the VCP, then echoes input
add_executable(hwtest_f031 hwtest.cpp)
target_link_libraries(hwtest_f031 CMSIS::STM32::F031K6 STM32::NoSys STM32::Nano)
target_compile_definitions(hwtest_f031 PRIVATE HSI_VALUE=8000000 HSE_VALUE=8000000)
target_compile_options(hwtest_f031 PRIVATE -fno-exceptions $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> -ffunction-sections -fdata-sections)
stm32_print_size_of_target(hwtest_f031)

# LD3 blink from TIM3 update interrupt
add_executable(led_blinking_f031 blink.cpp)
target_link_libraries(led_blinking_f031 CMSIS::STM32::F031K6 STM32::NoSys STM32::Nano)
target_compile_definitions(led_blinking_f031 PRIVATE HSI_VALUE=8000000 HSE_VALUE=8000000)
target_compile_options(led_blinking_f031 PRIVATE -fno-exceptions $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> -ffunction-sections -fdata-sections)
stm32_print_size_of_target(led_blinking_f031)

# Cycle benchmark of library primitives, timed with TIM2 at HCLK
add_executable(bench_f031 bench.cpp)
target_link_libraries(bench_f031 CMSIS::STM32::F031K6 STM32::NoSys STM32::Nano)
target_compile_definitions(bench_f031 PRIVATE HSI_VALUE=8000000 HSE_VALUE=8000000)
target_compile_options(bench_f031 PRIVATE -fno-exceptions $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> -ffunction-sections -fdata-sections)
stm32_print_size_of_target(bench_f031)
