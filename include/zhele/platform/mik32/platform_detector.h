/**
 * @file
 * NIIET: detect platform and device from CMSIS / project macros.
 *
 * Defines:
 *   ZHELE_PLATFORM_NIIET          — any NIIET target
 *   ZHELE_NIIET_DEVICE_K1948VK015 — K1948VK015 (RISC-V, 3×16-pin GPIO ports)
 *
 * Detection order:
 *   1. K1948VK015 compile-time define (e.g. -DK1948VK015 from CMake).
 *   2. __K1948VK015_H header guard (set after #include <K1948VK015.h>).
 *   3. Explicit ZHELE_PLATFORM_NIIET — allowed for testing / mocking.
 */
#pragma once

#if defined(K1948VK015) || defined(__K1948VK015_H)
  #define ZHELE_PLATFORM_NIIET          1
  #define ZHELE_NIIET_DEVICE_K1948VK015 1
#elif defined(ZHELE_PLATFORM_NIIET)
  // Device / family must also be set explicitly when overriding.
#endif


