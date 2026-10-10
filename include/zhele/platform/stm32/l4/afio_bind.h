/**
 * @file
 * Implements utils for STM32L4 afio
 * GPIO alternate functions have hard bind
 * So, there is easy to move afio resolve in compile-time
 * 
 * @author Alexey Zhelonkin (based on Konstantin Chizhov)
 * @date 2022
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_L4_AFIO_BIND_H
#define ZHELE_PLATFORM_STM32_L4_AFIO_BIND_H

#include <cstdint>

namespace Zhele::Private
{
    /**
     * @brief Alternate function number of a peripheral, by its register block address.
     * The binding is fixed in hardware, so it is resolved at compile time.
     */
    constexpr uint8_t AltFunctionOf(uintptr_t base)
    {
        switch (base)
        {
            case USART1_BASE: case USART2_BASE: return 7;
        #if defined (USART3_BASE)
            case USART3_BASE: return 7;
        #endif
        #if defined (UART4_BASE)
            case UART4_BASE: return 8;
        #endif
        #if defined (UART5_BASE)
            case UART5_BASE: return 8;
        #endif
        #if defined (USART6_BASE)
            case USART6_BASE: return 8;
        #endif
            case SPI1_BASE: case SPI2_BASE: return 5;
        #if defined (SPI3_BASE)
            case SPI3_BASE: return 6;
        #endif
            case I2C1_BASE: case I2C2_BASE: return 4;
        #if defined (I2C3_BASE)
            case I2C3_BASE: return 4;
        #endif
        #if defined (USB_OTG_FS_PERIPH_BASE)
            case USB_OTG_FS_PERIPH_BASE: return 10;
        #endif
        }
        return 0;
    }

    template<typename Regs>
    inline constexpr uint8_t GetAltFunctionNumber = AltFunctionOf(Regs::Address);
} // namespace Private

#endif // !ZHELE_PLATFORM_STM32_L4_AFIO_BIND_H
