/**
 * @file
 * Implemets clocks for stm32g0 series
 * 
 * @author Alexey Zhelonkin
 * @date 2024
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_G0_CLOCK_H
#define ZHELE_PLATFORM_STM32_G0_CLOCK_H

#include <stm32g0xx.h>

// For compatibility with "default" CMSIS (F0/F1/F4)
#define RCC_CFGR_SW_HSI                    0x00000000U                         /*!< HSI selected as system clock */
#define RCC_CFGR_SW_HSE                    0x00000001U                         /*!< HSE selected as system clock */
#define RCC_CFGR_SW_PLL                    0x00000002U                         /*!< PLL selected as system clock */
#define RCC_CFGR_SWS_HSI                   0x00000000U                         /*!< HSI oscillator used as system clock        */
#define RCC_CFGR_SWS_HSE                   0x00000008U                         /*!< HSE oscillator used as system clock        */
#define RCC_CFGR_SWS_PLL                   0x00000010U                         /*!< PLL used as system clock                   */

#include "../common/clock.h"

namespace Zhele::Clock
{
    using PllM = MaskedBitField<PeriphReg<RCC_BASE, ^^RCC_TypeDef::PLLCFGR>, RCC_PLLCFGR_PLLM_Msk>;
    using PllN = MaskedBitField<PeriphReg<RCC_BASE, ^^RCC_TypeDef::PLLCFGR>, RCC_PLLCFGR_PLLN_Msk>;
    using PllP = MaskedBitField<PeriphReg<RCC_BASE, ^^RCC_TypeDef::PLLCFGR>, RCC_PLLCFGR_PLLP_Msk>;

#if defined (RCC_PLLCFGR_PLLQ_Pos)
    using PllQ = MaskedBitField<PeriphReg<RCC_BASE, ^^RCC_TypeDef::PLLCFGR>, RCC_PLLCFGR_PLLQ_Msk>;
#endif

#if defined (RCC_PLLCFGR_PLLR_Pos)
    using PllR = MaskedBitField<PeriphReg<RCC_BASE, ^^RCC_TypeDef::PLLCFGR>, RCC_PLLCFGR_PLLR_Msk>;
#endif

    inline unsigned PllClock::GetDivider()
    {
        return PllM::Get() + 1;
    }

    template<unsigned divider>
    inline void PllClock::SetDivider()
    {
        static_assert(1 <= divider && divider <= (PllM::MaxValue + 1), "Invalid divider value!");
        PllM::Set(divider - 1);
    }

    inline unsigned PllClock::GetMultipler()
    {
        return PllN::Get();
    }

    template<unsigned multiplier>
    inline void PllClock::SetMultiplier()
    {
        static_assert(8 <= multiplier && multiplier <= 86, "Invalid divider value!");
        PllN::Set(multiplier);
    }  

    template<PllClock::ClockSource clockSource>
    inline void PllClock::SelectClockSource()
    {
        RCC->PLLCFGR = (RCC->PLLCFGR & ~(RCC_PLLCFGR_PLLSRC_Msk))
            | (clockSource == External
                ? RCC_PLLCFGR_PLLSRC_HSE
                : RCC_PLLCFGR_PLLSRC_HSI);
    }

    inline PllClock::ClockSource PllClock::GetClockSource()
    {
        return (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC_Msk) == RCC_PLLCFGR_PLLSRC_HSE
            ? ClockSource::External
            : ClockSource::Internal;
    }

    inline unsigned PllClock::GetSystemOutputDivider()
    {
        return (PllR::Get()) + 1;
    }

    template<unsigned divider>
    inline void PllClock::SetSystemOutputDivider()
    {
        static_assert(2 <= divider && divider <= (PllR::MaxValue + 1), "Invalid divider value!");
        RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
        PllR::Set(divider - 1);
    }

#if defined (RCC_PLLCFGR_PLLQ_Pos)
    inline unsigned PllClock::GetUsbOutputDivider()
    {
        return PllQ::Get();
    }

    template<unsigned divider>
    inline void PllClock::SetUsbOutputDivider()
    {
        static_assert(2 <= multiplier && multiplier <= (PllQ::MaxValue + 1), "Invalid divider value!");
        PllQ::Set(divider);
    }
#endif

#if defined (RCC_PLLCFGR_PLLR_Pos)
    inline unsigned PllClock::GetI2SOutputDivider()
    {
        return PllP::Get() + 1;
    }

    template<unsigned divider>
    inline void PllClock::SetI2SOutputDivider()
    {
        static_assert(2 <= divider && divider <= (PllP::MaxValue + 1), "Invalid divider value!");
        RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN;
        PllP::Set(divider - 1);
    }
#endif

