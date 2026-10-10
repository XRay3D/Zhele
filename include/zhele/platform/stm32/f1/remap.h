 /**
 * @file
 * Contains class remap GPIO on stm32f1
 * 
 * STM32F1 mcu has specific scheme for remap gpio pins
 * There is no AFR register in GPIO_TypeDef struct for each port, but
 * two registers (MAPR/MAPR2). So, STM32F1 does not assume remap pins and
 * assume remap pins set for each module instead
 * 
 * For example, you can remap Timer2 (all channels at the same time)
 * 
 * For compability I tried enable "SelectPins" method for periphery (for timer, for example),
 * but there is some limitations. For example, Timer realization allows select pins for each channel
 * via SelectPins. Inside this method for F1 I edit MAPR register AND REMAP ALSO OTHER CHANNELS!
 * So if you select Pa6 for Tim3Ch1 and after that select Pb5 for Tim3Ch2 then you also select Pb4 for Tim3Ch1
 * You must remember this feature and use Remap class for custom remap
 * 
 * @author Aleksei Zhelonkin
 * @date 2020
 * @licence MIT
 */

#ifndef ZHELE_PLATFORM_STM32_F1_REMAP_H
#define ZHELE_PLATFORM_STM32_F1_REMAP_H

#include <stm32f1xx.h>
#include "../common/ioreg.h"

#include "clock.h"

namespace Zhele::IO
{
    namespace Private
    {
        template<typename _ClockRegs>
        class PeriphRemapBitField{};


        // Timers Remap
        using Timer1RemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_TIM1_REMAP_Msk>;
        template<> class PeriphRemapBitField<Zhele::Clock::Tim1Clock> { public: using BitField = Timer1RemapBitField; };

        using Timer2RemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_TIM2_REMAP_Msk>;
        template<> class PeriphRemapBitField<Zhele::Clock::Tim2Clock> { public: using BitField = Timer2RemapBitField; };

        using Timer3RemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_TIM3_REMAP_Msk>;
        template<> class PeriphRemapBitField<Zhele::Clock::Tim3Clock> { public: using BitField = Timer3RemapBitField; };
    #if defined (TIM4)
        using Timer4RemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_TIM4_REMAP_Msk>;
        template<> class PeriphRemapBitField<Zhele::Clock::Tim4Clock> { public: using BitField = Timer4RemapBitField; };
    #endif

        // Usarts remap
        using Usart1RemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_USART1_REMAP_Msk>;
        template<> class PeriphRemapBitField<Zhele::Clock::Usart1Clock> { public: using BitField = Usart1RemapBitField; };

        using Usart2RemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_USART2_REMAP_Msk>;
        template<> class PeriphRemapBitField<Zhele::Clock::Usart2Clock> { public: using BitField = Usart2RemapBitField; };

        #if defined (USART3)
            using Usart3RemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_USART3_REMAP_Msk>;
            template<> class PeriphRemapBitField<Zhele::Clock::Usart3Clock> { public: using BitField = Usart3RemapBitField; };
        #endif

        // SPI remap
        using Spi1RemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_SPI1_REMAP_Msk>;
        template<> class PeriphRemapBitField<Zhele::Clock::Spi1Clock> { public: using BitField = Spi1RemapBitField; };
        // High-density F103 have SPI3 but no SPI3 remap bit (connectivity line only)
        #if defined(AFIO_MAPR_SPI3_REMAP)
            using Spi3RemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_SPI3_REMAP_Msk>;
            template<> class PeriphRemapBitField<Zhele::Clock::Spi3Clock> { public: using BitField = Spi3RemapBitField; };
        #endif

        // I2C remap
        using I2c1RemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_I2C1_REMAP_Msk>;
        template<> class PeriphRemapBitField<Zhele::Clock::I2c1Clock> { public: using BitField = I2c1RemapBitField; };

        // SWJ remap
        using SwjRemapBitField = MaskedBitField<PeriphReg<AFIO_BASE, ^^AFIO_TypeDef::MAPR>, AFIO_MAPR_SWJ_CFG_Msk>;

        template<typename Clock>
        using PeriphRemap = typename Private::PeriphRemapBitField<Clock>::BitField;
    }

    using Timer1Remap = Private::PeriphRemap<Zhele::Clock::Tim1Clock>;
    using Timer2Remap = Private::PeriphRemap<Zhele::Clock::Tim2Clock>;
    using Timer3Remap = Private::PeriphRemap<Zhele::Clock::Tim3Clock>;
#if defined (TIM4)
    using Timer4Remap = Private::PeriphRemap<Zhele::Clock::Tim4Clock>;
#endif
    using Usart1Remap = Private::PeriphRemap<Zhele::Clock::Usart1Clock>;
    using Usart2Remap = Private::PeriphRemap<Zhele::Clock::Usart2Clock>;

#if defined (USART3)
    using Usart3Remap = Private::PeriphRemap<Zhele::Clock::Usart3Clock>;
#endif
    using Spi1Remap = Private::PeriphRemap<Zhele::Clock::Spi1Clock>;
#if defined(AFIO_MAPR_SPI3_REMAP)
    using Spi3Remap = Private::PeriphRemap<Zhele::Clock::Spi3Clock>;
#endif

    using I2c1Remap = Private::PeriphRemap<Zhele::Clock::I2c1Clock>;

    using SwjRemap = Private::SwjRemapBitField;
} // namespace Zhele::IO

#endif // !ZHELE_PLATFORM_STM32_F1_REMAP_H