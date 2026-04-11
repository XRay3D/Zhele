/**
 * @file
 * United header for ADC
 *
 * @author Alexey Zhelonkin
 * @license MIT
 */

#ifndef ZHELE_ADC_H
#define ZHELE_ADC_H

#include "platform_detector.h"

#if defined(ZHELE_PLATFORM_STM32)
  #include "platform/stm32/adc.h"
#elif defined(ZHELE_PLATFORM_CH32)
  #include "platform/ch32/adc.h"
#else
  #error "Zhele: unsupported platform. Define ZHELE_PLATFORM_XX or include CMSIS device headers."
#endif

#endif // ZHELE_ADC_H
