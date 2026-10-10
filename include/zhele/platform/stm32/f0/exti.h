/**
 * @file
 * Implements EXTI for stm32f0 series
 * 
 * @author Alexey Zhelonkin
 * @date 2020
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_F0_EXTI_H
#define ZHELE_PLATFORM_STM32_F0_EXTI_H

#include <stm32f0xx.h>

#include "../common/exti.h"

#include <meta>

namespace Zhele
{
    template<uint8_t _Line, IRQn_Type _IRQn>
    template <typename port>
    void Exti<_Line, _IRQn>::SelectPort()
    {
        SYSCFG->EXTICR[_Line / 4] = (SYSCFG->EXTICR[_Line / 4] & ~(0xFu << _Line % 4 * 4)) | ((port::Id - 'A') << (_Line % 4 * 4));
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    void Exti<_Line, _IRQn>::SelectPort(uint8_t portID)
    {
        SYSCFG->EXTICR[_Line / 4] = (SYSCFG->EXTICR[_Line / 4] & ~(0xFu << _Line % 4 * 4)) | ((portID - 'A') << (_Line % 4 * 4));
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    void Exti<_Line, _IRQn>::EnableClock()
    {
        Zhele::Clock::SysCfgCompClock::Enable();
    }

    // Exti0..Exti15; lines 0-1, 2-3 and 4-15 share an IRQ
    consteval
    {
        for (unsigned line = 0; line < 16; ++line)
        {
            const IRQn_Type irq = line < 2 ? EXTI0_1_IRQn : line < 4 ? EXTI2_3_IRQn : EXTI4_15_IRQn;
            std::meta::queue_injection(^^{ using \["Exti", line] = Exti<\val(line), \val(irq)>; });
        }
    }
}
#endif //! ZHELE_PLATFORM_STM32_F0_EXTI_H