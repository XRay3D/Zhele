/**
 * @file
 * United header for DAC
 *
 * @author Alexey Zhelonkin
 * @date 2022
 * @licence FreeBSD
 */

#if defined(STM32F0)
    #include "f0/dac.h"
#elif defined(STM32F1)
    #include "f1/dac.h"
#elif defined(STM32F4)
    #include "f4/dac.h"
#elif defined(STM32L4)
    #include "l4/dac.h"
#elif defined(STM32G0)
    #include "g0/dac.h"
#elif defined(STM32G4)
    #include "g4/dac.h"
#endif