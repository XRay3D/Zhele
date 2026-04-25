/**
 * @file
 * K1948VK018: instantiate GPIO port types (Porta, Portb, Portc).
 */
#pragma once

#include <gpio.h>
#include <mik32_memory_map.h>

#include "../common/clock_gpio_ahb.h"
#include "../common/ioports_gpio.h"
#include "../common/ioreg.h"

namespace Zhele::IO {

#define MAKE_PORT(REGS, ClkEnReg, className, ID)            \
    namespace Private {                                     \
    IO_STRUCT_WRAPPER(REGS, className##REGS, GPIO_TypeDef); \
    }                                                       \
    using className = Private::PortImplementation<Private::className##REGS, ClkEnReg, ID>;

#if defined(GPIO_0)
MAKE_PORT(GPIO_0, Zhele::Clock::Port0Clock, Porta, 'A')
#endif

#if defined(GPIO_1)
MAKE_PORT(GPIO_1, Zhele::Clock::Port1Clock, Portb, 'B')
#endif

#if defined(GPIO_2)
MAKE_PORT(GPIO_2, Zhele::Clock::Port2Clock, Portc, 'C')
#endif

#undef MAKE_PORT

} // namespace Zhele::IO
