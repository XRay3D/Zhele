/**
 * @file
 * Implement DMA protocol for stm32f4 series
 * 
 * @author Alexey Zhelonkin
 * @date 2019
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_L4_DMA_H
#define ZHELE_PLATFORM_STM32_L4_DMA_H

#include <stm32l4xx.h>
#include "../common/dma.h"

namespace Zhele
{
    namespace Private
    {
        using Dma1 = IoStruct<DMA1_BASE, DMA_TypeDef>;
        using Dma2 = IoStruct<DMA2_BASE, DMA_TypeDef>;
        
        using Dma1Stream1 = IoStruct<DMA1_Channel1_BASE, DMA_Channel_TypeDef>;
        using Dma1Stream2 = IoStruct<DMA1_Channel2_BASE, DMA_Channel_TypeDef>;
        using Dma1Stream3 = IoStruct<DMA1_Channel3_BASE, DMA_Channel_TypeDef>;
        using Dma1Stream4 = IoStruct<DMA1_Channel4_BASE, DMA_Channel_TypeDef>;
        using Dma1Stream5 = IoStruct<DMA1_Channel5_BASE, DMA_Channel_TypeDef>;
        using Dma1Stream6 = IoStruct<DMA1_Channel6_BASE, DMA_Channel_TypeDef>;
        using Dma1Stream7 = IoStruct<DMA1_Channel7_BASE, DMA_Channel_TypeDef>;
        
        using Dma2Stream1 = IoStruct<DMA2_Channel1_BASE, DMA_Channel_TypeDef>;
        using Dma2Stream2 = IoStruct<DMA2_Channel2_BASE, DMA_Channel_TypeDef>;
        using Dma2Stream3 = IoStruct<DMA2_Channel3_BASE, DMA_Channel_TypeDef>;
        using Dma2Stream4 = IoStruct<DMA2_Channel4_BASE, DMA_Channel_TypeDef>;
        using Dma2Stream5 = IoStruct<DMA2_Channel5_BASE, DMA_Channel_TypeDef>;
        using Dma2Stream6 = IoStruct<DMA2_Channel6_BASE, DMA_Channel_TypeDef>;
        using Dma2Stream7 = IoStruct<DMA2_Channel7_BASE, DMA_Channel_TypeDef>;

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
    using Dma1Stream1 = DmaStream<Dma1, Private::Dma1Stream1, 1, DMA1_Channel1_IRQn>;
    using Dma1Stream2 = DmaStream<Dma1, Private::Dma1Stream2, 2, DMA1_Channel2_IRQn>;
    using Dma1Stream3 = DmaStream<Dma1, Private::Dma1Stream3, 3, DMA1_Channel3_IRQn>;
    using Dma1Stream4 = DmaStream<Dma1, Private::Dma1Stream4, 4, DMA1_Channel4_IRQn>;
    using Dma1Stream5 = DmaStream<Dma1, Private::Dma1Stream5, 5, DMA1_Channel5_IRQn>;
    using Dma1Stream6 = DmaStream<Dma1, Private::Dma1Stream6, 6, DMA1_Channel6_IRQn>;
    using Dma1Stream7 = DmaStream<Dma1, Private::Dma1Stream7, 7, DMA1_Channel7_IRQn>;
    

    using Dma2 = DmaModule<Private::Dma2, Clock::Dma2Clock, 8>;
    using Dma2Stream1 = DmaStream<Dma2, Private::Dma2Stream1, 1, DMA2_Channel1_IRQn>;
    using Dma2Stream2 = DmaStream<Dma2, Private::Dma2Stream2, 2, DMA2_Channel2_IRQn>;
    using Dma2Stream3 = DmaStream<Dma2, Private::Dma2Stream3, 3, DMA2_Channel3_IRQn>;
    using Dma2Stream4 = DmaStream<Dma2, Private::Dma2Stream4, 4, DMA2_Channel4_IRQn>;
    using Dma2Stream5 = DmaStream<Dma2, Private::Dma2Stream5, 5, DMA2_Channel5_IRQn>;
    using Dma2Stream6 = DmaStream<Dma2, Private::Dma2Stream6, 6, DMA2_Channel6_IRQn>;
    using Dma2Stream7 = DmaStream<Dma2, Private::Dma2Stream7, 7, DMA2_Channel7_IRQn>;

    using Dma1Stream1Channel0 = Private::DmaStreamChannel<Dma1Stream1, 0>;
    using Dma1Stream1Channel1 = Private::DmaStreamChannel<Dma1Stream1, 1>;
    using Dma1Stream1Channel2 = Private::DmaStreamChannel<Dma1Stream1, 2>;
    using Dma1Stream1Channel3 = Private::DmaStreamChannel<Dma1Stream1, 3>;
    using Dma1Stream1Channel4 = Private::DmaStreamChannel<Dma1Stream1, 4>;
    using Dma1Stream1Channel5 = Private::DmaStreamChannel<Dma1Stream1, 5>;
    using Dma1Stream1Channel6 = Private::DmaStreamChannel<Dma1Stream1, 6>;
    using Dma1Stream1Channel7 = Private::DmaStreamChannel<Dma1Stream1, 7>;
    using Dma1Stream2Channel0 = Private::DmaStreamChannel<Dma1Stream2, 0>;
    using Dma1Stream2Channel1 = Private::DmaStreamChannel<Dma1Stream2, 1>;
    using Dma1Stream2Channel2 = Private::DmaStreamChannel<Dma1Stream2, 2>;
    using Dma1Stream2Channel3 = Private::DmaStreamChannel<Dma1Stream2, 3>;
    using Dma1Stream2Channel4 = Private::DmaStreamChannel<Dma1Stream2, 4>;
    using Dma1Stream2Channel5 = Private::DmaStreamChannel<Dma1Stream2, 5>;
    using Dma1Stream2Channel6 = Private::DmaStreamChannel<Dma1Stream2, 6>;
    using Dma1Stream2Channel7 = Private::DmaStreamChannel<Dma1Stream2, 7>;
    using Dma1Stream3Channel0 = Private::DmaStreamChannel<Dma1Stream3, 0>;
    using Dma1Stream3Channel1 = Private::DmaStreamChannel<Dma1Stream3, 1>;
    using Dma1Stream3Channel2 = Private::DmaStreamChannel<Dma1Stream3, 2>;
    using Dma1Stream3Channel3 = Private::DmaStreamChannel<Dma1Stream3, 3>;
    using Dma1Stream3Channel4 = Private::DmaStreamChannel<Dma1Stream3, 4>;
    using Dma1Stream3Channel5 = Private::DmaStreamChannel<Dma1Stream3, 5>;
    using Dma1Stream3Channel6 = Private::DmaStreamChannel<Dma1Stream3, 6>;
    using Dma1Stream3Channel7 = Private::DmaStreamChannel<Dma1Stream3, 7>;
    using Dma1Stream4Channel0 = Private::DmaStreamChannel<Dma1Stream4, 0>;
    using Dma1Stream4Channel1 = Private::DmaStreamChannel<Dma1Stream4, 1>;
    using Dma1Stream4Channel2 = Private::DmaStreamChannel<Dma1Stream4, 2>;
    using Dma1Stream4Channel3 = Private::DmaStreamChannel<Dma1Stream4, 3>;
    using Dma1Stream4Channel4 = Private::DmaStreamChannel<Dma1Stream4, 4>;
    using Dma1Stream4Channel5 = Private::DmaStreamChannel<Dma1Stream4, 5>;
    using Dma1Stream4Channel6 = Private::DmaStreamChannel<Dma1Stream4, 6>;
    using Dma1Stream4Channel7 = Private::DmaStreamChannel<Dma1Stream4, 7>;
    using Dma1Stream5Channel0 = Private::DmaStreamChannel<Dma1Stream5, 0>;
    using Dma1Stream5Channel1 = Private::DmaStreamChannel<Dma1Stream5, 1>;
    using Dma1Stream5Channel2 = Private::DmaStreamChannel<Dma1Stream5, 2>;
    using Dma1Stream5Channel3 = Private::DmaStreamChannel<Dma1Stream5, 3>;
    using Dma1Stream5Channel4 = Private::DmaStreamChannel<Dma1Stream5, 4>;
    using Dma1Stream5Channel5 = Private::DmaStreamChannel<Dma1Stream5, 5>;
    using Dma1Stream5Channel6 = Private::DmaStreamChannel<Dma1Stream5, 6>;
    using Dma1Stream5Channel7 = Private::DmaStreamChannel<Dma1Stream5, 7>;
    using Dma1Stream6Channel0 = Private::DmaStreamChannel<Dma1Stream6, 0>;
    using Dma1Stream6Channel1 = Private::DmaStreamChannel<Dma1Stream6, 1>;
    using Dma1Stream6Channel2 = Private::DmaStreamChannel<Dma1Stream6, 2>;
    using Dma1Stream6Channel3 = Private::DmaStreamChannel<Dma1Stream6, 3>;
    using Dma1Stream6Channel4 = Private::DmaStreamChannel<Dma1Stream6, 4>;
    using Dma1Stream6Channel5 = Private::DmaStreamChannel<Dma1Stream6, 5>;
    using Dma1Stream6Channel6 = Private::DmaStreamChannel<Dma1Stream6, 6>;
    using Dma1Stream6Channel7 = Private::DmaStreamChannel<Dma1Stream6, 7>;
    using Dma1Stream7Channel0 = Private::DmaStreamChannel<Dma1Stream7, 0>;
    using Dma1Stream7Channel1 = Private::DmaStreamChannel<Dma1Stream7, 1>;
    using Dma1Stream7Channel2 = Private::DmaStreamChannel<Dma1Stream7, 2>;
    using Dma1Stream7Channel3 = Private::DmaStreamChannel<Dma1Stream7, 3>;
    using Dma1Stream7Channel4 = Private::DmaStreamChannel<Dma1Stream7, 4>;
    using Dma1Stream7Channel5 = Private::DmaStreamChannel<Dma1Stream7, 5>;
    using Dma1Stream7Channel6 = Private::DmaStreamChannel<Dma1Stream7, 6>;
    using Dma1Stream7Channel7 = Private::DmaStreamChannel<Dma1Stream7, 7>;

    using Dma2Stream1Channel0 = Private::DmaStreamChannel<Dma2Stream1, 0>;
    using Dma2Stream1Channel1 = Private::DmaStreamChannel<Dma2Stream1, 1>;
    using Dma2Stream1Channel2 = Private::DmaStreamChannel<Dma2Stream1, 2>;
    using Dma2Stream1Channel3 = Private::DmaStreamChannel<Dma2Stream1, 3>;
    using Dma2Stream1Channel4 = Private::DmaStreamChannel<Dma2Stream1, 4>;
    using Dma2Stream1Channel5 = Private::DmaStreamChannel<Dma2Stream1, 5>;
    using Dma2Stream1Channel6 = Private::DmaStreamChannel<Dma2Stream1, 6>;
    using Dma2Stream1Channel7 = Private::DmaStreamChannel<Dma2Stream1, 7>;
    using Dma2Stream2Channel0 = Private::DmaStreamChannel<Dma2Stream2, 0>;
    using Dma2Stream2Channel1 = Private::DmaStreamChannel<Dma2Stream2, 1>;
    using Dma2Stream2Channel2 = Private::DmaStreamChannel<Dma2Stream2, 2>;
    using Dma2Stream2Channel3 = Private::DmaStreamChannel<Dma2Stream2, 3>;
    using Dma2Stream2Channel4 = Private::DmaStreamChannel<Dma2Stream2, 4>;
    using Dma2Stream2Channel5 = Private::DmaStreamChannel<Dma2Stream2, 5>;
    using Dma2Stream2Channel6 = Private::DmaStreamChannel<Dma2Stream2, 6>;
    using Dma2Stream2Channel7 = Private::DmaStreamChannel<Dma2Stream2, 7>;
    using Dma2Stream3Channel0 = Private::DmaStreamChannel<Dma2Stream3, 0>;
    using Dma2Stream3Channel1 = Private::DmaStreamChannel<Dma2Stream3, 1>;
    using Dma2Stream3Channel2 = Private::DmaStreamChannel<Dma2Stream3, 2>;
    using Dma2Stream3Channel3 = Private::DmaStreamChannel<Dma2Stream3, 3>;
    using Dma2Stream3Channel4 = Private::DmaStreamChannel<Dma2Stream3, 4>;
    using Dma2Stream3Channel5 = Private::DmaStreamChannel<Dma2Stream3, 5>;
    using Dma2Stream3Channel6 = Private::DmaStreamChannel<Dma2Stream3, 6>;
    using Dma2Stream3Channel7 = Private::DmaStreamChannel<Dma2Stream3, 7>;
    using Dma2Stream4Channel0 = Private::DmaStreamChannel<Dma2Stream4, 0>;
    using Dma2Stream4Channel1 = Private::DmaStreamChannel<Dma2Stream4, 1>;
    using Dma2Stream4Channel2 = Private::DmaStreamChannel<Dma2Stream4, 2>;
    using Dma2Stream4Channel3 = Private::DmaStreamChannel<Dma2Stream4, 3>;
    using Dma2Stream4Channel4 = Private::DmaStreamChannel<Dma2Stream4, 4>;
    using Dma2Stream4Channel5 = Private::DmaStreamChannel<Dma2Stream4, 5>;
    using Dma2Stream4Channel6 = Private::DmaStreamChannel<Dma2Stream4, 6>;
    using Dma2Stream4Channel7 = Private::DmaStreamChannel<Dma2Stream4, 7>;
    using Dma2Stream5Channel0 = Private::DmaStreamChannel<Dma2Stream5, 0>;
    using Dma2Stream5Channel1 = Private::DmaStreamChannel<Dma2Stream5, 1>;
    using Dma2Stream5Channel2 = Private::DmaStreamChannel<Dma2Stream5, 2>;
    using Dma2Stream5Channel3 = Private::DmaStreamChannel<Dma2Stream5, 3>;
    using Dma2Stream5Channel4 = Private::DmaStreamChannel<Dma2Stream5, 4>;
    using Dma2Stream5Channel5 = Private::DmaStreamChannel<Dma2Stream5, 5>;
    using Dma2Stream5Channel6 = Private::DmaStreamChannel<Dma2Stream5, 6>;
    using Dma2Stream5Channel7 = Private::DmaStreamChannel<Dma2Stream5, 7>;
    using Dma2Stream6Channel0 = Private::DmaStreamChannel<Dma2Stream6, 0>;
    using Dma2Stream6Channel1 = Private::DmaStreamChannel<Dma2Stream6, 1>;
    using Dma2Stream6Channel2 = Private::DmaStreamChannel<Dma2Stream6, 2>;
    using Dma2Stream6Channel3 = Private::DmaStreamChannel<Dma2Stream6, 3>;
    using Dma2Stream6Channel4 = Private::DmaStreamChannel<Dma2Stream6, 4>;
    using Dma2Stream6Channel5 = Private::DmaStreamChannel<Dma2Stream6, 5>;
    using Dma2Stream6Channel6 = Private::DmaStreamChannel<Dma2Stream6, 6>;
    using Dma2Stream6Channel7 = Private::DmaStreamChannel<Dma2Stream6, 7>;
    using Dma2Stream7Channel0 = Private::DmaStreamChannel<Dma2Stream7, 0>;
    using Dma2Stream7Channel1 = Private::DmaStreamChannel<Dma2Stream7, 1>;
    using Dma2Stream7Channel2 = Private::DmaStreamChannel<Dma2Stream7, 2>;
    using Dma2Stream7Channel3 = Private::DmaStreamChannel<Dma2Stream7, 3>;
    using Dma2Stream7Channel4 = Private::DmaStreamChannel<Dma2Stream7, 4>;
    using Dma2Stream7Channel5 = Private::DmaStreamChannel<Dma2Stream7, 5>;
    using Dma2Stream7Channel6 = Private::DmaStreamChannel<Dma2Stream7, 6>;
    using Dma2Stream7Channel7 = Private::DmaStreamChannel<Dma2Stream7, 7>;
}

#endif //! ZHELE_PLATFORM_STM32_L4_DMA_H