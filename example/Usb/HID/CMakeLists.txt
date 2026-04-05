cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../../stm32-cmake/cmake/stm32_gcc.cmake)
set(CMAKE_CXX_STANDARD 23)

project(usb_hid CXX C ASM)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../include)

set(_usb_compile_opts -fno-exceptions $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> -ffunction-sections -fdata-sections)

add_executable(usb_hid_led_f0 UsbLedControlExample.cpp)
target_link_libraries(usb_hid_led_f0 CMSIS::STM32::F072RB STM32::NoSys STM32::Nano)
target_compile_options(usb_hid_led_f0 PRIVATE ${_usb_compile_opts})
stm32_print_size_of_target(usb_hid_led_f0)

add_executable(usb_hid_led_f1 UsbLedControlExample_f1.cpp)
target_link_libraries(usb_hid_led_f1 CMSIS::STM32::F103C8 STM32::NoSys STM32::Nano)
target_compile_options(usb_hid_led_f1 PRIVATE ${_usb_compile_opts})
stm32_print_size_of_target(usb_hid_led_f1)

add_executable(usb_hid_led_strings_f1 UsbLedControlExampleWithStrings_f1.cpp)
target_link_libraries(usb_hid_led_strings_f1 CMSIS::STM32::F103C8 STM32::NoSys STM32::Nano)
target_compile_options(usb_hid_led_strings_f1 PRIVATE ${_usb_compile_opts})
stm32_print_size_of_target(usb_hid_led_strings_f1)

add_executable(usb_hid_led_f4 UsbExample_f4_Led.cpp)
target_link_libraries(usb_hid_led_f4 CMSIS::STM32::F401CC STM32::NoSys STM32::Nano)
target_compile_definitions(usb_hid_led_f4 PRIVATE F_CPU=84000000)
target_compile_options(usb_hid_led_f4 PRIVATE ${_usb_compile_opts})
stm32_print_size_of_target(usb_hid_led_f4)
