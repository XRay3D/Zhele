/**
 * @file
 * DMAMUX methods implementation
 * 
 * @author Alexey Zhelonkin
 * @date 2024
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IMPL_DMAMUX_H
#define ZHELE_PLATFORM_STM32_COMMON_IMPL_DMAMUX_H

#include <cstdint>

namespace Zhele
{

    template<typename _BaseRegs, std::convertible_to<uint32_t> _SyncInput, std::convertible_to<uint32_t> _RequestInput>
    template<unsigned _ChannelNumber>
    inline void DmaMux<_BaseRegs, _SyncInput, _RequestInput>::Channel<_ChannelNumber>::SetSyncInput(_SyncInput input)
    {
        SyncInputBitfield::Set(static_cast<uint32_t>(input));
    }

    template<typename _BaseRegs, std::convertible_to<uint32_t> _SyncInput, std::convertible_to<uint32_t> _RequestInput>
    template<unsigned _ChannelNumber>
    inline _SyncInput DmaMux<_BaseRegs, _SyncInput, _RequestInput>::Channel<_ChannelNumber>::GetSyncInput()
    {
        return static_cast<_SyncInput>(SyncInputBitfield::Get());
    }

    template<typename _BaseRegs, std::convertible_to<uint32_t> _SyncInput, std::convertible_to<uint32_t> _RequestInput>
    template<unsigned _ChannelNumber>
    inline void DmaMux<_BaseRegs, _SyncInput, _RequestInput>::Channel<_ChannelNumber>::SetSyncPolarity(SyncPolarity polarity)
    {
        SyncPolarityBitfield::Set(static_cast<uint32_t>(polarity));
    }

    template<typename _BaseRegs, std::convertible_to<uint32_t> _SyncInput, std::convertible_to<uint32_t> _RequestInput>
    template<unsigned _ChannelNumber>
    inline DmaMux<_BaseRegs, _SyncInput, _RequestInput>::Channel<_ChannelNumber>::SyncPolarity DmaMux<_BaseRegs, _SyncInput, _RequestInput>::Channel<_ChannelNumber>::GetSyncPolarity()
    {
        return static_cast<SyncPolarity>(SyncPolarityBitfield::Get());
    }

    template<typename _BaseRegs, std::convertible_to<uint32_t> _SyncInput, std::convertible_to<uint32_t> _RequestInput>
    template<unsigned _ChannelNumber>
    inline void DmaMux<_BaseRegs, _SyncInput, _RequestInput>::Channel<_ChannelNumber>::EnableSynchronization()
    {
        ChannelRegs::Get()[_ChannelNumber].CCR |= DMAMUX_CxCR_SE;
    }

    template<typename _BaseRegs, std::convertible_to<uint32_t> _SyncInput, std::convertible_to<uint32_t> _RequestInput>
    template<unsigned _ChannelNumber>
    inline void DmaMux<_BaseRegs, _SyncInput, _RequestInput>::Channel<_ChannelNumber>::DisableSynchronization()
    {
        ChannelRegs::Get()[_ChannelNumber].CCR &= ~DMAMUX_CxCR_SE;
    }

    template<typename _BaseRegs, std::convertible_to<uint32_t> _SyncInput, std::convertible_to<uint32_t> _RequestInput>
    template<unsigned _ChannelNumber>
    inline void DmaMux<_BaseRegs, _SyncInput, _RequestInput>::Channel<_ChannelNumber>::SelectRequestInput(_RequestInput input)
    {
        RequestPolarityBitfield::Set(static_cast<uint32_t>(input));
    }

    template<typename _BaseRegs, std::convertible_to<uint32_t> _SyncInput, std::convertible_to<uint32_t> _RequestInput>
    template<unsigned _ChannelNumber>
    inline _RequestInput DmaMux<_BaseRegs, _SyncInput, _RequestInput>::Channel<_ChannelNumber>::GetRequestInput()
    {
        return static_cast<_RequestInput>(RequestPolarityBitfield::Get());
    }
}
#endif  //! ZHELE_PLATFORM_STM32_COMMON_IMPL_DMAMUX_H