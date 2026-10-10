/**
 * @file
 * SPI methods implementation
 * @author Konstantin Chizhov
 * @date ??
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IMPL_SPI_H
#define ZHELE_PLATFORM_STM32_COMMON_IMPL_SPI_H

namespace Zhele::Private
{

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::Enable()
    {
        _Regs::template SetBits<^^SPI_TypeDef::CR1, SPI_CR1_SPE>();
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::Disable()
    {
        _Regs::template ClearBits<^^SPI_TypeDef::CR1, SPI_CR1_SPE>();
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::Init(Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::ClockDivider divider, Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::Mode mode)
    {
        _Clock::Enable();
        _Regs()->CR1 = static_cast<unsigned>(divider) | mode;
        _Regs()->CR2 = (mode >> 16) | SPI_CR2_SSOE;
        SetDataSize(DataSize::DataSize8);
#if defined (SPI_I2SCFGR_I2SMOD)
        _Regs()->I2SCFGR &= (uint16_t)~((uint16_t)SPI_I2SCFGR_I2SMOD);
#endif
        Enable();
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::SetDivider(Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::ClockDivider divider)
    {
        _Regs()->CR1 = (_Regs()->CR1 & ~SPI_CR1_BR) | divider;
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::SetClockPolarity(Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::ClockPolarity clockPolarity)
    {
        _Regs()->CR1 = (_Regs()->CR1 & ~SPI_CR1_CPOL) | clockPolarity;
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::SetClockPhase(Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::ClockPhase clockPhase)
    {
        _Regs()->CR1 = (_Regs()->CR1 & ~SPI_CR1_CPHA) | clockPhase;
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::SetBitOrder(Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::BitOrder bitOrder)
    {
        _Regs()->CR1 = (_Regs()->CR1 & ~SPI_CR1_LSBFIRST) | bitOrder;
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::SetDataSize(Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::DataSize dataSize)
    {
        #if defined (SPI_CR1_DFF)
            _Regs()->CR1 = (_Regs()->CR1 & ~SPI_CR1_DFF_Msk) | dataSize;
        #else
            _Regs()->CR2 = (_Regs()->CR2 & ~SPI_CR2_DS) | dataSize;
            #if defined(SPI_CR2_FRXTH)
                if(dataSize <= DataSize8)
                {
                    _Regs::template SetBits<^^SPI_TypeDef::CR2, SPI_CR2_FRXTH>();
                }
            #endif
        #endif
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::SetSlaveControl(Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::SlaveControl slaveControl)
    {
        _Regs()->CR1 = (_Regs()->CR1 & ~SPI_CR1_SSM) | slaveControl;
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::SetSS()
    {
        _Regs::template SetBits<^^SPI_TypeDef::CR1, SPI_CR1_SSI>();
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::ClearSS()
    {
        _Regs::template ClearBits<^^SPI_TypeDef::CR1, SPI_CR1_SSI>();
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    bool Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::Busy()
    {
        return (_Regs()->SR & SPI_SR_BSY) > 0;
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    uint16_t Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::Send(uint16_t value)
    {
        while ((_Regs()->SR & SPI_SR_TXE) == 0);

    #if defined(SPI_CR1_DFF)
        if(_Regs()->CR1 & SPI_CR1_DFF)
    #else
        if((_Regs()->CR2 & SPI_CR2_DS) > DataSize8)
    #endif
        {
            _Regs()->DR = value;
        }
        else
        {
            *(__IO uint8_t*)&_Regs()->DR = static_cast<uint8_t>(value);
        }
        
        while ((_Regs()->SR & SPI_SR_RXNE) == 0);
    #if defined(SPI_CR1_DFF)
        if(_Regs()->CR1 & SPI_CR1_DFF)
    #else
        if((_Regs()->CR2 & SPI_CR2_DS) > DataSize8)
    #endif
        {
            return _Regs()->DR;
        }
        else
        {
            return *(__IO uint8_t*)&_Regs()->DR;
        }
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::SendAsync(void* transmitBuffer, void* receiveBuffer, size_t bufferSize, TransferCallback callback)
    {
        _DmaRx::ClearTransferComplete();
        _Regs()->CR2 |= (SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);
        auto dataSize = 
        #if defined(SPI_CR1_DFF)
            (_Regs()->CR1 & SPI_CR1_DFF) > 0
        #else
            (_Regs()->CR2 & SPI_CR2_DS) > DataSize8
        #endif
            ? (_DmaTx::PSize16Bits | _DmaTx::MSize16Bits)
            : (_DmaTx::PSize8Bits | _DmaTx::MSize8Bits);
        _DmaRx::SetTransferCallback(callback);
        _DmaRx::Transfer(_DmaRx::Periph2Mem | _DmaRx::MemIncrement | _DmaRx::Circular | dataSize, receiveBuffer, &_Regs()->DR, bufferSize);

        _DmaTx::Transfer(_DmaTx::Mem2Periph | _DmaRx::MemIncrement | dataSize, transmitBuffer, &_Regs()->DR, bufferSize);
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::Write(uint16_t data)
    {
        Send(data);
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::WriteAsync(const void* data, uint16_t size, TransferCallback callback)
    {
        _DmaTx::ClearTransferComplete();
        _Regs::template SetBits<^^SPI_TypeDef::CR2, SPI_CR2_TXDMAEN>();
        typename _DmaTx::Mode dataSize = 
        #if defined(SPI_CR1_DFF)
            (_Regs()->CR1 & SPI_CR1_DFF) > 0
        #else
            (_Regs()->CR2 & SPI_CR2_DS) > DataSize8
        #endif
            ? (_DmaTx::PSize16Bits | _DmaTx::MSize16Bits)
            : (_DmaTx::PSize8Bits | _DmaTx::MSize8Bits);

        _DmaTx::SetTransferCallback(callback);
        _DmaTx::Transfer(_DmaTx::Mem2Periph | _DmaTx::MemIncrement | dataSize, data, &_Regs()->DR, size);
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::WriteAsyncNoIncrement(const void* data, uint16_t size, TransferCallback callback)
    {
        _DmaTx::ClearTransferComplete();
        _Regs::template SetBits<^^SPI_TypeDef::CR2, SPI_CR2_TXDMAEN>();
        auto dataSize = 
        #if defined(SPI_CR1_DFF)
            (_Regs()->CR1 & SPI_CR1_DFF) > 0
        #else
            (_Regs()->CR2 & SPI_CR2_DS) > DataSize8
        #endif
            ? (_DmaTx::PSize16Bits | _DmaTx::MSize16Bits)
            : (_DmaTx::PSize8Bits | _DmaTx::MSize8Bits);

        _DmaTx::SetTransferCallback(callback);
        _DmaTx::Transfer(_DmaTx::Mem2Periph | dataSize, data, &_Regs()->DR, size);
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    uint16_t Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::Read()
    {
        return Send(0xffff);
    }

    template<typename _Regs, typename _Clock, typename _MosiPins, typename _MisoPins, typename _ClockPins, typename _SsPins, typename _DmaTx, typename _DmaRx>
    void Spi<_Regs, _Clock, _MosiPins, _MisoPins, _ClockPins, _SsPins, _DmaTx, _DmaRx>::ReadAsync(void* receiveBuffer, size_t bufferSize, TransferCallback callback)
    {
        _DmaRx::ClearTransferComplete();
        _Regs()->CR2 |= (SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);
        auto dataSize = 
        #if defined(SPI_CR1_DFF)
            (_Regs()->CR1 & SPI_CR1_DFF) > 0
        #else
            (_Regs()->CR2 & SPI_CR2_DS) > DataSize8
        #endif
            ? (_DmaTx::PSize16Bits | _DmaTx::MSize16Bits)
            : (_DmaTx::PSize8Bits | _DmaTx::MSize8Bits);
        _DmaRx::SetTransferCallback(callback);
        _DmaRx::Transfer(_DmaRx::Periph2Mem | _DmaRx::MemIncrement | _DmaRx::Circular | dataSize, receiveBuffer, &_Regs()->DR, bufferSize);

        // Send dummmy value
        uint16_t dummy = 0xffff;
        _DmaTx::Transfer(_DmaTx::Mem2Periph | dataSize, &dummy, &_Regs()->DR, bufferSize);
    }
}
#endif //! ZHELE_PLATFORM_STM32_COMMON_IMPL_SPI_H