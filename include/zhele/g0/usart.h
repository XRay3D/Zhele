/**
 * @file
 * @brief Implements USART protocol for stm32g0 series
 * @author Alexey Zhelonkin
 * @date 2024
 * @license FreeBSD
 */

#ifndef ZHELE_USART_H
#define ZHELE_USART_H

#include <stm32g0xx.h>

// For compatibility with "default" CMSIS (F0/F1/F4) map G0 constants for Fx constants
#define USART_ISR_TXE USART_ISR_TXE_TXFNF
#define USART_ISR_RXNE USART_ISR_RXNE_RXFNE
#define USART_CR1_RXNEIE USART_CR1_RXNEIE_RXFNEIE
#define USART_CR1_TXEIE USART_CR1_TXEIE_TXFNFIE

#include "../common/usart.h"

#include "clock.h"
#include "dma.h"
#include "iopins.h"

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
            TxPins::SetConfiguration(TxPins::AltFunc, maskTx);
            TxPins::AltFuncNumber(_TxPins::alt_functions[static_cast<size_t>(txPinNumber)], maskTx);

            if(rxPinNumber != -1)
            {
                RxPins::Enable();
                Type maskRx(1 << rxPinNumber);
                RxPins::SetConfiguration(RxPins::AltFunc, maskRx);
                RxPins::AltFuncNumber(_RxPins::alt_functions[static_cast<size_t>(rxPinNumber)], maskRx);
            }
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        template<int8_t TxPinNumber, int8_t RxPinNumber>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::SelectTxRxPins()
        {
            using TxPin = typename _TxPins::io_pins::template Pin<TxPinNumber>;

            TxPin::Port::Enable();

            TxPin::template SetConfiguration<TxPin::Port::AltFunc>();
            TxPin::template AltFuncNumber<_TxPins::alt_functions[TxPinNumber]>();

            if constexpr(RxPinNumber != -1)
            {
                using RxPin = typename _RxPins::io_pins::template Pin<RxPinNumber>;

                if constexpr (!std::is_same_v<typename RxPin::Port, typename TxPin::Port>) {
                    RxPin::Port::Enable();
                }

                RxPin::template SetConfiguration<RxPin::Port::AltFunc>();
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
            using io_pins = IO::PinList<IO::Pa9, IO::Pb6, IO::Pc4>;
            static constexpr std::array<uint8_t, 3> alt_functions{1, 0, 1};
        };
        struct Usart1RxPins
        {
            using io_pins = IO::PinList<IO::Pa10, IO::Pb7, IO::Pc5>;
            static constexpr std::array<uint8_t, 3> alt_functions{1, 0, 1};
        };

        struct Usart2TxPins
        {
            using io_pins = IO::PinList<IO::Pa2, IO::Pa14, IO::Pd5>;
            static constexpr std::array<uint8_t, 3> alt_functions{1, 1, 0};
        };
        struct Usart2RxPins
        {
            using io_pins = IO::PinList<IO::Pa3, IO::Pa15, IO::Pd6>;
            static constexpr std::array<uint8_t, 3> alt_functions{1, 1, 0};
        };

        struct Usart3TxPins
        {
            using io_pins = IO::PinList<IO::Pa5, IO::Pb2, IO::Pb8, IO::Pb10, IO::Pc4, IO::Pc10>;
            static constexpr std::array<uint8_t, 6> alt_functions{4, 4, 4, 4, 0, 0};
        };
        struct Usart3RxPins
        {
            using io_pins = IO::PinList<IO::Pb0, IO::Pb9, IO::Pb11, IO::Pc5, IO::Pc11>;
            static constexpr std::array<uint8_t, 5> alt_functions{4, 4, 4, 0, 0};
        };

        IO_STRUCT_WRAPPER(USART1, Usart1Regs, USART_TypeDef);
        IO_STRUCT_WRAPPER(USART2, Usart2Regs, USART_TypeDef);
    #if defined(USART3)
        IO_STRUCT_WRAPPER(USART3, Usart3Regs, USART_TypeDef);
    #endif
    }

    template<typename _DmaTx = void, typename _DmaRx = void>
    using Usart1 = Private::Usart<Private::Usart1Regs, USART1_IRQn, Clock::Usart1Clock, Private::Usart1TxPins, Private::Usart1RxPins, _DmaTx, _DmaRx>;
    
    template<typename _DmaTx = void, typename _DmaRx = void>
    using Usart2 = Private::Usart<Private::Usart2Regs, USART2_IRQn, Clock::Usart2Clock, Private::Usart2TxPins, Private::Usart2RxPins, _DmaTx, _DmaRx>;

#if defined (USART3)
    template<typename _DmaTx = void, typename _DmaRx = void>
    using Usart3 = Private::Usart<Private::Usart3Regs, USART3_IRQn, Clock::Usart3Clock, Private::Usart3TxPins, Private::Usart3RxPins, _DmaTx, _DmaRx>;
#endif
}

#endif //! ZHELE_USART_H
