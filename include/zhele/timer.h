/**
 * @file
 * United header for timers
 *
 * @author Alexey Zhelonkin
 * @license MIT
 */

#ifndef ZHELE_TIMER_H
#define ZHELE_TIMER_H

#include "platform_detector.h"

#if defined(ZHELE_PLATFORM_STM32)
  #include "platform/stm32/timer.h"
#elif defined(ZHELE_PLATFORM_CH32)
  #include "platform/ch32/timer.h"
#else
  #error "Zhele: unsupported platform. Define ZHELE_PLATFORM_XX or include CMSIS device headers."
#endif

#endif // ZHELE_TIMER_H
