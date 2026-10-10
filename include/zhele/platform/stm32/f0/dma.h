/**
 * @file
 * Implement DMA protocol for stm32f0 series
 * 
 * @author Alexey Zhelonkin
 * @date 2019
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_F0_DMA_H
#define ZHELE_PLATFORM_STM32_F0_DMA_H

#include <stm32f0xx.h>
#include "../common/dma.h"

#include <meta>

namespace Zhele
{
    namespace Private
    {
        using Dma1 = IoStruct<DMA1_BASE, DMA_TypeDef>;

        struct DmaChannelEntry { unsigned number; uintptr_t base; IRQn_Type irq; };
        inline constexpr DmaChannelEntry dma1Channels[] = {
            { 1, DMA1_Channel1_BASE, DMA1_Channel1_IRQn },
            { 2, DMA1_Channel2_BASE, DMA1_Ch2_3_DMA2_Ch1_2_IRQn },
            { 3, DMA1_Channel3_BASE, DMA1_Ch2_3_DMA2_Ch1_2_IRQn },
            { 4, DMA1_Channel4_BASE, DMA1_Channel4_5_6_7_IRQn },
            { 5, DMA1_Channel5_BASE, DMA1_Channel4_5_6_7_IRQn },
        #if defined (DMA1_Channel6)
            { 6, DMA1_Channel6_BASE, DMA1_Channel4_5_6_7_IRQn },
        #endif
        #if defined (DMA1_Channel7)
            { 7, DMA1_Channel7_BASE, DMA1_Channel4_5_6_7_IRQn },
        #endif
        };
    }

    using Dma1 = DmaModule<Private::Dma1, Clock::Dma1Clock, 7>;

    // Dma1ChannelN = DmaChannel<Dma1, Private::Dma1ChannelN, N, irq> for every channel the device has
    consteval
    {
        for (const Private::DmaChannelEntry& channel : Private::dma1Channels)
            std::meta::queue_injection(^^{
                namespace Private { using \["Dma1Channel", channel.number] = IoStruct<\val(channel.base), DMA_Channel_TypeDef>; }
                using \["Dma1Channel", channel.number] = DmaChannel<Dma1, Private::\["Dma1Channel", channel.number], \val(channel.number), \val(channel.irq)>;
            });
    }

#if defined (RCC_AHBENR_DMA2EN)
    namespace Private
    {
        using Dma2 = IoStruct<DMA2_BASE, DMA_TypeDef>;
        using Dma2Channel1 = IoStruct<DMA2_Channel1_BASE, DMA_Channel_TypeDef>;
        using Dma2Channel2 = IoStruct<DMA2_Channel2_BASE, DMA_Channel_TypeDef>;
        using Dma2Channel3 = IoStruct<DMA2_Channel3_BASE, DMA_Channel_TypeDef>;
        using Dma2Channel4 = IoStruct<DMA2_Channel4_BASE, DMA_Channel_TypeDef>;
        using Dma2Channel5 = IoStruct<DMA2_Channel5_BASE, DMA_Channel_TypeDef>;	
    }
    using Dma2 = DmaModule<Private::Dma2, Clock::Dma2Clock, 5>;
    using Dma2Channel1 = DmaChannel<Dma2, Private::Dma2Channel1, 1, DMA1_Ch2_3_DMA2_Ch1_2_IRQn>;
    using Dma2Channel2 = DmaChannel<Dma2, Private::Dma2Channel2, 2, DMA1_Ch2_3_DMA2_Ch1_2_IRQn>;
    using Dma2Channel3 = DmaChannel<Dma2, Private::Dma2Channel3, 3, DMA1_Ch4_7_DMA2_Ch3_5_IRQn>;
    using Dma2Channel4 = DmaChannel<Dma2, Private::Dma2Channel4, 4, DMA1_Ch4_7_DMA2_Ch3_5_IRQn>;
    using Dma2Channel5 = DmaChannel<Dma2, Private::Dma2Channel5, 5, DMA1_Ch4_7_DMA2_Ch3_5_IRQn>;
#endif
} // namespace Zhele

#endif //! ZHELE_PLATFORM_STM32_F0_DMA_H