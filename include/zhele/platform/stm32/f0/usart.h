/**
 * @file
 * @brief Implements USART protocol for stm32f0 series
 * @author Alexey Zhelonkin
 * @date 2019
 * @license MIT
 */

#pragma once

#include "../common/usart.h"

#include "clock.h"
#include "dma.h"
#include "iopins.h"
#include "zhele/common/template_utils/array.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Zhele
{
    namespace Private
    {
        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::SelectTxRxPins(int8_t txPinNumber, int8_t rxPinNumber)
        {
            using TxPins = typename _TxPins::io_pins;
            using RxPins = typename _RxPins::io_pins;

            using Type = typename TxPins::DataType;

            TxPins::Enable();
            Type maskTx(1 << txPinNumber);
            TxPins::SetConfiguration(TxPins::Configuration::AltFunc, maskTx);
            TxPins::AltFuncNumber(_TxPins::alt_functions[static_cast<size_t>(txPinNumber)], maskTx);

            if(rxPinNumber != -1)
            {
                RxPins::Enable();
                Type maskRx(1 << rxPinNumber);
                RxPins::SetConfiguration(RxPins::Configuration::AltFunc, maskRx);
                RxPins::AltFuncNumber(_RxPins::alt_functions[static_cast<size_t>(rxPinNumber)], maskRx);
            }
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        template<int8_t TxPinNumber, int8_t RxPinNumber>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::SelectTxRxPins()
        {
            using TxPin = typename _TxPins::io_pins::template Pin<TxPinNumber>;

            TxPin::Port::Enable();

            TxPin::template SetConfiguration<TxPin::Port::Configuration::AltFunc>();
            TxPin::template AltFuncNumber<_TxPins::alt_functions[TxPinNumber]>();

            if constexpr(RxPinNumber != -1)
            {
                using RxPin = typename _RxPins::io_pins::template Pin<RxPinNumber>;

                if constexpr (!std::is_same_v<typename RxPin::Port, typename TxPin::Port>) {
                    RxPin::Port::Enable();
                }

                RxPin::template SetConfiguration<RxPin::Port::Configuration::AltFunc>();
                RxPin::template AltFuncNumber<_RxPins::alt_functions[RxPinNumber]>();
            }
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        template<typename TxPin, typename RxPin>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::SelectTxRxPins()
        {
            const int8_t txPinIndex = _TxPins::io_pins:: template IndexOf<TxPin>;
            const int8_t rxPinIndex = !std::is_same_v<RxPin, IO::NullPin>
                                ? _RxPins::io_pins:: template IndexOf<RxPin>
                                : -1;
            static_assert(txPinIndex >= 0);
            static_assert(rxPinIndex >= -1);
            SelectTxRxPins<txPinIndex, rxPinIndex>();
        }

        struct Usart1TxPins
        {
            using io_pins = IO::PinList<IO::Pa2, IO::Pa9, IO::Pb6>;
            static constexpr ArrayU8 alt_functions{1, 1, 0};
        };
        struct Usart1RxPins
        {
            using io_pins = IO::PinList<IO::Pa10, IO::Pa15, IO::Pb7>;
            static constexpr ArrayU8 alt_functions{1, 1, 0};
        };

        struct Usart2TxPins
        {
            using io_pins = IO::PinList<IO::Pa2, IO::Pd5>;
            static constexpr ArrayU8 alt_functions{1, 1};
        };
        struct Usart2RxPins
        {
            using io_pins = IO::PinList<IO::Pa3, IO::Pd6>;
            static constexpr ArrayU8 alt_functions{1, 1};
        };

        IO_STRUCT_WRAPPER(USART1, Usart1Regs, USART_TypeDef);
    #if defined(USART2)
        IO_STRUCT_WRAPPER(USART2, Usart2Regs, USART_TypeDef);
    #endif
    }
    using Usart1 = Private::Usart<Private::Usart1Regs, USART1_IRQn, Clock::Usart1Clock, Private::Usart1TxPins, Private::Usart1RxPins, Dma1Channel2, Dma1Channel3>;
#if defined (USART2)
    using Usart2 = Private::Usart<Private::Usart2Regs, USART2_IRQn, Clock::Usart2Clock, Private::Usart2TxPins, Private::Usart2RxPins, Dma1Channel4, Dma1Channel5>;
#endif
}


