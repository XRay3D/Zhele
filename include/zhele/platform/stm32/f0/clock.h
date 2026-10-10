/**
 * @file
 * Implemets clocks for stm32f0 series
 * 
 * @author Alexey Zhelonkin
 * @date 2020
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_F0_CLOCK_H
#define ZHELE_PLATFORM_STM32_F0_CLOCK_H

#include <stm32f0xx.h>
#include "../common/clock.h"

namespace Zhele::Clock
{

    inline unsigned PllClock::GetDivider()
    {
        if(GetClockSource() == Internal)
            return 2;

        return (RCC->CFGR & RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV2) > 0 
            ? 2
            : 1;
    }

    template<unsigned divider>
    inline void PllClock::SetDivider()
    {
        static_assert(1 <= divider && divider <= 2, "Divider can be 1 or 2 only!");
            
        RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLXTPRE) | (divider == 2
            ? RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV2
            : RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1);
    
    }

    inline unsigned PllClock::GetMultipler()
    {
        return ((RCC->CFGR & RCC_CFGR_PLLMUL) >> RCC_CFGR_PLLMUL_Pos) + 2;
    }

    template<unsigned multiplier>
    inline void PllClock::SetMultiplier()
    {
        static_assert(2 <= multiplier && multiplier <= 16, "Invalud multiplier value!");
        RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLMUL) | ((multiplier - 2) << RCC_CFGR_PLLMUL_Pos);
    }    

    template<PllClock::ClockSource clockSource>
    inline void PllClock::SelectClockSource()
    {
        RCC->CFGR = clockSource == External
            ? RCC->CFGR  | RCC_CFGR_PLLSRC
            : RCC->CFGR  & ~RCC_CFGR_PLLSRC;
    }

    inline PllClock::ClockSource PllClock::GetClockSource()
    {
        return RCC->CFGR & RCC_CFGR_PLLSRC
            ? ClockSource::External
            : ClockSource::Internal;
    }

    inline unsigned PllClock::GetSystemOutputDivider()
    {
        return 1;
    }

    #if defined(RCC_HSI48_SUPPORT)
        /**
         * @brief Implements Hsi48 clock source
         */
        using RccCr2Reg = PeriphReg<RCC_BASE, ^^RCC_TypeDef::CR2>;
        class Hsi48Clock : public ClockBase<RccCr2Reg>
        {
        public:
            static ClockFrequenceT SrcClockFreq()
            {
                return 48000000;
            }
            static unsigned GetDivider() { return 1; }
            static unsigned GetMultipler() { return 1; }
            static ClockFrequenceT ClockFreq()
            {
                return SrcClockFreq();
            }
            static bool Enable()
            {
                return ClockBase::EnableClockSource(RCC_CR2_HSI48ON, RCC_CR2_HSI48RDY);
            }
            static bool Disable()
            {
                return ClockBase::DisableClockSource(RCC_CR2_HSI48ON, RCC_CR2_HSI48RDY);
            }
        };
    #endif

    using AhbPrescalerBitField = MaskedBitField<PeriphReg<RCC_BASE, ^^RCC_TypeDef::CFGR>, RCC_CFGR_HPRE_Msk>;

    class AhbClock : public BusClock<SysClock, AhbPrescalerBitField>
    {
        using Base = BusClock<SysClock, AhbPrescalerBitField>;
    public:
        // AHB prescaler values
        enum Prescaler
        {
            Div1 = RCC_CFGR_HPRE_DIV1 >> AhbPrescalerBitField::Offset, ///< No divide (prescaler = 1)
            Div2 = RCC_CFGR_HPRE_DIV2 >> AhbPrescalerBitField::Offset, ///< Prescaler = 2
            Div4 = RCC_CFGR_HPRE_DIV4 >> AhbPrescalerBitField::Offset, ///< Prescaler = 4
            Div8 = RCC_CFGR_HPRE_DIV8 >> AhbPrescalerBitField::Offset, ///< Prescaler = 8
            Div16 = RCC_CFGR_HPRE_DIV16 >> AhbPrescalerBitField::Offset, ///< Prescaler = 16
            Div64 = RCC_CFGR_HPRE_DIV64 >> AhbPrescalerBitField::Offset, ///< Prescaler = 64
            Div128 = RCC_CFGR_HPRE_DIV128 >> AhbPrescalerBitField::Offset, ///< Prescaler = 128
            Div256 = RCC_CFGR_HPRE_DIV256 >> AhbPrescalerBitField::Offset, ///< Prescaler = 256
            Div512 = RCC_CFGR_HPRE_DIV512 >> AhbPrescalerBitField::Offset ///< Prescaler = 512
        };

        static ClockFrequenceT ClockFreq()
        {
            ClockFrequenceT clock = SysClock::ClockFreq();
            uint8_t shiftBits = AhbPrescalerShift(AhbPrescalerBitField::Get());
            clock >>= shiftBits;
            return clock;
        }

        template<Prescaler prescaler>
        static void SetPrescaler()
        {
            Base::SetPrescaler(prescaler);
        }
    };

    using ApbPrescalerBitField = MaskedBitField<PeriphReg<RCC_BASE, ^^RCC_TypeDef::CFGR>, RCC_CFGR_PPRE_Msk>;

    /**
     * @brief Implements APB clock
     */
    class ApbClock : BusClock<AhbClock, ApbPrescalerBitField>
    {
        using Base = BusClock<AhbClock, ApbPrescalerBitField>;
    public:
        /**
         * @brief APB1 clock prescalers
         */
        enum Prescaler
        {
            Div1 = RCC_CFGR_PPRE_DIV1 >> ApbPrescalerBitField::Offset, ///< No divide (prescaler = 1)
            Div2 = RCC_CFGR_PPRE_DIV2 >> ApbPrescalerBitField::Offset, ///< Prescaler = 2
            Div4 = RCC_CFGR_PPRE_DIV4 >> ApbPrescalerBitField::Offset, ///< Prescaler = 4
            Div8 = RCC_CFGR_PPRE_DIV8 >> ApbPrescalerBitField::Offset, ///< Prescaler = 8
            Div16 = RCC_CFGR_PPRE_DIV16 >> ApbPrescalerBitField::Offset, ///< Prescaler = 16
        };

        static ClockFrequenceT ClockFreq()
        {
            ClockFrequenceT clock = AhbClock::ClockFreq();
            uint8_t shiftBits = ApbPrescalerShift(ApbPrescalerBitField::Get());
            clock >>= shiftBits;
            return clock;
        }

        template<Prescaler prescaler>
        static void SetPrescaler()
        {
            Base::SetPrescaler(prescaler);
        }
    };
    using Apb1Clock = ApbClock;
    using Apb2Clock = ApbClock;

    using AdcPrescalerBitField = MaskedBitField<PeriphReg<RCC_BASE, ^^RCC_TypeDef::CFGR>, RCC_CFGR_ADCPRE_Msk>;

    class AdcClockSource : _AdcClockSource
    {
    public:
        /**
         * @brief ADC clock sources
         */
        enum ClockSource
        {
            Apb2 = 0, ///< APB2
        };
        
        /**
         * @brief ADC prescaler
         */
        enum Prescaler
        {
            Div2 = RCC_CFGR_ADCPRE_DIV2 >> AdcPrescalerBitField::Offset, ///< Prescaler = 2
            Div4 = RCC_CFGR_ADCPRE_DIV4 >> AdcPrescalerBitField::Offset, ///< Prescaler = 4
        };
        
        /**
         * @brief Select clock source for ADC
         * 
         * @retval true Always
         */
        template<ClockSource source = ClockSource::Apb2>
        static bool SelectClockSource()
        {
            return true;
        }
        
        /**
         * @brief Set prescaler for ADC
         * 
         * @par Returns
         *	Nothing
        */
        template<Prescaler prescaller>
        static void SetPrescaler()
        {
            AdcPrescalerBitField::Set(static_cast<uint32_t>(prescaller));
        }
        
        /**
         * @brief Returns source clock frequence of ADC
         * 
         * @returns Clock frequence
         */
        static ClockFrequenceT SrcClockFreq()
        {
            return ApbClock::ClockFreq();
        }
        
        /**
         * @brief Returns current clock frequence of ADC
         * 
         * @returns Current frequence
         */
        static ClockFrequenceT ClockFreq()
        {
            return SrcClockFreq() / ((AdcPrescalerBitField::Get() + 1) * 2);
        }
    };
    
    using AhbClockEnableReg = PeriphReg<RCC_BASE, ^^RCC_TypeDef::AHBENR>;
    using PeriphClockEnable1 = PeriphReg<RCC_BASE, ^^RCC_TypeDef::APB1ENR>;
    using PeriphClockEnable2 = PeriphReg<RCC_BASE, ^^RCC_TypeDef::APB2ENR>;

    using CrcClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_CRCEN, AhbClock>;
    using DmaClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_DMAEN, AhbClock>;
    using Dma1Clock = DmaClock;
    using FlitfClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_FLITFEN, AhbClock>;
    using SramClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_SRAMEN, AhbClock>;
    using PortaClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_GPIOAEN, AhbClock>;
    using PortbClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_GPIOBEN, AhbClock>;
    using PortcClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_GPIOCEN, AhbClock>;
    using PortfClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_GPIOFEN, AhbClock>;

    using I2c1Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_I2C1EN, ApbClock>;
    using PowerClock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_PWREN, ApbClock>;
    using Tim3Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_TIM3EN, ApbClock>;
    using Tim14Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_TIM14EN, ApbClock>;
    using WatchDogClock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_WWDGEN, ApbClock>;

    using AdcClock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_ADCEN, AdcClockSource>;
    using DbgMcuClock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_DBGMCUEN, ApbClock>;
    using Spi1Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_SPI1EN, ApbClock>;
    using SysCfgCompClock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_SYSCFGCOMPEN, ApbClock>;
    using Tim1Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_TIM1EN, ApbClock>;
    using Tim16Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_TIM16EN, ApbClock>;
    using Tim17Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_TIM17EN, ApbClock>;
    using Usart1Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_USART1EN, ApbClock>;

    /// Specific *ENR clocks
    #if defined (RCC_AHBENR_DMA2EN)
        using Dma2Clock = ClockControl<AhbClockEnableReg, RCC_AHBENR_DMA2EN, AhbClock>;
    #endif
    #if defined (RCC_AHBENR_GPIODEN)
        using PortdClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_GPIODEN, AhbClock>;
    #endif
    #if defined (RCC_AHBENR_GPIOEEN)
        using PorteClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_GPIOEEN, AhbClock>;
    #endif
    #if defined (RCC_AHBENR_TSCEN)
        using TscClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_TSCEN, AhbClock>;
    #endif

    #if defined (RCC_APB1ENR_CANEN)
        using CanClock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_CANEN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_CECEN)
        using CecClock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_CECEN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_DACEN)
        using DacClock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_DACEN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_I2C2EN)
        using I2c2Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_I2C2EN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_SPI2EN)
        using Spi2Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_SPI2EN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_TIM2EN)
        using Tim2Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_TIM2EN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_TIM6EN)
        using Tim6Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_TIM6EN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_TIM7EN)
        using Tim7Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_TIM7EN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_USART2EN)
        using Usart2Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_USART2EN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_USART3EN)
        using Usart3Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_USART3EN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_USART4EN)
        using Usart4Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_USART4EN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_USART5EN)
        using Usart5Clock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_USART5EN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_CRSEN)
        using CrsClock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_CRSEN, ApbClock>;
    #endif
    #if defined (RCC_APB1ENR_USBEN)
        using UsbClock = ClockControl<PeriphClockEnable1, RCC_APB1ENR_USBEN, ApbClock>;
    #endif

    #if defined (RCC_APB2ENR_TIM15EN)
        using Tim15Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_TIM15EN, ApbClock>;
    #endif
    #if defined (RCC_APB2ENR_USART6EN)
        using Usart6Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_USART6EN, ApbClock>;
    #endif
    #if defined (RCC_APB2ENR_USART7EN)
        using Usart7Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_USART7EN, ApbClock>;
    #endif
    #if defined (RCC_APB2ENR_USART8EN)
        using Usart8Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_USART8EN, ApbClock>;
    #endif
}

#endif //! ZHELE_PLATFORM_STM32_F0_CLOCK_H