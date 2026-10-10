/**
 * @file
 * Implement i2c protocol for stm32f0 series
 * 
 * @author Alexey Zhelonkin
 * @date 2020
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_F0_I2C_H
#define ZHELE_PLATFORM_STM32_F0_I2C_H

#include "../common/i2c.h"

#include "dma.h"
#include "iopins.h"

#include <array>
#include <cstdint>
#include <type_traits>

namespace Zhele
{
    namespace Private
    {
        template<typename _Regs, IRQn_Type _EventIrqNumber, IRQn_Type _ErrorIrqNumber, typename _ClockCtrl, typename _SclPins, typename _SdaPins, typename _DmaTx, typename _DmaRx>
        template<unsigned sclPinNumber, unsigned sdaPinNumber>
        void I2cBase<_Regs, _EventIrqNumber, _ErrorIrqNumber, _ClockCtrl, _SclPins, _SdaPins, _DmaTx, _DmaRx>::SelectPins()
        {
            using SclPin = typename _SclPins::io_pins::template Pin<sclPinNumber>;
            SclPin::Port::Enable();
            SclPin::template SetConfiguration<SclPin::Port::Configuration::AltFunc>();
            SclPin::template AltFuncNumber<_SclPins::alt_functions[sclPinNumber]>();
            SclPin::SetDriverType(SclPin::Port::DriverType::OpenDrain);
            SclPin::SetPullMode(SclPin::PullMode::PullUp);

            using SdaPin = typename _SdaPins::io_pins::template Pin<sdaPinNumber>;
            if constexpr (!std::is_same_v<typename SdaPin::Port, typename SclPin::Port>)
            {
                SdaPin::Port::Enable();
            }
            SdaPin::template SetConfiguration<SdaPin::Port::Configuration::AltFunc>();
            SdaPin::template AltFuncNumber<_SdaPins::alt_functions[sdaPinNumber]>();
            SdaPin::SetDriverType(SdaPin::Port::DriverType::OpenDrain);
            SdaPin::SetPullMode(SdaPin::PullMode::PullUp);
        }

        template<typename _Regs, IRQn_Type _EventIrqNumber, IRQn_Type _ErrorIrqNumber, typename _ClockCtrl, typename _SclPins, typename _SdaPins, typename _DmaTx, typename _DmaRx>
        void I2cBase<_Regs, _EventIrqNumber, _ErrorIrqNumber, _ClockCtrl, _SclPins, _SdaPins, _DmaTx, _DmaRx>::SelectPins(uint8_t sclPinNumber, uint8_t sdaPinNumber)
        {
            using SclPins = typename _SclPins::io_pins;
            using SdaPins = typename _SdaPins::io_pins;

            using Type = typename SclPins::DataType;

            SclPins::Enable();
            Type maskScl(1 << sclPinNumber);
            SclPins::SetConfiguration(SclPins::Configuration::AltFunc, maskScl);
            SclPins::AltFuncNumber(_SclPins::alt_functions[sclPinNumber], maskScl);
            SclPins::SetDriverType(SclPins::DriverType::OpenDrain, maskScl);
            SclPins::SetPullMode(SclPins::PullMode::PullUp, maskScl);

            SdaPins::Enable();
            Type maskSda(1 << sdaPinNumber);
            SdaPins::SetConfiguration(SdaPins::Configuration::AltFunc, maskSda);
            SdaPins::AltFuncNumber(_SdaPins::alt_functions[sdaPinNumber], maskSda);
            SdaPins::SetDriverType(SdaPins::DriverType::OpenDrain, maskSda);
            SdaPins::SetPullMode(SdaPins::PullMode::PullUp, maskSda);
        }
        
        template<typename _Regs, IRQn_Type _EventIrqNumber, IRQn_Type _ErrorIrqNumber, typename _ClockCtrl, typename _SclPins, typename _SdaPins, typename _DmaTx, typename _DmaRx>
        template<typename SclPin, typename SdaPin>
        void I2cBase<_Regs, _EventIrqNumber, _ErrorIrqNumber, _ClockCtrl, _SclPins, _SdaPins, _DmaTx, _DmaRx>::SelectPins()
        {
            const int sclPinIndex = _SclPins::io_pins:: template IndexOf<SclPin>;
            const int sdaPinIndex = _SdaPins::io_pins:: template IndexOf<SdaPin>;
            
            static_assert(sclPinIndex >= 0);
            static_assert(sdaPinIndex >= 0);

            SelectPins<sclPinIndex, sdaPinIndex>();
        }

        struct I2C1SclPins
        {
            using io_pins = IO::PinList<IO::Pa9, IO::Pa11, IO::Pb6, IO::Pb8, IO::Pb10>;
            static constexpr std::array<uint8_t, 5> alt_functions{4, 5, 1, 1, 1};
        };
        struct I2C1SdaPins
        {
            using io_pins = IO::PinList<IO::Pa10, IO::Pa12, IO::Pb7, IO::Pb9, IO::Pb11>;
            static constexpr std::array<uint8_t, 5> alt_functions{4, 5, 1, 1, 1};
        };

        using I2C1Regs = IoStruct<I2C1_BASE, I2C_TypeDef>;
    }
    using I2c1 = Private::I2cBase<Private::I2C1Regs, I2C1_IRQn, I2C1_IRQn, Clock::I2c1Clock, Private::I2C1SclPins, Private::I2C1SdaPins, Dma1Channel2, Dma1Channel3>;
}

#endif //! ZHELE_PLATFORM_STM32_F0_I2C_H
