/**
 * @file
 * Implement DMA protocol for stm32f1 series
 * 
 * @author Alexey Zhelonkin
 * @date 2019
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_F1_DMA_H
#define ZHELE_PLATFORM_STM32_F1_DMA_H

#include <stm32f1xx.h>
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
        using Dma1Channel6 = IoStruct<DMA1_Channel6_BASE, DMA_Channel_TypeDef>;
        using Dma1Channel7 = IoStruct<DMA1_Channel7_BASE, DMA_Channel_TypeDef>;	
    }

    using Dma1 = DmaModule<Private::Dma1, Clock::Dma1Clock, 7>;
    using Dma1Channel1 = DmaChannel<Dma1, Private::Dma1Channel1, 1, DMA1_Channel1_IRQn>;
    using Dma1Channel2 = DmaChannel<Dma1, Private::Dma1Channel2, 2, DMA1_Channel2_IRQn>;
    using Dma1Channel3 = DmaChannel<Dma1, Private::Dma1Channel3, 3, DMA1_Channel3_IRQn>;
    using Dma1Channel4 = DmaChannel<Dma1, Private::Dma1Channel4, 4, DMA1_Channel4_IRQn>;
    using Dma1Channel5 = DmaChannel<Dma1, Private::Dma1Channel5, 5, DMA1_Channel5_IRQn>;
    using Dma1Channel6 = DmaChannel<Dma1, Private::Dma1Channel6, 6, DMA1_Channel6_IRQn>;
    using Dma1Channel7 = DmaChannel<Dma1, Private::Dma1Channel7, 7, DMA1_Channel7_IRQn>;

#if defined (RCC_AHBENR_DMA2EN)
// On devices where DMA2 channels 4/5 share one vector, CMSIS provides the
// compatibility define (DMA2_Channel4_IRQn = DMA2_Channel4_5_IRQn) for
// channel 4 only; provide the missing one for channel 5.
#ifdef DMA2_Channel4_IRQn
    #define DMA2_Channel5_IRQn DMA2_Channel4_5_IRQn
#endif
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
    using Dma2Channel1 = DmaChannel<Dma2, Private::Dma2Channel1, 1, DMA2_Channel1_IRQn>;
    using Dma2Channel2 = DmaChannel<Dma2, Private::Dma2Channel2, 2, DMA2_Channel2_IRQn>;
    using Dma2Channel3 = DmaChannel<Dma2, Private::Dma2Channel3, 3, DMA2_Channel3_IRQn>;
    using Dma2Channel4 = DmaChannel<Dma2, Private::Dma2Channel4, 4, DMA2_Channel4_IRQn>;
    using Dma2Channel5 = DmaChannel<Dma2, Private::Dma2Channel5, 5, DMA2_Channel5_IRQn>;
#endif
} // namespace Zhele

#endif //! ZHELE_PLATFORM_STM32_F1_DMA_H