/**
 * @file
 * MIK32: detect platform and device from CMSIS / project macros.
 *
 * Defines:
 *   ZHELE_PLATFORM_MIK32          — any MIK32 target
 *   ZHELE_MIK32_DEVICE_K1948VK018 — K1948VK018 (RISC-V, 3×16-pin GPIO ports)
 *
 * Detection order:
 *   1. K1948VK018 compile-time define (e.g. -DK1948VK018 from CMake).
 *   2. __K1948VK018_H header guard (set after #include <K1948VK018.h>).
 *   3. Explicit ZHELE_PLATFORM_MIK32 — allowed for testing / mocking.
 */
#pragma once

#if defined(K1948VK018) || defined(__K1948VK018_H)
  #define ZHELE_PLATFORM_MIK32          1
  #define ZHELE_MIK32_DEVICE_K1948VK018 1
#elif defined(ZHELE_PLATFORM_MIK32)
  // Device / family must also be set explicitly when overriding.
#endif


