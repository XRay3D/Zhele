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

namespace Zhele
{
    namespace Private
    {
        using Dma1 = IoStruct<DMA1_BASE, DMA_TypeDef>;
        
        using Dma1Channel1 = IoStruct<DMA1_Channel1_BASE, DMA_Channel_TypeDef>;
        using Dma1Channel2 = IoStruct<DMA1_Channel2_BASE, DMA_Channel_TypeDef>;
        using Dma1Channel3 = IoStruct<DMA1_Channel3_BASE, DMA_Channel_TypeDef>;
        using Dma1Channel4 = IoStruct<DMA1_Channel4_BASE, DMA_Channel_TypeDef>;
        using Dma1Channel5 = IoStruct<DMA1_Channel5_BASE, DMA_Channel_TypeDef>;
    #if defined (DMA1_Channel6)
        using Dma1Channel6 = IoStruct<DMA1_Channel6_BASE, DMA_Channel_TypeDef>;
    #endif
    #if defined (DMA1_Channel7)
        using Dma1Channel7 = IoStruct<DMA1_Channel7_BASE, DMA_Channel_TypeDef>;	
    #endif
    }

    using Dma1 = DmaModule<Private::Dma1, Clock::Dma1Clock, 7>;
    using Dma1Channel1 = DmaChannel<Dma1, Private::Dma1Channel1, 1, DMA1_Channel1_IRQn>;
    using Dma1Channel2 = DmaChannel<Dma1, Private::Dma1Channel2, 2, DMA1_Ch2_3_DMA2_Ch1_2_IRQn>;
    using Dma1Channel3 = DmaChannel<Dma1, Private::Dma1Channel3, 3, DMA1_Ch2_3_DMA2_Ch1_2_IRQn>;
    using Dma1Channel4 = DmaChannel<Dma1, Private::Dma1Channel4, 4, DMA1_Channel4_5_6_7_IRQn>;
    using Dma1Channel5 = DmaChannel<Dma1, Private::Dma1Channel5, 5, DMA1_Channel4_5_6_7_IRQn>;
#if defined (DMA1_Channel6)
    using Dma1Channel6 = DmaChannel<Dma1, Private::Dma1Channel6, 6, DMA1_Channel4_5_6_7_IRQn>;
#endif
#if defined (DMA1_Channel7)
    using Dma1Channel7 = DmaChannel<Dma1, Private::Dma1Channel7, 7, DMA1_Channel4_5_6_7_IRQn>;
#endif

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