    using AhbPrescalerBitField = MaskedBitField<PeriphReg<RCC_BASE, ^^RCC_TypeDef::CFGR>, RCC_CFGR_HPRE_Msk>;

    class AhbClock : public BusClock<SysClock, AhbPrescalerBitField>
    {
        using Base = BusClock<SysClock, AhbPrescalerBitField>;
    public:
        // AHB prescaler values
        enum Prescaler
        {
            Div1 = 0b0000 >> AhbPrescalerBitField::Offset, ///< No divide (prescaler = 1)
            Div2 = 0b1000 >> AhbPrescalerBitField::Offset, ///< Prescaler = 2
            Div4 = 0b1001 >> AhbPrescalerBitField::Offset, ///< Prescaler = 4
            Div8 = 0b1010 >> AhbPrescalerBitField::Offset, ///< Prescaler = 8
            Div16 = 0b1011 >> AhbPrescalerBitField::Offset, ///< Prescaler = 16
            Div64 = 0b1100 >> AhbPrescalerBitField::Offset, ///< Prescaler = 64
            Div128 = 0b1101 >> AhbPrescalerBitField::Offset, ///< Prescaler = 128
            Div256 = 0b1110 >> AhbPrescalerBitField::Offset, ///< Prescaler = 256
            Div512 = 0b1111 >> AhbPrescalerBitField::Offset ///< Prescaler = 512
        };

        static ClockFrequenceT ClockFreq()
        {
            ClockFrequenceT clock = SysClock::ClockFreq();
            uint8_t shiftBits = AhbPrescalerShift(AhbPrescalerBitField::Get());
            clock >>= shiftBits;
            return clock;
        }

        static void SetPrescaler(Prescaler prescaler)
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
            Div1 = 0b000, ///< No divide (prescaler = 1)
            Div2 = 0b100, ///< Prescaler = 2
            Div4 = 0b101, ///< Prescaler = 4
            Div8 = 0b110, ///< Prescaler = 8
            Div16 = 0b111, ///< Prescaler = 16
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
    
    using AhbClockEnableReg = PeriphReg<RCC_BASE, ^^RCC_TypeDef::AHBENR>;
    using PeriphClockEnable1 = PeriphReg<RCC_BASE, ^^RCC_TypeDef::APBENR1>;
    using PeriphClockEnable2 = PeriphReg<RCC_BASE, ^^RCC_TypeDef::APBENR2>;
    using IOPeriphClockEnable = PeriphReg<RCC_BASE, ^^RCC_TypeDef::IOPENR>;

    using AhbResetReg = PeriphReg<RCC_BASE, ^^RCC_TypeDef::AHBRSTR>;
    using ApbResetReg1 = PeriphReg<RCC_BASE, ^^RCC_TypeDef::APBRSTR1>;
    using ApbResetReg2 = PeriphReg<RCC_BASE, ^^RCC_TypeDef::APBRSTR2>;

    using PortaClock = ClockControl<IOPeriphClockEnable, RCC_IOPENR_GPIOAEN, ApbClock>;
    using PortbClock = ClockControl<IOPeriphClockEnable, RCC_IOPENR_GPIOBEN, ApbClock>;
    using PortcClock = ClockControl<IOPeriphClockEnable, RCC_IOPENR_GPIOCEN, ApbClock>;
    using PortdClock = ClockControl<IOPeriphClockEnable, RCC_IOPENR_GPIODEN, ApbClock>;
    using PortfClock = ClockControl<IOPeriphClockEnable, RCC_IOPENR_GPIOFEN, ApbClock>;

    using DmaClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_DMA1EN, AhbClock>;
    using Dma1Clock = DmaClock;
    using FlashClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_FLASHEN, AhbClock>;
    using CrcClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_CRCEN, AhbClock>;

