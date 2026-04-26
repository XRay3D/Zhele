
/// @file
/// @brief Implements USART protocol for mic32 series
/// @author Alexey Zhelonkin / X-Ray
/// @date 2026
/// @license MIT

#pragma once

#include <epic.h>
#include <uart.h>

// For compatibility with "default" CMSIS (F0/F1/F4) map G0 constants for Fx constants
// #define USART_ISR_TXE    USART_ISR_TXE_TXFNF
// #define USART_ISR_RXNE   USART_ISR_RXNE_RXFNE
// #define USART_CR1_RXNEIE USART_CR1_RXNEIE_RXFNEIE
// #define USART_CR1_TXEIE  USART_CR1_TXEIE_TXFNFIE

#include "../common/usart.h"

// #include "clock.h"
#include "dma.h"
#include "iopins.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Zhele {
namespace Private {

template <typename... Params>
void Usart<Params...>::SelectTxRxPins(int8_t txPinNumber, int8_t rxPinNumber) {
    using _TxPins = typename TxPins::io_pins;
    using _RxPins = typename RxPins::io_pins;
    using Type    = typename _TxPins::DataType;
    _TxPins::Enable();
    Type maskTx(1 << txPinNumber);
    _TxPins::SetConfiguration(_TxPins::Configuration::AltFunc, maskTx);
    _TxPins::AltFuncNumber(TxPins::alt_functions[static_cast<size_t>(txPinNumber)], maskTx);

    if(rxPinNumber != -1) {
        _RxPins::Enable();
        Type maskRx(1 << rxPinNumber);
        _RxPins::SetConfiguration(_RxPins::Configuration::AltFunc, maskRx);
        _RxPins::AltFuncNumber(RxPins::alt_functions[static_cast<size_t>(rxPinNumber)], maskRx);
    }
}

template <typename... Params>
template <int8_t TxPinNumber, int8_t RxPinNumber>
void Usart<Params...>::SelectTxRxPins() {
    using TxPin = TxPins::io_pins::template Pin<TxPinNumber>;

    TxPin::Port::Enable();

    TxPin::template SetConfiguration<TxPin::Port::Configuration::AltFunc>();
    TxPin::template AltFuncNumber<TxPins::alt_functions[TxPinNumber]>();

    if constexpr(RxPinNumber != -1) {
        using RxPin = RxPins::io_pins::template Pin<RxPinNumber>;

        if constexpr(!std::is_same_v<typename RxPin::Port, typename TxPin::Port>) {
            RxPin::Port::Enable();
        }

        RxPin::template SetConfiguration<RxPin::Port::Configuration::AltFunc>();
        RxPin::template AltFuncNumber<RxPins::alt_functions[RxPinNumber]>();
    }
}

template <typename... Params>
template <typename TxPin, typename RxPin>
void Usart<Params...>::SelectTxRxPins() {
    const int8_t txPinIndex = TxPins::io_pins::template IndexOf<TxPin>;
    const int8_t rxPinIndex = !std::is_same_v<RxPin, IO::NullPin>
        ? RxPins::io_pins::template IndexOf<RxPin>
        : -1;
    static_assert(txPinIndex >= 0);
    static_assert(rxPinIndex >= -1);
    SelectTxRxPins<txPinIndex, rxPinIndex>();
}

/*
Назначение             Назначение| Тип   | Описание
UART0_CTS PADx_CFG = 1 GPIO0_07  | Вход  | Готовность передачи
UART0_DCD PADx_CFG = 2 GPIO1_13  | Вход  | Сигнал обнаружен ия несущей
UART0_DSR PADx_CFG = 2 GPIO1_14  | Вход  | Сигнал готовности источника данных
UART0_DTR PADx_CFG = 2 GPIO1_12  | Выход | Готовность данных приемника
UART0_RI  PADx_CFG = 2 GPIO1_15  | Вход  | Звонок (вызов) на телефонно й линии
UART0_RTS PADx_CFG = 1 GPIO0_08  | Выход | Запрос на передачу
UART0_RXD PADx_CFG = 1 GPIO0_05  | Вход  | Принимаемые данные
UART0_TXD PADx_CFG = 1 GPIO0_06  | Выход | Данные для передачи
UART1_CTS PADx_CFG = 1 GPIO1_10  | Вход  | Готовность передачи
UART1_RTS PADx_CFG = 1 GPIO1_11  | Выход | Запрос на передачу
UART1_RXD PADx_CFG = 1 GPIO1_08  | Вход  | Принимаемые данные
UART1_TXD PADx_CFG = 1 GPIO1_09  | Выход | Данные для передачи
*/

// | UART0_RXD PADx_CFG = 1 GPIO0_05  | Вход  | Принимаемые данные
// | UART0_TXD PADx_CFG = 1 GPIO0_06  | Выход | Данные для передачи
// | UART1_RXD PADx_CFG = 1 GPIO1_08  | Вход  | Принимаемые данные
// | UART1_TXD PADx_CFG = 1 GPIO1_09  | Выход | Данные для передачи

using Usart1TxPins = IO::AltPinList<{1}, IO::Pa6>;
using Usart1RxPins = IO::AltPinList<{1}, IO::Pa5>;

using Usart2TxPins = IO::AltPinList<{1}, IO::Pb9>;
using Usart2RxPins = IO::AltPinList<{1}, IO::Pb8>;

template <std::size_t ADDR>
using UsartRegs = Regs<UART_TypeDef, ADDR>;

using Usart1Regs = UsartRegs<UART_0_BASE_ADDRESS>;
using Usart2Regs = UsartRegs<UART_1_BASE_ADDRESS>;

template <typename DmaTx = void, typename DmaRx = void>
using Usart1 = Private::Usart<Private::Usart1Regs, Isr<EPIC_LINE_UART_0_S>,
    Clock::Usart1Clock, Private::Usart1TxPins, Private::Usart1RxPins, DmaTx, DmaRx>;

template <typename DmaTx = void, typename DmaRx = void>
using Usart2 = Private::Usart<Private::Usart2Regs, Isr<EPIC_LINE_UART_1_S>,
    Clock::Usart2Clock, Private::Usart2TxPins, Private::Usart2RxPins, DmaTx, DmaRx>;

} // namespace Private

using Usart1 = Private::Usart1<>;
using Usart2 = Private::Usart2<>;

} // namespace Zhele
