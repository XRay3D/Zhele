/**
 * @file
 * UART methods implementation
 *
 * @author Konstantin Chizhov / X-Ray
 * @date 2026
 * @license MIT
 */

#pragma once

namespace Zhele {
namespace Private {
#define USART_TEMPLATE_ARGS      template <typename _Regs, uint32_t _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
#define USART_TEMPLATE_QUALIFIER Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>

USART_TEMPLATE_ARGS
template <unsigned long baud>
void USART_TEMPLATE_QUALIFIER::Init(UsartMode mode) { Init(baud, mode); }

USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::Init(unsigned baud, UsartMode mode) {
    _ClockCtrl::Enable();
    SetBaud(baud);
    _Regs()->FLAGS    = 0x00;
    _Regs()->CONTROL1 = UART_CONTROL1_UE_M;
    _Regs()->CONTROL3 = mode.CONTROL3;
    _Regs()->CONTROL2 = mode.CONTROL2;
    _Regs()->CONTROL1 |= mode.CONTROL1;
}

USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::SetConfig(UsartMode modeMask) {
    _Regs()->CONTROL3 |= modeMask.CONTROL3;
    _Regs()->CONTROL2 |= modeMask.CONTROL2;
    _Regs()->CONTROL1 |= modeMask.CONTROL1;
}

USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::ClearConfig(UsartMode modeMask) {
    _Regs()->CONTROL3 &= ~modeMask.CONTROL3;
    _Regs()->CONTROL2 &= ~modeMask.CONTROL2;
    _Regs()->CONTROL1 &= ~modeMask.CONTROL1;
}

USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::SetBaud(unsigned baud) {
    _Regs()->DIVIDER =/*_ClockCtrl::ClockFreq()*/32000000u / baud;
}

USART_TEMPLATE_ARGS
bool USART_TEMPLATE_QUALIFIER::ReadReady() {
    return _Regs()->FLAGS & RxNotEmptyInt;
}

USART_TEMPLATE_ARGS
uint8_t USART_TEMPLATE_QUALIFIER::Read() {
    while(!ReadReady());
    return _Regs()->RXDATA;
}

// USART_TEMPLATE_ARGS
// void USART_TEMPLATE_QUALIFIER::EnableAsyncRead(void* receiveBuffer, size_t bufferSize, TransferCallback callback) {
// _DmaRx::ClearTransferComplete();
// _Regs()->CONTROL3 |= UART_CONTROL3_DMAR_M;
// _DmaRx::SetTransferCallback(callback);
// _DmaRx::Transfer(_DmaRx::Periph2Mem | _DmaRx::MemIncrement, receiveBuffer, &_Regs()->RXDATA, bufferSize);
// }

USART_TEMPLATE_ARGS
bool USART_TEMPLATE_QUALIFIER::WriteReady() {
    if constexpr(!std::is_same_v<_DmaTx, void>) {
        bool dmaActive = (_Regs()->CONTROL3 & UART_CONTROL3_DMAT_M) && _DmaTx::Enabled();
        return (!dmaActive || _DmaTx::TransferComplete()) && (_Regs()->FLAGS & TxEmptyInt);
    } else {
        return _Regs()->FLAGS & TxEmptyInt;
    }
}

USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::Write(const void* data, size_t size, bool async) {
    // if(async && size > 1) {
    //     WriteAsync(data, size);
    // } else {
    //     Write(data, size);
    // }
}

USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::Write(const void* data, size_t size) {
    const uint8_t* ptr = static_cast<const uint8_t*>(data);
    while(size--) {
        Write(*ptr++);
    }
}
#if 0
USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::WriteAsync(const void* data, size_t size, TransferCallback callback) {
// if(size == 0)
//     return;

// while(!WriteReady());
// _DmaTx::ClearTransferComplete();
// _DmaTx::SetTransferCallback(callback);
// _Regs()->CONTROL3 |= UART_CONTROL3_DMAT_M;
    #if defined(USART_TYPE_1)
// _Regs()->ICR = TxCompleteInt;
    #endif
    #if defined(USART_TYPE_2)
// _Regs()->SR &= ~TxCompleteInt;
    #endif
    // _DmaTx::Transfer(_DmaTx::Mem2Periph | _DmaTx::MemIncrement, data, &_Regs()->TXDATA, size);
}
#endif

USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::Write(uint8_t data) {
    while(!WriteReady()) continue;
    _Regs()->TXDATA = data;
}

#if defined(UART_CONTROL2_RTOEN_M)
USART_TEMPLATE_ARGS
inline void USART_TEMPLATE_QUALIFIER::EnableReceiverTimeout(uint32_t bitCount) {
    // _Regs()->CONTROL2 |= UART_CONTROL2_RTOEN_M;
    // SetReceiverTimeout(bitCount);
}

USART_TEMPLATE_ARGS
inline void USART_TEMPLATE_QUALIFIER::SetReceiverTimeout(uint32_t bitCount) {
    // _Regs()->RTOR = (_Regs()->RTOR & ~USART_RTOR_RTO_Msk)
    //     | (bitCount << USART_RTOR_RTO_Pos);
}
#endif
USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::EnableInterrupt(InterruptFlags interruptFlags) {
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

    _Regs()->CONTROL1 |= cr1Mask;
    _Regs()->CONTROL2 |= cr2Mask;
    _Regs()->CONTROL3 |= cr3Mask;

    // TODO if(interruptFlags != NoInterrupt) NVIC_EnableIRQ(_IRQNumber);
}

USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::DisableInterrupt(InterruptFlags interruptFlags) {
    // uint32_t cr1Mask = 0;
    // uint32_t cr2Mask = 0;
    // uint32_t cr3Mask = 0;

    // if(interruptFlags & ParityErrorInt)
    //     cr1Mask |= UART_CONTROL1_PEIE_M;

#if defined(UART_CONTROL2_RTOEN_M)
// if(interruptFlags & ReceiveTimeout)
//     cr1Mask |= UART_CONTROL1_RTOIE_M;
#endif

    // static_assert(
    //     UART_CONTROL1_TXEIE_M == TxEmptyInt && UART_CONTROL1_TCIE_M == TxCompleteInt && UART_CONTROL1_RXNEIE_M == RxNotEmptyInt && UART_CONTROL1_IDLEIE_M == IdleInt);

    // cr1Mask |= interruptFlags & (UART_CONTROL1_TXEIE_M | UART_CONTROL1_TCIE_M | UART_CONTROL1_RXNEIE_M | UART_CONTROL1_IDLEIE_M);

#if defined(UART_CONTROL2_LBDIE_M)
// if(interruptFlags & LineBreakInt)
//     cr2Mask |= UART_CONTROL2_LBDIE_M;
#endif
    // if(interruptFlags & ErrorInt)
    //     cr3Mask |= UART_CONTROL3_EIE_M;

    // if(interruptFlags & CtsInt)
    //     cr3Mask |= UART_CONTROL3_CTSIE_M;

    // _Regs()->CONTROL1 &= ~cr1Mask;
    // _Regs()->CONTROL2 &= ~cr2Mask;
    // _Regs()->CONTROL3 &= ~cr3Mask;
}

USART_TEMPLATE_ARGS
typename USART_TEMPLATE_QUALIFIER::InterruptFlags USART_TEMPLATE_QUALIFIER::InterruptSource() {
    return static_cast<InterruptFlags>(_Regs()->FLAGS & InterruptMask);
}

USART_TEMPLATE_ARGS
typename USART_TEMPLATE_QUALIFIER::Error USART_TEMPLATE_QUALIFIER::GetError() {
    return static_cast<Error>(_Regs()->FLAGS & ErrorMask);
}

USART_TEMPLATE_ARGS
void USART_TEMPLATE_QUALIFIER::ClearInterruptFlag(InterruptFlags interruptFlags) {
#if defined(USART_TYPE_1)
// _Regs()->ICR = interruptFlags;
#endif
#if defined(USART_TYPE_2)
// _Regs()->SR &= ~interruptFlags;
#endif
}

USART_TEMPLATE_ARGS
inline void USART_TEMPLATE_QUALIFIER::ClearAllInterruptFlags() {
#if defined(USART_TYPE_1)
// _Regs()->ICR = 0xffffffffUL;
#endif
#if defined(USART_TYPE_2)
// _Regs()->SR = 0x00000000;
#endif
}
}
} // namespace Zhele::Private
