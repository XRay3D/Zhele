/**
 * @file
 * EXTI methods implementation
 * 
 * @author Alexey Zhelonkin
 * @date 2020
 * @license MIT
 */
#ifndef ZHELE_PLATFORM_STM32_COMMON_IMPL_EXTI_H
#define ZHELE_PLATFORM_STM32_COMMON_IMPL_EXTI_H

#include <zhele/clock.h>
#include <zhele/ioports.h>
#include "../ioreg.h"

namespace Zhele
{
    namespace Regs::Exti
    {
        consteval { Private::InjectRegisters(EXTI_BASE, ^^EXTI_TypeDef); }
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    template <typename Exti<_Line, _IRQn>::Trigger trigger, typename port>
    void Exti<_Line, _IRQn>::Init()
    {
        EnableClock();
        Regs::Exti::RTSR::WriteBit<_Line>(trigger & Rising);
        Regs::Exti::FTSR::WriteBit<_Line>(trigger & Falling);
        SelectPort<port>();
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    void Exti<_Line, _IRQn>::Init(Trigger trigger, char portId)
    {
        EnableClock();
        Regs::Exti::RTSR::WriteBit<_Line>(trigger & Rising);
        Regs::Exti::FTSR::WriteBit<_Line>(trigger & Falling);
        SelectPort(portId);
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    template <typename pin>
    void Exti<_Line, _IRQn>::InitPin(
        typename pin::PullMode pullMode,
        typename pin::DriverType driverType,
        typename pin::Speed speed)
    {
        pin::Port::Enable();
        pin::SetConfiguration(pin::Configuration::In);
        pin::SetPullMode(pullMode);
#if !defined(STM32F1)
        pin::SetDriverType(driverType);
        pin::SetSpeed(speed);
#endif
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    void Exti<_Line, _IRQn>::EnableInterrupt()
    {
        Regs::Exti::IMR::SetBit<_Line>();
        NVIC_EnableIRQ(_IRQn);
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    void Exti<_Line, _IRQn>::DisableInterrupt()
    {
        Regs::Exti::IMR::ClearBit<_Line>();
        NVIC_DisableIRQ(_IRQn);
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    void Exti<_Line, _IRQn>::ClearInterruptFlag()
    {
#if defined (EXTI_RPR1_RPIF0)
        EXTI->RPR1 = (1 << _Line);
        EXTI->FPR1 = (1 << _Line);
#else
        EXTI->PR = (1u << _Line);
#endif
    }
}
#endif //! ZHELE_PLATFORM_STM32_COMMON_IMPL_EXTI_H