/**
 * @file
 * Implement DMA protocol for stm32f4 series
 * 
 * @author Alexey Zhelonkin
 * @date 2019
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_F4_DMA_H
#define ZHELE_PLATFORM_STM32_F4_DMA_H

#include <stm32f4xx.h>
#include "../common/dma.h"

#include <meta>

namespace Zhele
{
    namespace Private
    {
        using Dma1 = IoStruct<DMA1_BASE, DMA_TypeDef>;
        using Dma2 = IoStruct<DMA2_BASE, DMA_TypeDef>;
        
        using Dma1Stream0 = IoStruct<DMA1_Stream0_BASE, DMA_Stream_TypeDef>;
        using Dma1Stream1 = IoStruct<DMA1_Stream1_BASE, DMA_Stream_TypeDef>;
        using Dma1Stream2 = IoStruct<DMA1_Stream2_BASE, DMA_Stream_TypeDef>;
        using Dma1Stream3 = IoStruct<DMA1_Stream3_BASE, DMA_Stream_TypeDef>;
        using Dma1Stream4 = IoStruct<DMA1_Stream4_BASE, DMA_Stream_TypeDef>;
        using Dma1Stream5 = IoStruct<DMA1_Stream5_BASE, DMA_Stream_TypeDef>;
        using Dma1Stream6 = IoStruct<DMA1_Stream6_BASE, DMA_Stream_TypeDef>;
        using Dma1Stream7 = IoStruct<DMA1_Stream7_BASE, DMA_Stream_TypeDef>;
        
        using Dma2Stream0 = IoStruct<DMA2_Stream0_BASE, DMA_Stream_TypeDef>;
        using Dma2Stream1 = IoStruct<DMA2_Stream1_BASE, DMA_Stream_TypeDef>;
        using Dma2Stream2 = IoStruct<DMA2_Stream2_BASE, DMA_Stream_TypeDef>;
        using Dma2Stream3 = IoStruct<DMA2_Stream3_BASE, DMA_Stream_TypeDef>;
        using Dma2Stream4 = IoStruct<DMA2_Stream4_BASE, DMA_Stream_TypeDef>;
        using Dma2Stream5 = IoStruct<DMA2_Stream5_BASE, DMA_Stream_TypeDef>;
        using Dma2Stream6 = IoStruct<DMA2_Stream6_BASE, DMA_Stream_TypeDef>;
        using Dma2Stream7 = IoStruct<DMA2_Stream7_BASE, DMA_Stream_TypeDef>;

        template<typename _DmaStream, unsigned _DmaChannel>
        class DmaStreamChannel : public _DmaStream
        {
        public:
            static void Transfer(DmaBase::Mode mode, const void* buffer, volatile void* periph, uint32_t bufferSize)
            {
                _DmaStream::Transfer(mode, buffer, periph, bufferSize, _DmaChannel);
            }
        };
    }        

    // Alias for stream
    template<typename _Module, typename _StreamRegs, unsigned _Stream, IRQn_Type _IRQNumber>
    using DmaStream = DmaChannel<_Module, _StreamRegs, _Stream, _IRQNumber>;

    using Dma1 = DmaModule<Private::Dma1, Clock::Dma1Clock, 8>;
    using Dma1Stream0 = DmaStream<Dma1, Private::Dma1Stream0, 0, DMA1_Stream0_IRQn>;
    using Dma1Stream1 = DmaStream<Dma1, Private::Dma1Stream1, 1, DMA1_Stream1_IRQn>;
    using Dma1Stream2 = DmaStream<Dma1, Private::Dma1Stream2, 2, DMA1_Stream2_IRQn>;
    using Dma1Stream3 = DmaStream<Dma1, Private::Dma1Stream3, 3, DMA1_Stream3_IRQn>;
    using Dma1Stream4 = DmaStream<Dma1, Private::Dma1Stream4, 4, DMA1_Stream4_IRQn>;
    using Dma1Stream5 = DmaStream<Dma1, Private::Dma1Stream5, 5, DMA1_Stream5_IRQn>;
    using Dma1Stream6 = DmaStream<Dma1, Private::Dma1Stream6, 6, DMA1_Stream6_IRQn>;
    using Dma1Stream7 = DmaStream<Dma1, Private::Dma1Stream7, 7, DMA1_Stream7_IRQn>;
    

    using Dma2 = DmaModule<Private::Dma2, Clock::Dma2Clock, 8>;
    using Dma2Stream0 = DmaStream<Dma2, Private::Dma2Stream0, 0, DMA2_Stream0_IRQn>;
    using Dma2Stream1 = DmaStream<Dma2, Private::Dma2Stream1, 1, DMA2_Stream1_IRQn>;
    using Dma2Stream2 = DmaStream<Dma2, Private::Dma2Stream2, 2, DMA2_Stream2_IRQn>;
    using Dma2Stream3 = DmaStream<Dma2, Private::Dma2Stream3, 3, DMA2_Stream3_IRQn>;
    using Dma2Stream4 = DmaStream<Dma2, Private::Dma2Stream4, 4, DMA2_Stream4_IRQn>;
    using Dma2Stream5 = DmaStream<Dma2, Private::Dma2Stream5, 5, DMA2_Stream5_IRQn>;
    using Dma2Stream6 = DmaStream<Dma2, Private::Dma2Stream6, 6, DMA2_Stream6_IRQn>;
    using Dma2Stream7 = DmaStream<Dma2, Private::Dma2Stream7, 7, DMA2_Stream7_IRQn>;

    // Dma<d>Stream<s>Channel<c> = DmaStreamChannel<Dma<d>Stream<s>, c>, 2 x 8 x 8 aliases
    consteval
    {
        for (unsigned dma = 1; dma <= 2; ++dma)
            for (unsigned stream = 0; stream < 8; ++stream)
                for (unsigned channel = 0; channel < 8; ++channel)
                    std::meta::queue_injection(^^{
                        using \["Dma", dma, "Stream", stream, "Channel", channel] = Private::DmaStreamChannel<\["Dma", dma, "Stream", stream], \val(channel)>;
                    });
    }
}

#endif //! ZHELE_PLATFORM_STM32_F4_DMA_H