    using Tim3Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_TIM3EN, ApbClock>;
    using RtcClock = ClockControl<PeriphClockEnable1, RCC_APBENR1_RTCAPBEN, ApbClock>;
    using WatchDogClock = ClockControl<PeriphClockEnable1, RCC_APBENR1_WWDGEN, ApbClock>;
    using Spi2Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_SPI2EN, ApbClock>;
    using Usart2Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_USART2EN, ApbClock>;
    using I2c1Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_I2C1EN, ApbClock>;
    using I2c2Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_I2C2EN, ApbClock>;
    using DbgClock = ClockControl<PeriphClockEnable1, RCC_APBENR1_DBGEN, ApbClock>;
    using PowerClock = ClockControl<PeriphClockEnable1, RCC_APBENR1_PWREN, ApbClock>;
    using SysCfgClock = ClockControl<PeriphClockEnable2, RCC_APBENR2_SYSCFGEN, ApbClock>;
    using Tim1Clock = ClockControl<PeriphClockEnable2, RCC_APBENR2_TIM1EN, ApbClock>;
    using Spi1Clock = ClockControl<PeriphClockEnable2, RCC_APBENR2_SPI1EN, ApbClock>;
    using Usart1Clock = ClockControl<PeriphClockEnable2, RCC_APBENR2_USART1EN, ApbClock>;
    using Tim14Clock = ClockControl<PeriphClockEnable2, RCC_APBENR2_TIM14EN, ApbClock>;
    using Tim16Clock = ClockControl<PeriphClockEnable2, RCC_APBENR2_TIM16EN, ApbClock>;
    using Tim17Clock = ClockControl<PeriphClockEnable2, RCC_APBENR2_TIM17EN, ApbClock>;
    using AdcClock = ClockControl<PeriphClockEnable2, RCC_APBENR2_ADCEN, ApbClock>;

#if defined (RCC_APBENR1_TIM2EN)
    using Tim2Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_TIM2EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_LPUART1EN)
    using LpUart1Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_LPUART1EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_LPTIM2EN)
    using LpTim2Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_LPTIM2EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_LPTIM1EN)
    using LpTim1Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_LPTIM1EN, ApbClock>;
#endif
#if defined (RCC_AHBENR_AESEN)
    using AesClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_AESEN, AhbClock>;
#endif
#if defined (RCC_AHBENR_RNGEN)
    using RngClock = ClockControl<AhbClockEnableReg, RCC_AHBENR_RNGEN, AhbClock>;
#endif
#if defined (RCC_APBENR1_TIM6EN)
    using Tim6Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_TIM6EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_TIM7EN)
    using Tim7Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_TIM7EN, ApbClock>;
#endif
#if defined (RCC_APBENR2_TIM15EN)
    using Tim15Clock = ClockControl<PeriphClockEnable2, RCC_APBENR2_TIM15EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_DAC1EN)
    using Dac1Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_DAC1EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_USART3EN)
    using Usart3Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_USART3EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_USART4EN)
    using Usart4Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_USART4EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_CECEN)
    using CecClock = ClockControl<PeriphClockEnable1, RCC_APBENR1_CECEN, ApbClock>;
#endif
#if defined (RCC_APBENR1_UCPD1EN)
    using Ucpd1Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_UCPD1EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_UCPD2EN)
    using Ucpd2Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_UCPD2EN, ApbClock>;
#endif
#if defined (RCC_IOPENR_GPIOEEN)
    using PorteClock = ClockControl<IOPeriphClockEnable, RCC_IOPENR_GPIOEEN, ApbClock>;
#endif
#if defined (RCC_AHBENR_DMA2EN)
    using Dma2Clock = ClockControl<AhbClockEnableReg, RCC_AHBENR_DMA2EN, AhbClock>;
#endif
#if defined (RCC_APBENR1_TIM4EN)
    using Tim4Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_TIM4EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_USART5EN)
    using Usart5Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_USART5EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_USART6EN)
    using Usart6Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_USART6EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_USBEN)
    using UsbClock = ClockControl<PeriphClockEnable1, RCC_APBENR1_USBEN, ApbClock>;
#endif
#if defined (RCC_APBENR1_SPI3EN)
    using Spi3Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_SPI3EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_I2C3EN)
    using I2c3Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_I2C3EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_LPUART2EN)
    using LpUart2Clock = ClockControl<PeriphClockEnable1, RCC_APBENR1_LPUART2EN, ApbClock>;
#endif
#if defined (RCC_APBENR1_FDCANEN)
    using FdCanClock = ClockControl<PeriphClockEnable1, RCC_APBENR1_FDCANEN, ApbClock>;
#endif
#if defined (RCC_APBENR1_CRSEN)
    using CrsClock = ClockControl<PeriphClockEnable1, RCC_APBENR1_CRSEN, ApbClock>;
#endif

