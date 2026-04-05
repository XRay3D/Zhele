/**
 * @file
 * Implement i2c protocol for stm32f0 series
 * 
 * @author Alexey Zhelonkin
 * @date 2020
 * @license FreeBSD
 */

#ifndef ZHELE_I2C_H
#define ZHELE_I2C_H

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
        I2C_TEMPLATE_ARGS
        template<unsigned sclPinNumber, unsigned sdaPinNumber>
        void I2C_TEMPLATE_QUALIFIER::SelectPins()
        {
            using SclPin = typename _SclPins::io_pins::template Pin<sclPinNumber>;
            SclPin::Port::Enable();
            SclPin::template SetConfiguration<SclPin::Port::AltFunc>();
            SclPin::template AltFuncNumber<_SclPins::alt_functions[sclPinNumber]>();
            SclPin::SetDriverType(SclPin::Port::OpenDrain);
            SclPin::SetPullMode(SclPin::PullMode::PullUp);

            using SdaPin = typename _SdaPins::io_pins::template Pin<sdaPinNumber>;
            if constexpr (!std::is_same_v<typename SdaPin::Port, typename SclPin::Port>)
            {
                SdaPin::Port::Enable();
            }
            SdaPin::template SetConfiguration<SdaPin::Port::AltFunc>();
            SdaPin::template AltFuncNumber<_SdaPins::alt_functions[sdaPinNumber]>();
            SdaPin::SetDriverType(SdaPin::Port::OpenDrain);
            SdaPin::SetPullMode(SdaPin::PullMode::PullUp);
        }

        I2C_TEMPLATE_ARGS
        void I2C_TEMPLATE_QUALIFIER::SelectPins(uint8_t sclPinNumber, uint8_t sdaPinNumber)
        {
            using SclPins = typename _SclPins::io_pins;
            using SdaPins = typename _SdaPins::io_pins;

            using Type = typename SclPins::DataType;

            SclPins::Enable();
            Type maskScl(1 << sclPinNumber);
            SclPins::SetConfiguration(SclPins::AltFunc, maskScl);
            SclPins::AltFuncNumber(_SclPins::alt_functions[sclPinNumber], maskScl);
            SclPins::SetDriverType(SclPins::OpenDrain, maskScl);
            SclPins::SetPullMode(SclPins::PullMode::PullUp, maskScl);

            SdaPins::Enable();
            Type maskSda(1 << sdaPinNumber);
            SdaPins::SetConfiguration(SdaPins::AltFunc, maskSda);
            SdaPins::AltFuncNumber(_SdaPins::alt_functions[sdaPinNumber], maskSda);
            SdaPins::SetDriverType(SdaPins::OpenDrain, maskSda);
            SdaPins::SetPullMode(SdaPins::PullMode::PullUp, maskSda);
        }
        
        I2C_TEMPLATE_ARGS
        template<typename SclPin, typename SdaPin>
        void I2C_TEMPLATE_QUALIFIER::SelectPins()
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

        IO_STRUCT_WRAPPER(I2C1, I2C1Regs, I2C_TypeDef);
    }
    using I2c1 = Private::I2cBase<Private::I2C1Regs, I2C1_IRQn, I2C1_IRQn, Clock::I2c1Clock, Private::I2C1SclPins, Private::I2C1SdaPins, Dma1Channel2, Dma1Channel3>;
}

#endif //! ZHELE_I2C_H
