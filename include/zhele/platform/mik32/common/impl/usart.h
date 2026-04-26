
/// @file
///  UART methods implementation
/// @author Konstantin Chizhov / X-Ray
/// @date 2026
/// @license MIT

#pragma once

#include "uart.h"
#include "zhele/platform/mik32/common/usart.h"

namespace Zhele::Private {

template <typename... Rs>
template <uint32_t baud>
void Usart<Rs...>::Init(UsartMode mode) { Init(baud, mode); }

template <typename... Rs>
void Usart<Rs...>::Init(uint32_t baud, UsartMode mode) {
    ClockCtrl::Enable();
    SetBaud(baud);
    Regs()->FLAGS    = 0x00;
    Regs()->CONTROL1 = UART_CONTROL1_UE_M;
    Regs()->CONTROL3 = mode.CONTROL3;
    Regs()->CONTROL2 = mode.CONTROL2;
    Regs()->CONTROL1 |= mode.CONTROL1;
}

template <typename... Rs>
void Usart<Rs...>::SetConfig(UsartMode modeMask) {
    Regs()->CONTROL3 |= modeMask.CONTROL3;
    Regs()->CONTROL2 |= modeMask.CONTROL2;
    Regs()->CONTROL1 |= modeMask.CONTROL1;
}

template <typename... Rs>
void Usart<Rs...>::ClearConfig(UsartMode modeMask) {
    Regs()->CONTROL3 &= ~modeMask.CONTROL3;
    Regs()->CONTROL2 &= ~modeMask.CONTROL2;
    Regs()->CONTROL1 &= ~modeMask.CONTROL1;
}

template <typename... Rs>
void Usart<Rs...>::SetBaud(uint32_t baud) {
    Regs()->DIVIDER = /*ClockCtrl::ClockFreq()*/ 32000000u / baud;
}

template <typename... Rs>
bool Usart<Rs...>::ReadReady() {
    return Regs()->FLAGS & RxNotEmptyInt;
}

template <typename... Rs>
uint8_t Usart<Rs...>::Read() {
    while(!ReadReady());
    return Regs()->RXDATA;
}

template <typename... Rs>
template <typename Ty>
uint8_t Usart<Rs...>::Read(WrData<Ty> data, uint32_t tmeout = {}) {
    return {};
}

template <typename... Rs>
template <typename Ty>
void Usart<Rs...>::EnableAsyncRead(WrData<Ty> data, TransferCallback<Ty> callback) {
    // DmaRx::ClearTransferComplete();
    // Regs()->CONTROL3 |= UART_CONTROL3_DMAR_M;
    // DmaRx::SetTransferCallback(callback);
    // DmaRx::Transfer(DmaRx::Periph2Mem | DmaRx::MemIncrement, receiveBuffer, &Regs()->RXDATA, bufferSize);
}

template <typename... Rs>
bool Usart<Rs...>::WriteReady() {
    if constexpr(!std::is_same_v<DmaTx, void>) {
        bool dmaActive = (Regs()->CONTROL3 & UART_CONTROL3_DMAT_M) && DmaTx::Enabled();
        return (!dmaActive || DmaTx::TransferComplete()) && (Regs()->FLAGS & TxEmptyInt);
    } else {
        return Regs()->FLAGS & TxEmptyInt;
    }
}

// template <typename... Rs>
// template <typename Ty>
// void Usart<Rs...>::Write(WrData<Ty> data, bool async) {
//     if(async && data.size() > 1) {
//         WriteAsync(data);
//     } else {
//         Write(data);
//     }
// }

template <typename... Rs>
template <typename Ty>
void Usart<Rs...>::Write(WrData<Ty> data) {
    for(Ty val: data) Write(static_cast<uint8_t>(val));
}

template <typename... Rs>
template <typename Ty>
void Usart<Rs...>::WriteAsync(WrData<Ty> data, TransferCallback<Ty> callback) {
    // if(size == 0)
    //     return;

    // while(!WriteReady());
    // DmaTx::ClearTransferComplete();
    // DmaTx::SetTransferCallback(callback);
    // Regs()->CONTROL3 |= UART_CONTROL3_DMAT_M;
    // #if defined(USART_TYPE_1)
    // Regs()->ICR = TxCompleteInt;
    // #endif
    // #if defined(USART_TYPE_2)
    // Regs()->SR &= ~TxCompleteInt;
    // #endif
    // DmaTx::Transfer(DmaTx::Mem2Periph | DmaTx::MemIncrement, data, &Regs()->TXDATA, size);
}

template <typename... Rs>
void Usart<Rs...>::Write(uint8_t data) {
    while(!WriteReady()) continue;
    Regs()->TXDATA = data;
}

#if defined(UART_CONTROL2_RTOEN_M)
USART_TEMPLATE_ARGS
inline void USART_TEMPLATE_QUALIFIER::EnableReceiverTimeout(uint32_t bitCount) {
    // Regs()->CONTROL2 |= UART_CONTROL2_RTOEN_M;
    // SetReceiverTimeout(bitCount);
}

USART_TEMPLATE_ARGS
inline void USART_TEMPLATE_QUALIFIER::SetReceiverTimeout(uint32_t bitCount) {
    // Regs()->RTOR = (Regs()->RTOR & ~USART_RTOR_RTO_Msk)
    //     | (bitCount << USART_RTOR_RTO_Pos);
}
#endif

template <typename... Rs>
void Usart<Rs...>::EnableInterrupt(InterruptFlags interruptFlags) {
    uint32_t cr1Mask = 0;
    uint32_t cr2Mask = 0;
    uint32_t cr3Mask = 0;

    if(interruptFlags & ParityErrorInt)
        cr1Mask |= UART_CONTROL1_PEIE_M;

#if defined(UART_CONTROL2_RTOEN_M)
// if(interruptFlags & ReceiveTimeout)
//     cr1Mask |= UART_CONTROL1_RTOIE_M;
#endif

    static_assert(
        UART_CONTROL1_TXEIE_M == TxEmptyInt && UART_CONTROL1_TCIE_M == TxCompleteInt && UART_CONTROL1_RXNEIE_M == RxNotEmptyInt && UART_CONTROL1_IDLEIE_M == IdleInt);

    cr1Mask |= interruptFlags & (UART_CONTROL1_TXEIE_M | UART_CONTROL1_TCIE_M | UART_CONTROL1_RXNEIE_M | UART_CONTROL1_IDLEIE_M);

#if defined(UART_CONTROL2_LBDIE_M)
// if(interruptFlags & LineBreakInt)
//     cr2Mask |= UART_CONTROL2_LBDIE_M;
#endif

    if(interruptFlags & ErrorInt)
        cr3Mask |= UART_CONTROL3_EIE_M;

    if(interruptFlags & CtsInt)
        cr3Mask |= UART_CONTROL3_CTSIE_M;

    Regs()->CONTROL1 |= cr1Mask;
    Regs()->CONTROL2 |= cr2Mask;
    Regs()->CONTROL3 |= cr3Mask;

    // TODO if(interruptFlags != NoInterrupt) NVIC_EnableIRQ(_IRQNumber);
}

template <typename... Rs>
void Usart<Rs...>::DisableInterrupt(InterruptFlags interruptFlags) {
    // uint32_t cr1Mask = 0;
    // uint32_t cr2Mask = 0;
    // uint32_t cr3Mask = 0;

    // if(interruptFlags & ParityErrorInt)
    //     cr1Mask |= UART_CONTROL1_PEIE_M;

    // #if defined(UART_CONTROL2_RTOEN_M)
    // if(interruptFlags & ReceiveTimeout)
    //     cr1Mask |= UART_CONTROL1_RTOIE_M;
    // #endif

    // static_assert(
    //     UART_CONTROL1_TXEIE_M == TxEmptyInt && UART_CONTROL1_TCIE_M == TxCompleteInt && UART_CONTROL1_RXNEIE_M == RxNotEmptyInt && UART_CONTROL1_IDLEIE_M == IdleInt);

    // cr1Mask |= interruptFlags & (UART_CONTROL1_TXEIE_M | UART_CONTROL1_TCIE_M | UART_CONTROL1_RXNEIE_M | UART_CONTROL1_IDLEIE_M);

    // #if defined(UART_CONTROL2_LBDIE_M)
    // if(interruptFlags & LineBreakInt)
    //     cr2Mask |= UART_CONTROL2_LBDIE_M;
    // #endif
    // if(interruptFlags & ErrorInt)
    //     cr3Mask |= UART_CONTROL3_EIE_M;

    // if(interruptFlags & CtsInt)
    //     cr3Mask |= UART_CONTROL3_CTSIE_M;

    // Regs()->CONTROL1 &= ~cr1Mask;
    // Regs()->CONTROL2 &= ~cr2Mask;
    // Regs()->CONTROL3 &= ~cr3Mask;
}

template <typename... Rs>
typename Usart<Rs...>::InterruptFlags Usart<Rs...>::InterruptSource() {
    return static_cast<InterruptFlags>(Regs()->FLAGS & InterruptMask);
}

template <typename... Rs>
typename Usart<Rs...>::Error Usart<Rs...>::GetError() {
    return static_cast<Error>(Regs()->FLAGS & ErrorMask);
}

template <typename... Rs>
void Usart<Rs...>::ClearInterruptFlag(Usart<Rs...>::InterruptFlags interruptFlags) {
    Regs()->FLAGS &= ~interruptFlags;
}

template <typename... Rs>
void Usart<Rs...>::ClearAllInterruptFlags() {
    Regs()->FLAGS = 0UL; // 0xffffffffUL;
}

} // namespace Zhele::Private
