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
    namespace Private
    {
        using ExtiRegs = IoStruct<EXTI_BASE, EXTI_TypeDef>;
        using ExtiRisingReg = ExtiRegs::Reg<^^EXTI_TypeDef::RTSR>;
        using ExtiFallingReg = ExtiRegs::Reg<^^EXTI_TypeDef::FTSR>;
        using ExtiMaskReg = ExtiRegs::Reg<^^EXTI_TypeDef::IMR>;
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    template <typename Exti<_Line, _IRQn>::Trigger trigger, typename port>
    void Exti<_Line, _IRQn>::Init()
    {
        EnableClock();
        Private::ExtiRisingReg::WriteBit<_Line>(trigger & Rising);
        Private::ExtiFallingReg::WriteBit<_Line>(trigger & Falling);
        SelectPort<port>();
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    void Exti<_Line, _IRQn>::Init(Trigger trigger, char portId)
    {
        EnableClock();
        Private::ExtiRisingReg::WriteBit<_Line>(trigger & Rising);
        Private::ExtiFallingReg::WriteBit<_Line>(trigger & Falling);
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
        Private::ExtiMaskReg::SetBit<_Line>();
        NVIC_EnableIRQ(_IRQn);
    }

    template<uint8_t _Line, IRQn_Type _IRQn>
    void Exti<_Line, _IRQn>::DisableInterrupt()
    {
        Private::ExtiMaskReg::ClearBit<_Line>();
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