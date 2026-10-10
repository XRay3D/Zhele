/**
 * @file
 * DMA methods implementation
 * 
 * @author Konstantin Chizhov
 * @date ??
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IMPL_DMA_H
#define ZHELE_PLATFORM_STM32_COMMON_IMPL_DMA_H

namespace Zhele
{
    void DmaChannelData::NotifyTransferComplete()
    {
        if(transferCallback)
        {
            transferCallback(data, size, true);
        }
    }

    void DmaChannelData::NotifyError()
    {            
        if(transferCallback)
        {
            transferCallback(data, size, false);
        }
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::Transfer(Mode mode, const void* buffer, volatile void* periph, uint32_t bufferSize)
    {
        TransferImpl(mode, buffer, periph, bufferSize, 0);
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::Transfer(Mode mode, const void* buffer, volatile void* periph, uint32_t bufferSize, uint8_t channel)
        requires (dmaStreamSupported)
    {
        TransferImpl(mode, buffer, periph, bufferSize, channel);
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::TransferImpl(Mode mode, const void* buffer, volatile void* periph, uint32_t bufferSize, [[maybe_unused]] uint8_t channel)
    {
        _Module::Enable();
        if(!TransferError())
        {
            while(!Ready())
                ;
        }
        _ChannelRegs()->[:ControlMember:] = 0;
        _ChannelRegs()->[:CountMember:] = bufferSize;
        _ChannelRegs()->[:PeriphAddrMember:] = reinterpret_cast<uint32_t>(periph);
        _ChannelRegs()->[:MemAddrMember:] = reinterpret_cast<uint32_t>(buffer);
    Data.data = const_cast<void*>(buffer);
    Data.size = bufferSize;

    if(Data.transferCallback)
        mode = mode | DmaBase::TransferCompleteInterrupt | DmaBase::TransferErrorInterrupt;

    NVIC_EnableIRQ(_IRQNumber);

    #if defined (DMA_CSELR_C1S)
        _Module::template SetChannelSelect<_Channel>(channel);
    #endif
        if constexpr (dmaHasStreams)
            _ChannelRegs()->[:ControlMember:] = mode | ((channel & 0x07) << 25) | EnableBit;
        else
            _ChannelRegs()->[:ControlMember:] = mode | EnableBit;
    }
    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::SetTransferCallback(DmaChannelData::TransferCallback callback)
    {
        Data.transferCallback = callback;
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    bool DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::Ready()
    {
        return RemainingTransfers() == 0 || !Enabled() || TransferComplete();
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    bool DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::Enabled()
    {
        return _ChannelRegs()->[:ControlMember:] & EnableBit;
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::Enable()
    {
        _ChannelRegs()->[:ControlMember:] |= EnableBit;
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::Disable()
    {
        _ChannelRegs()->[:ControlMember:] &= ~EnableBit;
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    uint32_t DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::RemainingTransfers()
    {
        return _ChannelRegs()->[:CountMember:];
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void* DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::PeriphAddress()
    {
        return reinterpret_cast<void *>(_ChannelRegs()->[:PeriphAddrMember:]);
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void* DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::MemAddress()
    {
        return reinterpret_cast<void *>(_ChannelRegs()->[:MemAddrMember:]);
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    bool DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::TransferError()
    {
        return _Module::template TransferError<_Channel>();
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    bool DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::HalfTransfer()
    {
        return _Module::template HalfTransfer<_Channel>();
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    bool DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::TransferComplete()
    {
        return _Module::template TransferComplete<_Channel>();
    }
#if defined(DMA_CCR_EN)
    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    bool DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::Interrupt()
    {
        return _Module::template Interrupt<_Channel>();
    }
#endif
    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::ClearFlags()
    {
        _Module::template ClearChannelFlags<_Channel>();
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::ClearTransferError()
    {
        _Module::template ClearTransferError<_Channel>();
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::ClearHalfTransfer()
    {
        _Module::template ClearHalfTransfer<_Channel>();
    }

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::ClearTransferComplete() 
    {
        _Module::template ClearTransferComplete<_Channel>();
    }
#if defined(DMA_CCR_EN)
    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::ClearInterrupt()
    {
        _Module::template ClearInterrupt<_Channel>();
    }
#endif
    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    void DmaChannel<_Module, _ChannelRegs, _Channel, _IRQNumber>::IrqHandler()
    {
        if(TransferComplete())
        {
            ClearFlags();
            
            if(static_cast<uint32_t>(_ChannelRegs()->[:ControlMember:] & Mode::Circular) == 0)
                Disable();

            Data.NotifyTransferComplete();
        }
        if(TransferError())
        {
            ClearFlags();

            if(static_cast<uint32_t>(_ChannelRegs()->[:ControlMember:] & Mode::Circular) == 0)
                Disable();

            Data.NotifyError();
        }
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum, typename DmaModule<_DmaRegs, _Clock, _Channels>::Flags FlagMask>
    bool DmaModule<_DmaRegs, _Clock, _Channels>::ChannelFlag()
    {
    #if defined(DMA_CCR_EN)
        return _DmaRegs()->ISR & (static_cast<uint32_t>(FlagMask) << ((ChannelNum - 1) * 4));
    #endif
    #if defined(DMA_SxCR_EN)
        if constexpr(ChannelNum <= 1)
        {
            return _DmaRegs()->LISR & (static_cast<uint32_t>(FlagMask) << (ChannelNum * 6));
        }
        if constexpr(2 <= ChannelNum && ChannelNum <= 3)
        {
            return _DmaRegs()->LISR & (static_cast<uint32_t>(FlagMask) << (4 + ChannelNum * 6));
        }
        if constexpr(4 <= ChannelNum && ChannelNum <= 5)
        {
            return _DmaRegs()->HISR & (static_cast<uint32_t>(FlagMask) << ((ChannelNum - 4) * 6));
        }
        if constexpr(6 <= ChannelNum && ChannelNum <= 7)
        {
            return _DmaRegs()->HISR & (static_cast<uint32_t>(FlagMask) << (4 + (ChannelNum - 4) * 6));
        }
        return false;
    #endif
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum, typename DmaModule<_DmaRegs, _Clock, _Channels>::Flags FlagMask>
    void DmaModule<_DmaRegs, _Clock, _Channels>::ClearChannelFlag()
    {
    #if defined(DMA_CCR_EN)
        static_assert(ChannelNum > 0);
        _DmaRegs()->IFCR = (static_cast<uint32_t>(FlagMask) << ((ChannelNum - 1) * 4));
    #endif
    #if defined(DMA_SxCR_EN)
        if constexpr(ChannelNum <= 1)
        {
            _DmaRegs()->LIFCR = (static_cast<uint32_t>(FlagMask) << (ChannelNum * 6));
        }
        if constexpr(2 <= ChannelNum && ChannelNum <= 3)
        {
            _DmaRegs()->LIFCR = (static_cast<uint32_t>(FlagMask) << (4 + ChannelNum * 6));
        }
        if constexpr(4 <= ChannelNum && ChannelNum <= 5)
        {
            _DmaRegs()->HIFCR = (static_cast<uint32_t>(FlagMask) << ((ChannelNum - 4) * 6));
        }
        if constexpr(6 <= ChannelNum && ChannelNum <= 7)
        {
            _DmaRegs()->HIFCR = (static_cast<uint32_t>(FlagMask) << (4 + (ChannelNum - 4) * 6));
        }
    #endif
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    bool DmaModule<_DmaRegs, _Clock, _Channels>::TransferError()
    {
        return ChannelFlag<ChannelNum, Flags::TransferError>();
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    bool DmaModule<_DmaRegs, _Clock, _Channels>::HalfTransfer()
    {
        return ChannelFlag<ChannelNum, Flags::HalfTransfer>();
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    bool DmaModule<_DmaRegs, _Clock, _Channels>::TransferComplete()
    {
        return ChannelFlag<ChannelNum, Flags::TransferComplete>();
    }

#if defined(DMA_SxCR_EN)
    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    bool DmaModule<_DmaRegs, _Clock, _Channels>::FifoError()
    {
        return ChannelFlag<ChannelNum, Flags::FifoError>();
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    bool DmaModule<_DmaRegs, _Clock, _Channels>::DirectError()
    {
        return ChannelFlag<ChannelNum, Flags::DirectError>();
    }
#endif
#if defined(DMA_CCR_EN)
    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    bool DmaModule<_DmaRegs, _Clock, _Channels>::Interrupt()
    {
        return ChannelFlag<ChannelNum, Flags::Global>();
    }
#endif
    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    void DmaModule<_DmaRegs, _Clock, _Channels>::ClearChannelFlags()
    {
        ClearChannelFlag<ChannelNum, Flags::All>();
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    void DmaModule<_DmaRegs, _Clock, _Channels>::ClearTransferError()
    {
        ClearChannelFlag<ChannelNum, Flags::TransferError>();
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    void DmaModule<_DmaRegs, _Clock, _Channels>::ClearHalfTransfer()
    {
        ClearChannelFlag<ChannelNum, Flags::HalfTransfer>();
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    void DmaModule<_DmaRegs, _Clock, _Channels>::ClearTransferComplete()
    {
        ClearChannelFlag<ChannelNum, Flags::TransferComplete>();
    }
#if defined(DMA_CCR_EN)
    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    void DmaModule<_DmaRegs, _Clock, _Channels>::ClearInterrupt()
    {
        ClearChannelFlag<ChannelNum, Flags::Global>();
    }
#endif
#if defined(DMA_SxCR_EN)
    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    void DmaModule<_DmaRegs, _Clock, _Channels>::ClearFifoError()
    {
        ClearChannelFlag<ChannelNum, Flags::FifoError>();
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    template<int ChannelNum>
    void DmaModule<_DmaRegs, _Clock, _Channels>::ClearDirectError()
    {
        ClearChannelFlag<ChannelNum, Flags::DirectError>();
    }
#endif

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    void DmaModule<_DmaRegs, _Clock, _Channels>::Enable()
    {
        _Clock::Enable();
    }

    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    void DmaModule<_DmaRegs, _Clock, _Channels>::Disable()
    {
        _Clock::Disable();
    }

    #if defined (DMA_CSELR_C1S)
        template<typename _DmaRegs, typename _Clock, unsigned _Channels>
        template<uint8_t channel>
        void DmaModule<_DmaRegs, _Clock, _Channels>::SetChannelSelect(uint8_t channelSelect)
        {
            static const uint32_t ChannelSelectRegisterOffset = 0x0a8;

            *(reinterpret_cast<uint32_t*>(_DmaRegs::Get()) + ChannelSelectRegisterOffset) = 
                (*(reinterpret_cast<uint32_t*>(_DmaRegs::Get()) + ChannelSelectRegisterOffset) & ~(0xf << 4 * channel)) 
                | (channelSelect << 4 * channel);
        }
    #endif
}
#endif //! ZHELE_PLATFORM_STM32_COMMON_IMPL_DMA_H