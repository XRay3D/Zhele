/**
 * @file
 * United header for EXTI
 *
 * @author Alexey Zhelonkin
 * @license MIT
 */

#ifndef ZHELE_EXTI_H
#define ZHELE_EXTI_H

#include "platform_detector.h"

#if defined(ZHELE_PLATFORM_STM32)
  #include "platform/stm32/exti.h"
#elif defined(ZHELE_PLATFORM_CH32)
  #include "platform/ch32/exti.h"
#else
  #error "Zhele: unsupported platform. Define ZHELE_PLATFORM_XX or include CMSIS device headers."
#endif

#endif // ZHELE_EXTI_H
