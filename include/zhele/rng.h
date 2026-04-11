/**
 * @file
 * United header for RNG (hardware random generation)
 *
 * @author Alexey Zhelonkin
 * @license MIT
 */

#ifndef ZHELE_RNG_H
#define ZHELE_RNG_H

#include "platform_detector.h"

#if defined(ZHELE_PLATFORM_STM32)
  #include "platform/stm32/rng.h"
#elif defined(ZHELE_PLATFORM_CH32)
  #include "platform/ch32/rng.h"
#else
  #error "Zhele: unsupported platform. Define ZHELE_PLATFORM_XX or include CMSIS device headers."
#endif

#endif // ZHELE_RNG_H
