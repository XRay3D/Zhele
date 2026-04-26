/**
 * @file
 * K1948VK018: instantiate GPIO port types (Porta, Portb, Portc).
 */
#pragma once

#include <gpio.h>
#include <mik32_memory_map.h>
#include <pad_config.h>

#include "../common/clock_gpio_ahb.h"
#include "../common/ioports_gpio.h"
#include "../common/ioreg.h"

namespace Zhele::IO {

#define MAKE_PORT(REGS, REGS2, OFFSET, CLK_EN_REG, CLASS_NAME, ID)              \
    namespace Private {                                                       \
    IO_STRUCT_WRAPPER(REGS, CLASS_NAME##REGS, GPIO_TypeDef);                  \
    IO_STRUCT_WRAPPER(REGS2 + OFFSET, CLASS_NAME##REGS2, PAD_CONFIG_TypeDef); \
    }                                                                         \
    using CLASS_NAME = Private::PortImplementation<Private::CLASS_NAME##REGS, \
        Private::CLASS_NAME##REGS2, CLK_EN_REG, ID>;

#if defined(GPIO_0)
MAKE_PORT(GPIO_0, PAD_CONFIG, 0, Zhele::Clock::Port0Clock, Porta, 'A')
#endif

#if defined(GPIO_1)
MAKE_PORT(GPIO_1, PAD_CONFIG, 12, Zhele::Clock::Port1Clock, Portb, 'B')
#endif

#if defined(GPIO_2)
MAKE_PORT(GPIO_2, PAD_CONFIG, 24, Zhele::Clock::Port2Clock, Portc, 'C')
#endif

#undef MAKE_PORT

} // namespace Zhele::IO
