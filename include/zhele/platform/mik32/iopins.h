/**
 * @file
 * GPIO pin aliases for NIIET — dispatches to the correct device implementation.
 */
#pragma once

// Run the device detector even if ZHELE_PLATFORM_NIIET was set externally.
#include "platform_detector.h"

#if defined(ZHELE_NIIET_DEVICE_K1948VK015)
  #include "k1948vk015/iopins.h"
#else
  #error "Zhele: NIIET device not detected. Define K1948VK015 (compiler flag) or include <K1948VK015.h> before Zhele headers."
#endif


