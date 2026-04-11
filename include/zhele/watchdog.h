/**
 * @file
 * United header for Watchdog
 *
 * @author Alexey Zhelonkin
 * @license MIT
 */

#ifndef ZHELE_WATCHDOG_H
#define ZHELE_WATCHDOG_H

#include "platform_detector.h"

#if defined(ZHELE_PLATFORM_STM32)
  #include "platform/stm32/watchdog.h"
#elif defined(ZHELE_PLATFORM_CH32)
  #include "platform/ch32/watchdog.h"
#else
  #error "Zhele: unsupported platform. Define ZHELE_PLATFORM_XX or include CMSIS device headers."
#endif

#endif // ZHELE_WATCHDOG_H
