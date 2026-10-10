/**
 * @file
 * @brief Implements DAC
 * @date 2022
 * @author Aleksei Zhelonkin
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IMPL_DAC_H
#define ZHELE_PLATFORM_STM32_COMMON_IMPL_DAC_H

namespace Zhele::Private
{

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::Init()
    {
        _ClockCtrl::Enable();
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    template <typename Trigger>
    void DacBase<_Regs, _ClockCtrl, _Channel>::Init(Trigger trigger)
    {
        _ClockCtrl::Enable();
        _Regs()->CR |= (DAC_CR_TEN1
                    | (static_cast<uint8_t>(trigger) << DAC_CR_TSEL1_Pos)
                ) << (_Channel * ChannelOffset);
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::Enable()
    {
        _Regs()->CR |= DAC_CR_EN1 << (_Channel * ChannelOffset);
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::Disable()
    {
        _Regs()->CR &= ~(DAC_CR_EN1 << (_Channel * ChannelOffset));
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::EnableBuffer()
    {
    #if defined (DAC_MCR_MODE1)
        _Regs()->MCR &= ~(DAC_MCR_MODE1_Msk << (_Channel * ChannelOffset));
    #else
        _Regs()->CR &= ~(DAC_CR_BOFF1 << (_Channel * ChannelOffset));
    #endif
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::DisableBuffer()
    {
    #if defined (DAC_MCR_MODE1)
        _Regs()->MCR = (_Regs()->MCR & ~(DAC_MCR_MODE1_Msk << (_Channel * ChannelOffset)))
            | (0b010u << (_Channel * ChannelOffset));
    #else
        _Regs()->CR |= (DAC_CR_BOFF1 << (_Channel * ChannelOffset));
    #endif
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::EnableNoiseWaveGeneration(WaveAmplitude amplitude)
    {
        _Regs()->CR = (_Regs()->CR & ~((DAC_CR_WAVE1_Msk | DAC_CR_MAMP1_Msk) << (_Channel * ChannelOffset)))
            | (((static_cast<uint32_t>(WaveGeneration::Noise) << DAC_CR_WAVE1_Pos)
                | (static_cast<uint32_t>(amplitude) << DAC_CR_MAMP1_Pos)
                ) << (_Channel * ChannelOffset));
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::EnableTriangleWaveGeneration(WaveAmplitude amplitude)
    {
        _Regs()->CR = (_Regs()->CR & ~((DAC_CR_WAVE1_Msk | DAC_CR_MAMP1_Msk) << (_Channel * ChannelOffset)))
            | (((static_cast<uint32_t>(WaveGeneration::Triangle) << DAC_CR_WAVE1_Pos)
                | (static_cast<uint32_t>(amplitude) << DAC_CR_MAMP1_Pos)
                ) << (_Channel * ChannelOffset));
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::DisableWaveGeneration()
    {
        _Regs()->CR &= ~(DAC_CR_WAVE1_Msk << (_Channel * ChannelOffset));
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::WriteU8(uint8_t data)
    {
        if constexpr (_Channel == 0)
            _Regs()->DHR8R1 = data;
        else
            _Regs()->DHR8R2 = data;
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::Write(uint16_t data)
    {
        if constexpr (_Channel == 0)
            _Regs()->DHR12R1 = data;
        else
            _Regs()->DHR12R2 = data;
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::WriteLeftAligned(uint16_t data)
    {
        if constexpr (_Channel == 0)
            _Regs()->DHR12L1 = data;
        else
            _Regs()->DHR12L2 = data;
    }

    template <typename _Regs, typename _ClockCtrl, uint8_t _Channel>
    void DacBase<_Regs, _ClockCtrl, _Channel>::CauseSoftwareTrigger()
    {
        _Regs()->SWTRIGR = 1 << _Channel;
    }
}

#endif //! ZHELE_PLATFORM_STM32_COMMON_IMPL_DAC_H