/**
 * @file
 * United header for USART
 * @author Alexey Zhelonkin
 * @date 2019
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_USART_H
#define ZHELE_PLATFORM_STM32_USART_H

#if defined(STM32F0)
    #include "f0/usart.h"
#elif defined(STM32F1)
    #include "f1/usart.h"
#elif defined(STM32F4)
    #include "f4/usart.h"
#elif defined(STM32L4)
    #include "l4/usart.h"
#elif defined(STM32G0)
    #include "g0/usart.h"
#elif defined(STM32G4)
    #include "g4/usart.h"
#elif defined(STM32H5)
    #include "h5/usart.h"
#elif defined(STM32C0)
    #include "c0/usart.h"
#endif

#endif // ZHELE_PLATFORM_STM32_USART_H
