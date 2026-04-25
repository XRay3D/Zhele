/**
 * @file
 * GPIO ports for MIK32 — dispatches to the correct device implementation.
 */
#pragma once

// Run the device detector even if ZHELE_PLATFORM_MIK32 was set externally.
#include "platform_detector.h"

#if defined(ZHELE_MIK32_DEVICE_K1948VK018)
  #include "k1948vk018/ioports.h"
#else
  #error "Zhele: MIK32 device not detected. Define K1948VK018 (compiler flag) or include <K1948VK018.h> before Zhele headers."
#endif


