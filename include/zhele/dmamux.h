/**
 * @file
 * United header for DMA
 * 
 * @author Alexey Zhelonkin
 * @date 2019
 * @license FreeBSD
 */
#if defined(STM32F0)
    #error "No dmamux available for stm32f0"
#elif defined(STM32F1)
    #error "No dmamux available for stm32f1"
#elif defined(STM32F4)
    #error "No dmamux available for stm32f4"
#elif defined(STM32L4)
    #include "l4/dmamux.h"
#elif defined(STM32G0)
    #include "g0/dmamux.h"
#elif defined(STM32G4)
    #include "g4/dmamux.h"
#endif