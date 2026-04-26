/**
 * @file
 * United header for USART
 * @author Alexey Zhelonkin / X-Ray
 * @date 2026
 * @license MIT
 */

#pragma once

#if defined(ZHELE_MIK32_DEVICE_K1948VK018)
  #include "k1948vk018/usart.h"
#else
  #error "Zhele: MIK32 device not detected. Define K1948VK018 (compiler flag)"
#endif


