/**
 * @file
 * UART methods implementation
 * 
 * @author Konstantin Chizhov
 * @date ??
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IMPL_USART_H
#define ZHELE_PLATFORM_STM32_COMMON_IMPL_USART_H

namespace Zhele
{
    namespace Private
    {

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        template<unsigned long baud>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::Init(UsartMode mode)
        {
            Init(baud, mode);
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::Init(unsigned baud, UsartMode mode)
        {
            _ClockCtrl::Enable();
            SetBaud(baud);
            _Regs()->[:StatusMember:] = 0x00;
            _Regs()->CR3 = mode.CR3;
            _Regs()->CR2 = mode.CR2;
            _Regs()->CR1 = mode.CR1 | USART_CR1_UE;
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::SetConfig(UsartMode modeMask)
        {
            _Regs()->CR3 |= modeMask.CR3;
            _Regs()->CR2 |= modeMask.CR2;
            _Regs()->CR1 |= modeMask.CR1;
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::ClearConfig(UsartMode modeMask)
        {
            _Regs()->CR3 &= ~modeMask.CR3;
            _Regs()->CR2 &= ~modeMask.CR2;
            _Regs()->CR1 &= ~modeMask.CR1;
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::SetBaud(unsigned baud)
        {
            _Regs()->BRR = _ClockCtrl::ClockFreq() / baud;
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        bool Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::ReadReady()
        {
            return _Regs()->[:StatusMember:] & RxNotEmptyInt;
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        uint8_t Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::Read()
        {
            while(!ReadReady())
                ;
            return _Regs()->[:RxDataMember:];
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::EnableAsyncRead(void* receiveBuffer, size_t bufferSize, TransferCallback callback)
        {
            _DmaRx::ClearTransferComplete();
            _Regs::template SetBits<^^USART_TypeDef::CR3, USART_CR3_DMAR>();
            _DmaRx::SetTransferCallback(callback);
            _DmaRx::Transfer(_DmaRx::Periph2Mem | _DmaRx::MemIncrement, receiveBuffer, &_Regs()->[:RxDataMember:], bufferSize);
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        bool Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::WriteReady()
        {
            if constexpr (!std::is_same_v<_DmaTx, void>) {
                bool dmaActive = (_Regs()->CR3 & USART_CR3_DMAT) && _DmaTx::Enabled();
                return (!dmaActive || _DmaTx::TransferComplete()) && (_Regs()->[:StatusMember:] & TxEmptyInt);
            } else {
                return _Regs()->[:StatusMember:] & TxEmptyInt;
            }
            
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::Write(const void* data, size_t size, bool async)
        {
            if (async && size > 1) {
                WriteAsync(data, size);
            }  else {
                Write(data, size);
            }
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::Write(const void* data, size_t size)
        {
            const uint8_t *ptr = static_cast<const uint8_t*>(data);
            while (size--) {
                Write(*ptr++);
            }
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::WriteAsync(const void* data, size_t size, TransferCallback callback)
        {
            if (size == 0)
                return;
            
            while (!WriteReady()) ;
            _DmaTx::ClearTransferComplete();
            _DmaTx::SetTransferCallback(callback);
            _Regs::template SetBits<^^USART_TypeDef::CR3, USART_CR3_DMAT>();
        #if defined (USART_ISR_PE)
            _Regs()->ICR = TxCompleteInt;
        #endif
        #if defined (USART_SR_PE)
            _Regs()->SR &= ~TxCompleteInt;
        #endif
            _DmaTx::Transfer(_DmaTx::Mem2Periph | _DmaTx::MemIncrement, data, &_Regs()->[:TxDataMember:], size);
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::Write(uint8_t data)
        {
            while (!WriteReady()) continue;

            _Regs()->[:TxDataMember:] = data;  
        }

#if defined (USART_CR2_RTOEN)
        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        inline void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::EnableReceiverTimeout(uint32_t bitCount)
        {
            _Regs::template SetBits<^^USART_TypeDef::CR2, USART_CR2_RTOEN>();
            SetReceiverTimeout(bitCount);
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        inline void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::SetReceiverTimeout(uint32_t bitCount)
        {
            _Regs()->RTOR = (_Regs()->RTOR & ~USART_RTOR_RTO_Msk)
                | (bitCount << USART_RTOR_RTO_Pos);
        }
#endif
        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::EnableInterrupt(InterruptFlags interruptFlags)
        {
            uint32_t cr1Mask = 0;
            uint32_t cr2Mask = 0;
            uint32_t cr3Mask = 0;

            if(interruptFlags & ParityErrorInt)
                cr1Mask |= USART_CR1_PEIE;

        #if defined (USART_CR2_RTOEN)
            if(interruptFlags & ReceiveTimeout)
                cr1Mask |= USART_CR1_RTOIE;
        #endif

            static_assert(
                USART_CR1_TXEIE  == TxEmptyInt &&
                USART_CR1_TCIE   == TxCompleteInt &&
                USART_CR1_RXNEIE == RxNotEmptyInt &&
                USART_CR1_IDLEIE == IdleInt
            );

            cr1Mask |= interruptFlags & (USART_CR1_TXEIE | USART_CR1_TCIE | USART_CR1_RXNEIE | USART_CR1_IDLEIE);

        #if defined (USART_CR2_LBDIE)
            if(interruptFlags & LineBreakInt)
                cr2Mask |= USART_CR2_LBDIE;
        #endif

            if(interruptFlags & ErrorInt)
                cr3Mask |= USART_CR3_EIE;

            if(interruptFlags & CtsInt)
                cr3Mask |= USART_CR3_CTSIE;

            _Regs()->CR1 |= cr1Mask;
            _Regs()->CR2 |= cr2Mask;
            _Regs()->CR3 |= cr3Mask;

            if(interruptFlags != NoInterrupt)
                NVIC_EnableIRQ(_IRQNumber);
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::DisableInterrupt(InterruptFlags interruptFlags)
        {
            uint32_t cr1Mask = 0;
            uint32_t cr2Mask = 0;
            uint32_t cr3Mask = 0;

            if(interruptFlags & ParityErrorInt)
                cr1Mask |= USART_CR1_PEIE;

        #if defined (USART_CR2_RTOEN)
            if(interruptFlags & ReceiveTimeout)
                cr1Mask |= USART_CR1_RTOIE;
        #endif

            static_assert(
                    USART_CR1_TXEIE  == TxEmptyInt &&
                    USART_CR1_TCIE   == TxCompleteInt &&
                    USART_CR1_RXNEIE == RxNotEmptyInt &&
                    USART_CR1_IDLEIE == IdleInt
                    );

            cr1Mask |= interruptFlags & (USART_CR1_TXEIE | USART_CR1_TCIE | USART_CR1_RXNEIE | USART_CR1_IDLEIE);

        #if defined (USART_CR2_LBDIE)
            if(interruptFlags & LineBreakInt)
                cr2Mask |= USART_CR2_LBDIE;
        #endif
            if(interruptFlags & ErrorInt)
                cr3Mask |= USART_CR3_EIE;

            if(interruptFlags & CtsInt)
                cr3Mask |= USART_CR3_CTSIE;

            _Regs()->CR1 &= ~cr1Mask;
            _Regs()->CR2 &= ~cr2Mask;
            _Regs()->CR3 &= ~cr3Mask;
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        typename Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::InterruptFlags Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::InterruptSource()
        {
            return static_cast<InterruptFlags>(_Regs()->[:StatusMember:] & InterruptMask);
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        typename Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::Error Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::GetError()
        {
            return static_cast<Error>(_Regs()->[:StatusMember:] & ErrorMask);
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::ClearInterruptFlag(InterruptFlags interruptFlags)
        {
        #if defined (USART_ISR_PE)
            _Regs()->ICR = interruptFlags;
        #endif
        #if defined (USART_SR_PE)
            _Regs()->SR &= ~interruptFlags;
        #endif
        }

        template<typename _Regs, IRQn_Type _IRQNumber, typename _ClockCtrl, typename _TxPins, typename _RxPins, typename _DmaTx, typename _DmaRx>
        inline void Usart<_Regs, _IRQNumber, _ClockCtrl, _TxPins, _RxPins, _DmaTx, _DmaRx>::ClearAllInterruptFlags()
        {
        #if defined (USART_ISR_PE)
            _Regs()->ICR = 0xffffffffUL;
        #endif
        #if defined (USART_SR_PE)
            _Regs()->SR = 0x00000000;
        #endif
        }
    }
}
#endif
