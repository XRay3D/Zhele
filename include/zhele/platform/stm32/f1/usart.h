/**
 * @file
 * @brief Implements USART protocol for stm32f1 series
 * @author Alexey Zhelonkin
 * @date 2019
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_F1_USART_H
#define ZHELE_PLATFORM_STM32_F1_USART_H

#include "../common/usart.h"

#include "clock.h"
#include "dma.h"
#include "iopins.h"
#include "remap.h"

#include <array>
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

            if(rxPinNumber != -1)
            {
                RxPins::Enable();
                Type maskRx(1 << rxPinNumber);
                RxPins::SetConfiguration(RxPins::Configuration::In, maskRx);
            }

            Clock::AfioClock::Enable();
            Zhele::IO::Private::PeriphRemap<_ClockCtrl>::Set(_TxPins::alt_functions[static_cast<size_t>(txPinNumber)]);
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        template<int8_t TxPinNumber, int8_t RxPinNumber>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::SelectTxRxPins()
        {
            static_assert(TxPinNumber == RxPinNumber || RxPinNumber == -1);

            using TxPin = typename _TxPins::io_pins:: template Pin<TxPinNumber>;
            TxPin::Port::Enable();
            TxPin::SetConfiguration(TxPin::Port::Configuration::AltFunc);
            
            if constexpr(RxPinNumber != -1)
            {
                using RxPin = typename _RxPins::io_pins::template Pin<RxPinNumber>;

                if constexpr (!std::is_same_v<typename RxPin::Port, typename TxPin::Port>)
                {
                    RxPin::Port::Enable();
                }
                RxPin::SetConfiguration(RxPin::Port::Configuration::In);
            }

            Clock::AfioClock::Enable();
            Zhele::IO::Private::PeriphRemap<_ClockCtrl>::Set(_TxPins::alt_functions[TxPinNumber]);
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
            using io_pins = IO::PinList<IO::Pa9, IO::Pb6>;
            static constexpr std::array<uint8_t, 2> alt_functions{0, 1};
        };
        struct Usart1RxPins
        {
            using io_pins = IO::PinList<IO::Pa10, IO::Pb7>;
            static constexpr std::array<uint8_t, 2> alt_functions{0, 1};
        };

        struct Usart2TxPins
        {
            using io_pins = IO::PinList<IO::Pa2, IO::Pd5>;
            static constexpr std::array<uint8_t, 2> alt_functions{0, 1};
        };
        struct Usart2RxPins
        {
            using io_pins = IO::PinList<IO::Pa3, IO::Pd6>;
            static constexpr std::array<uint8_t, 2> alt_functions{0, 1};
        };

        struct Usart3TxPins
        {
            using io_pins = IO::PinList<IO::Pb10, IO::Pc10, IO::Pd8>;
            static constexpr std::array<uint8_t, 3> alt_functions{0, 1, 3};
        };
        struct Usart3RxPins
        {
            using io_pins = IO::PinList<IO::Pb11, IO::Pc11, IO::Pd9>;
            static constexpr std::array<uint8_t, 3> alt_functions{0, 1, 3};
        };

        IO_STRUCT_WRAPPER(USART1, Usart1Regs, USART_TypeDef);
        IO_STRUCT_WRAPPER(USART2, Usart2Regs, USART_TypeDef);
    #if defined(USART3)
        IO_STRUCT_WRAPPER(USART3, Usart3Regs, USART_TypeDef);
    #endif
    }
    using Usart1 = Private::Usart<Private::Usart1Regs, USART1_IRQn, Clock::Usart1Clock, Private::Usart1TxPins, Private::Usart1RxPins, Dma1Channel4, Dma1Channel5>;
    using Usart2 = Private::Usart<Private::Usart2Regs, USART2_IRQn, Clock::Usart2Clock, Private::Usart2TxPins, Private::Usart2RxPins, Dma1Channel7, Dma1Channel6>;
    #if defined(USART3)
        using Usart3 = Private::Usart<Private::Usart3Regs, USART3_IRQn, Clock::Usart3Clock, Private::Usart3TxPins, Private::Usart3RxPins, Dma1Channel2, Dma1Channel3>;
    #endif
}

#endif //! ZHELE_PLATFORM_STM32_F1_USART_H
