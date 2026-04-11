/**
 * @file
 * United header for USB
 *
 * @author Alexey Zhelonkin
 * @license MIT
 */

#ifndef ZHELE_USB_H
#define ZHELE_USB_H

#include "platform_detector.h"

#if defined(ZHELE_PLATFORM_STM32)
  #include "platform/stm32/usb.h"
#elif defined(ZHELE_PLATFORM_CH32)
  #include "platform/ch32/usb.h"
#else
  #error "Zhele: unsupported platform. Define ZHELE_PLATFORM_XX or include CMSIS device headers."
#endif

#endif // ZHELE_USB_H