/* Sleep/Stop mode not implemented
    using GPIOASMClock = ClockControl<PeriphClockEnable, RCC_IOPSMENR_GPIOASMEN, ApbClock>;
    using GPIOBSMClock = ClockControl<PeriphClockEnable, RCC_IOPSMENR_GPIOBSMEN, ApbClock>;
    using GPIOCSMClock = ClockControl<PeriphClockEnable, RCC_IOPSMENR_GPIOCSMEN, ApbClock>;
    using GPIODSMClock = ClockControl<PeriphClockEnable, RCC_IOPSMENR_GPIODSMEN, ApbClock>;
    using GPIOFSMClock = ClockControl<PeriphClockEnable, RCC_IOPSMENR_GPIOFSMEN, ApbClock>;
    using DMA1SMClock = ClockControl<PeriphClockEnable, RCC_AHBSMENR_DMA1SMEN, ApbClock>;
    using FLASHSMClock = ClockControl<PeriphClockEnable, RCC_AHBSMENR_FLASHSMEN, ApbClock>;
    using SRAMSMClock = ClockControl<PeriphClockEnable, RCC_AHBSMENR_SRAMSMEN, ApbClock>;
    using CRCSMClock = ClockControl<PeriphClockEnable, RCC_AHBSMENR_CRCSMEN, ApbClock>;
    using TIM3SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_TIM3SMEN, ApbClock>;
    using RTCAPBSMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_RTCAPBSMEN, ApbClock>;
    using WWDGSMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_WWDGSMEN, ApbClock>;
    using SPI2SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_SPI2SMEN, ApbClock>;
    using USART2SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_USART2SMEN, ApbClock>;
    using I2C1SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_I2C1SMEN, ApbClock>;
    using I2C2SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_I2C2SMEN, ApbClock>;
    using DBGSMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_DBGSMEN, ApbClock>;
    using PWRSMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_PWRSMEN, ApbClock>;
    using SYSCFGSMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR2_SYSCFGSMEN, ApbClock>;
    using TIM1SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR2_TIM1SMEN, ApbClock>;
    using SPI1SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR2_SPI1SMEN, ApbClock>;
    using USART1SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR2_USART1SMEN, ApbClock>;
    using TIM14SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR2_TIM14SMEN, ApbClock>;
    using TIM16SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR2_TIM16SMEN, ApbClock>;
    using TIM17SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR2_TIM17SMEN, ApbClock>;
    using ADCSMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR2_ADCSMEN, ApbClock>;

#if defined (RCC_AHBSMENR_AESSMEN)
    using AESSMClock = ClockControl<AhbClockEnableReg, RCC_AHBSMENR_AESSMEN, AhbClock>;
#endif
#if defined (RCC_AHBSMENR_RNGSMEN)
    using RNGSMClock = ClockControl<AhbClockEnableReg, RCC_AHBSMENR_RNGSMEN, AhbClock>;
#endif

#if defined (RCC_APBSMENR1_TIM2SMEN)
    using TIM2SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_TIM2SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_LPUART1SMEN)
    using LPUART1SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_LPUART1SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_LPTIM2SMEN)
    using LPTIM2SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_LPTIM2SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_LPTIM1SMEN)
    using LPTIM1SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_LPTIM1SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_TIM6SMEN)
    using Tim6SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_TIM6SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_TIM7SMEN)
    using Tim7SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_TIM7SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR2_TIM15SMEN)
    using TIM15SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR2_TIM15SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_DAC1SMEN)
    using DAC1SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_DAC1SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_USART3SMEN)
    using USART3SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_USART3SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_USART4SMEN)
    using USART4SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_USART4SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_CECSMEN)
    using CECSMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_CECSMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_UCPD1SMEN)
    using UCPD1SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_UCPD1SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_UCPD2SMEN)
    using UCPD2SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_UCPD2SMEN, ApbClock>;
#endif
#if defined (RCC_IOPSMENR_GPIOESMEN)
    using GPIOESMClock = ClockControl<PeriphClockEnable, RCC_IOPSMENR_GPIOESMEN, ApbClock>;
#endif
#if defined (RCC_AHBSMENR_DMA2SMEN)
    using DMA2SMClock = ClockControl<PeriphClockEnable, RCC_AHBSMENR_DMA2SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_TIM4SMEN)
    using TIM4SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_TIM4SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_USART5SMEN)
    using USART5SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_USART5SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_USART6SMEN)
    using USART6SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_USART6SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_USBSMEN)
    using USBSMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_USBSMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_SPI3SMEN)
    using SPI3SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_SPI3SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_I2C3SMEN)
    using I2C3SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_I2C3SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_LPUART2SMEN)
    using LpUart2SMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_LPUART2SMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_FDCANSMEN)
    using FDCANSMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_FDCANSMEN, ApbClock>;
#endif
#if defined (RCC_APBSMENR1_CRSSMEN)
    using CRSSMClock = ClockControl<PeriphClockEnable, RCC_APBSMENR1_CRSSMEN, ApbClock>;
#endif
*/
} // namespace Zhele::Clock

#endif //! ZHELE_PLATFORM_STM32_G0_CLOCK_H