/**
 * @file
 * Implemets clocks for stm32f4 series
 *
 * @author Alexey Zhelonkin (based on Konstantin Chizhov)
 * @date 2020
 * @license FreeBSD
 */

#ifndef ZHELE_CLOCK_H
#define ZHELE_CLOCK_H

#include "../common/clock.h"
#include <stm32g4xx.h>

namespace Zhele::Clock {
DECLARE_IO_BITFIELD_WRAPPER(RCC->PLLCFGR, PllM, RCC_PLLCFGR_PLLM);
DECLARE_IO_BITFIELD_WRAPPER(RCC->PLLCFGR, PllN, RCC_PLLCFGR_PLLN);
DECLARE_IO_BITFIELD_WRAPPER(RCC->PLLCFGR, PllP, RCC_PLLCFGR_PLLP);
DECLARE_IO_BITFIELD_WRAPPER(RCC->PLLCFGR, PllQ, RCC_PLLCFGR_PLLQ);

#if defined(RCC_PLLCFGR_PLLR_Pos)
DECLARE_IO_BITFIELD_WRAPPER(RCC->PLLCFGR, PllR, RCC_PLLCFGR_PLLR);
#endif

inline unsigned PllClock::GetDivider() {
    return PllM::Get();
}

template <unsigned divider>
inline void PllClock::SetDivider() {
    static_assert(2 <= divider && divider <= PllM::MaxValue, "Invalide divider value");
    PllM::Set(divider);
}

inline unsigned PllClock::GetMultipler() {
    return PllN::Get();
}

template <unsigned multiplier>
inline void PllClock::SetMultiplier() {
    static_assert(2 <= multiplier && multiplier <= 432, "Invalide multiplier value");
    PllN::Set(multiplier);
}

template <PllClock::ClockSource clockSource>
inline void PllClock::SelectClockSource() {
    RCC->PLLCFGR = clockSource == External
        ? RCC->PLLCFGR | RCC_PLLCFGR_PLLSRC_HSE
        : RCC->PLLCFGR & ~RCC_PLLCFGR_PLLSRC;
}

inline PllClock::ClockSource PllClock::GetClockSource() {
    return RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC_HSE
        ? ClockSource::External
        : ClockSource::Internal;
}

inline unsigned PllClock::GetSystemOutputDivider() {
    return (PllP::Get() >> 2) + 2;
}

template <unsigned divider>
inline void PllClock::SetSystemOutputDivider() {
    static_assert(divider == 2 || divider == 4 || divider == 6 || divider == 8, "Divider can be one of 2, 4, 6, 8");
    static constexpr uint8_t pllpValue = (divider - 2) << 2;
    PllP::Set(pllpValue);
}

inline unsigned PllClock::GetUsbOutputDivider() {
    return PllQ::Get();
}

template <unsigned divider>
inline void PllClock::SetUsbOutputDivider() {
    static_assert(2 <= divider && divider <= PllQ::MaxValue, "Invalide divider value");
    PllQ::Set(divider);
}

#if defined(RCC_PLLCFGR_PLLR_Pos)
inline unsigned PllClock::GetI2SOutputDivider() {
    return PllR::Get();
}
template <unsigned divider>
inline void PllClock::SetI2SOutputDivider() {
    static_assert(2 <= divider && divider <= PllR::MaxValue, "Invalide divider value");
    PllR::Set(divider);
}
#endif

DECLARE_IO_BITFIELD_WRAPPER(RCC->CFGR, AhbPrescalerBitField, RCC_CFGR_HPRE);

class AhbClock : public BusClock<SysClock, AhbPrescalerBitField> {
    using Base = BusClock<SysClock, AhbPrescalerBitField>;

public:
    // AHB prescaler values
    enum Prescaler {
        Div1 = RCC_CFGR_HPRE_DIV1 >> AhbPrescalerBitFieldOffset,     ///< No divide (prescaler = 1)
        Div2 = RCC_CFGR_HPRE_DIV2 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 2
        Div4 = RCC_CFGR_HPRE_DIV4 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 4
        Div8 = RCC_CFGR_HPRE_DIV8 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 8
        Div16 = RCC_CFGR_HPRE_DIV16 >> AhbPrescalerBitFieldOffset,   ///< Prescaler = 16
        Div64 = RCC_CFGR_HPRE_DIV64 >> AhbPrescalerBitFieldOffset,   ///< Prescaler = 64
        Div128 = RCC_CFGR_HPRE_DIV128 >> AhbPrescalerBitFieldOffset, ///< Prescaler = 128
        Div256 = RCC_CFGR_HPRE_DIV256 >> AhbPrescalerBitFieldOffset, ///< Prescaler = 256
        Div512 = RCC_CFGR_HPRE_DIV512 >> AhbPrescalerBitFieldOffset  ///< Prescaler = 512
    };

    static ClockFrequenceT ClockFreq() {
        static constexpr uint8_t clockPrescShift[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

        ClockFrequenceT clock = SysClock::ClockFreq();
        uint8_t shiftBits = clockPrescShift[AhbPrescalerBitField::Get()];
        clock >>= shiftBits;
        return clock;
    }

    template <Prescaler prescaler>
    static void SetPrescaler() {
        Base::SetPrescaler(prescaler);
    }
};

class Ahb1Clock : public BusClock<SysClock, AhbPrescalerBitField> {
    using Base = BusClock<SysClock, AhbPrescalerBitField>;

public:
    // AHB prescaler values
    enum Prescaler {
        Div1 = RCC_CFGR_HPRE_DIV1 >> AhbPrescalerBitFieldOffset,     ///< No divide (prescaler = 1)
        Div2 = RCC_CFGR_HPRE_DIV2 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 2
        Div4 = RCC_CFGR_HPRE_DIV4 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 4
        Div8 = RCC_CFGR_HPRE_DIV8 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 8
        Div16 = RCC_CFGR_HPRE_DIV16 >> AhbPrescalerBitFieldOffset,   ///< Prescaler = 16
        Div64 = RCC_CFGR_HPRE_DIV64 >> AhbPrescalerBitFieldOffset,   ///< Prescaler = 64
        Div128 = RCC_CFGR_HPRE_DIV128 >> AhbPrescalerBitFieldOffset, ///< Prescaler = 128
        Div256 = RCC_CFGR_HPRE_DIV256 >> AhbPrescalerBitFieldOffset, ///< Prescaler = 256
        Div512 = RCC_CFGR_HPRE_DIV512 >> AhbPrescalerBitFieldOffset  ///< Prescaler = 512
    };

    static ClockFrequenceT ClockFreq() {
        static constexpr uint8_t clockPrescShift[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

        ClockFrequenceT clock = SysClock::ClockFreq();
        uint8_t shiftBits = clockPrescShift[AhbPrescalerBitField::Get()];
        clock >>= shiftBits;
        return clock;
    }

    template <Prescaler prescaler>
    static void SetPrescaler() {
        Base::SetPrescaler(prescaler);
    }
};

class Ahb2Clock : public BusClock<SysClock, AhbPrescalerBitField> {
    using Base = BusClock<SysClock, AhbPrescalerBitField>;

public:
    // AHB prescaler values
    enum Prescaler {
        Div1 = RCC_CFGR_HPRE_DIV1 >> AhbPrescalerBitFieldOffset,     ///< No divide (prescaler = 1)
        Div2 = RCC_CFGR_HPRE_DIV2 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 2
        Div4 = RCC_CFGR_HPRE_DIV4 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 4
        Div8 = RCC_CFGR_HPRE_DIV8 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 8
        Div16 = RCC_CFGR_HPRE_DIV16 >> AhbPrescalerBitFieldOffset,   ///< Prescaler = 16
        Div64 = RCC_CFGR_HPRE_DIV64 >> AhbPrescalerBitFieldOffset,   ///< Prescaler = 64
        Div128 = RCC_CFGR_HPRE_DIV128 >> AhbPrescalerBitFieldOffset, ///< Prescaler = 128
        Div256 = RCC_CFGR_HPRE_DIV256 >> AhbPrescalerBitFieldOffset, ///< Prescaler = 256
        Div512 = RCC_CFGR_HPRE_DIV512 >> AhbPrescalerBitFieldOffset  ///< Prescaler = 512
    };

    static ClockFrequenceT ClockFreq() {
        static constexpr uint8_t clockPrescShift[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

        ClockFrequenceT clock = SysClock::ClockFreq();
        uint8_t shiftBits = clockPrescShift[AhbPrescalerBitField::Get()];
        clock >>= shiftBits;
        return clock;
    }

    template <Prescaler prescaler>
    static void SetPrescaler() {
        Base::SetPrescaler(prescaler);
    }
};

class Ahb3Clock : public BusClock<SysClock, AhbPrescalerBitField> {
  using Base = BusClock<SysClock, AhbPrescalerBitField>;

public:
  // AHB prescaler values
  enum Prescaler {
    Div1 = RCC_CFGR_HPRE_DIV1 >> AhbPrescalerBitFieldOffset,     ///< No divide (prescaler = 1)
    Div2 = RCC_CFGR_HPRE_DIV2 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 2
    Div4 = RCC_CFGR_HPRE_DIV4 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 4
    Div8 = RCC_CFGR_HPRE_DIV8 >> AhbPrescalerBitFieldOffset,     ///< Prescaler = 8
    Div16 = RCC_CFGR_HPRE_DIV16 >> AhbPrescalerBitFieldOffset,   ///< Prescaler = 16
    Div64 = RCC_CFGR_HPRE_DIV64 >> AhbPrescalerBitFieldOffset,   ///< Prescaler = 64
    Div128 = RCC_CFGR_HPRE_DIV128 >> AhbPrescalerBitFieldOffset, ///< Prescaler = 128
    Div256 = RCC_CFGR_HPRE_DIV256 >> AhbPrescalerBitFieldOffset, ///< Prescaler = 256
    Div512 = RCC_CFGR_HPRE_DIV512 >> AhbPrescalerBitFieldOffset  ///< Prescaler = 512
  };

  static ClockFrequenceT ClockFreq() {
    static constexpr uint8_t clockPrescShift[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

    ClockFrequenceT clock = SysClock::ClockFreq();
    uint8_t shiftBits = clockPrescShift[AhbPrescalerBitField::Get()];
    clock >>= shiftBits;
    return clock;
  }

  template <Prescaler prescaler>
  static void SetPrescaler() {
    Base::SetPrescaler(prescaler);
  }
};
DECLARE_IO_BITFIELD_WRAPPER(RCC->CFGR, Apb1PrescalerBitField, RCC_CFGR_PPRE1);

/**
 * @brief Implements APB clock
 */
class Apb1Clock : BusClock<AhbClock, Apb1PrescalerBitField> {
    using Base = BusClock<AhbClock, Apb1PrescalerBitField>;

public:
    /**
     * @brief APB1 clock prescalers
     */
    enum Prescaler {
        Div1 = RCC_CFGR_PPRE1_DIV1 >> Apb1PrescalerBitFieldOffset,   ///< No divide (prescaler = 1)
        Div2 = RCC_CFGR_PPRE1_DIV2 >> Apb1PrescalerBitFieldOffset,   ///< Prescaler = 2
        Div4 = RCC_CFGR_PPRE1_DIV4 >> Apb1PrescalerBitFieldOffset,   ///< Prescaler = 4
        Div8 = RCC_CFGR_PPRE1_DIV8 >> Apb1PrescalerBitFieldOffset,   ///< Prescaler = 8
        Div16 = RCC_CFGR_PPRE1_DIV16 >> Apb1PrescalerBitFieldOffset, ///< Prescaler = 16
    };

    static ClockFrequenceT ClockFreq() {
        static constexpr uint8_t clockPrescShift[] = {0, 0, 0, 0, 1, 2, 3, 4};

        ClockFrequenceT clock = AhbClock::ClockFreq();
        uint8_t shiftBits = clockPrescShift[Apb1PrescalerBitField::Get()];
        clock >>= shiftBits;
        return clock;
    }

    template <Prescaler prescaler>
    static void SetPrescaler() {
        Base::SetPrescaler(prescaler);
    }
};

DECLARE_IO_BITFIELD_WRAPPER(RCC->CFGR, Apb2PrescalerBitField, RCC_CFGR_PPRE2);

/**
 * @brief Implements APB clock
 */
class Apb2Clock : BusClock<AhbClock, Apb2PrescalerBitField> {
    using Base = BusClock<AhbClock, Apb2PrescalerBitField>;

public:
    /**
     * @brief APB1 clock prescalers
     */
    enum Prescaler {
        Div1 = RCC_CFGR_PPRE2_DIV1 >> Apb2PrescalerBitFieldOffset,   ///< No divide (prescaler = 1)
        Div2 = RCC_CFGR_PPRE2_DIV2 >> Apb2PrescalerBitFieldOffset,   ///< Prescaler = 2
        Div4 = RCC_CFGR_PPRE2_DIV4 >> Apb2PrescalerBitFieldOffset,   ///< Prescaler = 4
        Div8 = RCC_CFGR_PPRE2_DIV8 >> Apb2PrescalerBitFieldOffset,   ///< Prescaler = 8
        Div16 = RCC_CFGR_PPRE2_DIV16 >> Apb2PrescalerBitFieldOffset, ///< Prescaler = 16
    };

    static ClockFrequenceT ClockFreq() {
        static constexpr uint8_t clockPrescShift[] = {0, 0, 0, 0, 1, 2, 3, 4};

        ClockFrequenceT clock = AhbClock::ClockFreq();
        uint8_t shiftBits = clockPrescShift[Apb2PrescalerBitField::Get()];
        clock >>= shiftBits;
        return clock;
    }

    template <Prescaler prescaler>
    static void SetPrescaler() {
        Base::SetPrescaler(prescaler);
    }
};
/*
template <auto REG, auto NAME, typename DATA_TYPE>
class IoRegWrapper {
public:
    using DataT = DATA_TYPE;
    static DataT Get() { return REG->*NAME; }
    static void Set(DataT value) { REG->*NAME = value; }
    static void Or(DataT value) { REG->*NAME |= value; }
    static void And(DataT value) { REG->*NAME &= value; }
    static void Xor(DataT value) { REG->*NAME ^= value; }
    static void AndOr(DataT andMask, DataT orMask) { REG->*NAME = (REG->*NAME & andMask) | orMask; }
    template <unsigned Bit>
    static bool IsBitSet() { return REG->*NAME & (1 << Bit); }
    template <unsigned Bit>
    static bool IsBitClear() { return !(REG->*NAME & (1 << Bit)); }
};

inline RCC_TypeDef& RCC_ = *reinterpret_cast<RCC_TypeDef*>(RCC_BASE);

using Ahb1ClockEnableReg = IoRegWrapper<&RCC_, &RCC_TypeDef::AHB1ENR, uint32_t>;
using Ahb2ClockEnableReg = IoRegWrapper<&RCC_, &RCC_TypeDef::AHB2ENR, uint32_t>;
using Ahb3ClockEnableReg = IoRegWrapper<&RCC_, &RCC_TypeDef::AHB3ENR, uint32_t>;

using PeriphClockEnable1 = IoRegWrapper<&RCC_, &RCC_TypeDef::APB1ENR, uint32_t>;
using PeriphClockEnable2 = IoRegWrapper<&RCC_, &RCC_TypeDef::APB2ENR, uint32_t>;

using Apb1ResetReg = IoRegWrapper<&RCC_, &RCC_TypeDef::APB1RSTR, uint32_t>;
using Apb2ResetReg = IoRegWrapper<&RCC_, &RCC_TypeDef::APB2RSTR, uint32_t>;
*/
IO_REG_WRAPPER(RCC->AHB1ENR, Ahb1ClockEnableReg, uint32_t);
IO_REG_WRAPPER(RCC->AHB2ENR, Ahb2ClockEnableReg, uint32_t);
IO_REG_WRAPPER(RCC->AHB3ENR, Ahb3ClockEnableReg, uint32_t);

IO_REG_WRAPPER(RCC->APB1ENR1, PeriphClockEnable11, uint32_t);
IO_REG_WRAPPER(RCC->APB1ENR2, PeriphClockEnable12, uint32_t);
IO_REG_WRAPPER(RCC->APB2ENR, PeriphClockEnable2, uint32_t);

IO_REG_WRAPPER(RCC->APB1RSTR1, Apb1ResetReg1, uint32_t);
IO_REG_WRAPPER(RCC->APB1RSTR2, Apb1ResetReg2, uint32_t);
IO_REG_WRAPPER(RCC->APB2RSTR, Apb2ResetReg, uint32_t);

/*
RCC_AHB1SMENR_CORDICSMEN
RCC_AHB1SMENR_CRCSMEN
RCC_AHB1SMENR_DMA1SMEN
RCC_AHB1SMENR_DMA2SMEN
RCC_AHB1SMENR_DMAMUX1SMEN
RCC_AHB1SMENR_FLASHSMEN
RCC_AHB1SMENR_FMACSMEN
RCC_AHB1SMENR_SRAM1SMEN
RCC_AHB2SMENR_ADC12SMEN
RCC_AHB2SMENR_ADC345SMEN
RCC_AHB2SMENR_CCMSRAMSMEN
RCC_AHB2SMENR_DAC1SMEN
RCC_AHB2SMENR_DAC2SMEN
RCC_AHB2SMENR_DAC3SMEN
RCC_AHB2SMENR_DAC4SMEN
RCC_AHB2SMENR_GPIOASMEN
RCC_AHB2SMENR_GPIOBSMEN
RCC_AHB2SMENR_GPIOCSMEN
RCC_AHB2SMENR_GPIODSMEN
RCC_AHB2SMENR_GPIOESMEN
RCC_AHB2SMENR_GPIOFSMEN
RCC_AHB2SMENR_GPIOGSMEN
RCC_AHB2SMENR_RNGSMEN
RCC_AHB2SMENR_SRAM2SMEN
RCC_AHB3SMENR_FMCSMEN
RCC_AHB3SMENR_QSPISMEN
RCC_APB1SMENR1_CRSSMEN
RCC_APB1SMENR1_FDCANSMEN
RCC_APB1SMENR1_I2C1SMEN
RCC_APB1SMENR1_I2C2SMEN
RCC_APB1SMENR1_I2C3SMEN
RCC_APB1SMENR1_LPTIM1SMEN
RCC_APB1SMENR1_PWRSMEN
RCC_APB1SMENR1_RTCAPBSMEN
RCC_APB1SMENR1_SPI2SMEN
RCC_APB1SMENR1_SPI3SMEN
RCC_APB1SMENR1_TIM2SMEN
RCC_APB1SMENR1_TIM3SMEN
RCC_APB1SMENR1_TIM4SMEN
RCC_APB1SMENR1_TIM5SMEN
RCC_APB1SMENR1_TIM6SMEN
RCC_APB1SMENR1_TIM7SMEN
RCC_APB1SMENR1_UART4SMEN
RCC_APB1SMENR1_UART5SMEN
RCC_APB1SMENR1_USART2SMEN
RCC_APB1SMENR1_USART3SMEN
RCC_APB1SMENR1_USBSMEN
RCC_APB1SMENR1_WWDGSMEN
RCC_APB1SMENR2_I2C4SMEN
RCC_APB1SMENR2_LPUART1SMEN
RCC_APB1SMENR2_UCPD1SMEN
RCC_APB2SMENR_HRTIM1SMEN
RCC_APB2SMENR_SAI1SMEN
RCC_APB2SMENR_SPI1SMEN
RCC_APB2SMENR_SPI4SMEN
RCC_APB2SMENR_SYSCFGSMEN
RCC_APB2SMENR_TIM15SMEN
RCC_APB2SMENR_TIM16SMEN
RCC_APB2SMENR_TIM17SMEN
RCC_APB2SMENR_TIM1SMEN
RCC_APB2SMENR_TIM20SMEN
RCC_APB2SMENR_TIM8SMEN
RCC_APB2SMENR_USART1SMEN
*/

#if defined(RCC_AHB1ENR_CORDICEN)
using CordicClock = ClockControl<Ahb1ClockEnableReg, RCC_AHB1ENR_CORDICEN, Ahb1Clock>;
#endif
#if defined(RCC_AHB1ENR_CRCEN)
using CrcClock = ClockControl<Ahb1ClockEnableReg, RCC_AHB1ENR_CRCEN, Ahb1Clock>;
#endif
#if defined(RCC_AHB1ENR_DMA1EN)
using Dma1Clock = ClockControl<Ahb1ClockEnableReg, RCC_AHB1ENR_DMA1EN, Ahb1Clock>;
#endif
#if defined(RCC_AHB1ENR_DMA2EN)
using Dma2Clock = ClockControl<Ahb1ClockEnableReg, RCC_AHB1ENR_DMA2EN, Ahb1Clock>;
#endif
#if defined(RCC_AHB1ENR_DMAMUX1EN)
using Dmamux1Clock = ClockControl<Ahb1ClockEnableReg, RCC_AHB1ENR_DMAMUX1EN, Ahb1Clock>;
#endif
#if defined(RCC_AHB1ENR_FLASHEN)
using FlashClock = ClockControl<Ahb1ClockEnableReg, RCC_AHB1ENR_FLASHEN, Ahb1Clock>;
#endif
#if defined(RCC_AHB1ENR_FMACEN)
using FmacClock = ClockControl<Ahb1ClockEnableReg, RCC_AHB1ENR_FMACEN, Ahb1Clock>;
#endif
#if defined(RCC_AHB2ENR_ADC12EN)
using Adc12Clock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_ADC12EN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_ADC345EN)
using Adc345Clock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_ADC345EN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_DAC1EN)
using Dac1Clock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_DAC1EN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_DAC2EN)
using Dac2Clock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_DAC2EN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_DAC3EN)
using Dac3Clock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_DAC3EN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_DAC4EN)
using Dac4Clock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_DAC4EN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_GPIOAEN)
using PortaClock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_GPIOAEN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_GPIOBEN)
using PortbClock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_GPIOBEN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_GPIOCEN)
using PortcClock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_GPIOCEN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_GPIODEN)
using PortdClock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_GPIODEN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_GPIOEEN)
using PorteClock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_GPIOEEN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_GPIOFEN)
using PortfClock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_GPIOFEN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_GPIOGEN)
using PortgClock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_GPIOGEN, Ahb2Clock>;
#endif
#if defined(RCC_AHB2ENR_RNGEN)
using RngClock = ClockControl<Ahb2ClockEnableReg, RCC_AHB2ENR_RNGEN, Ahb2Clock>;
#endif
#if defined(RCC_AHB3ENR_FMCEN)
using FmcClock = ClockControl<Ahb3ClockEnableReg, RCC_AHB3ENR_FMCEN, Ahb3Clock>;
#endif
#if defined(RCC_AHB3ENR_QSPIEN)
using QspiClock = ClockControl<Ahb3ClockEnableReg, RCC_AHB3ENR_QSPIEN, Ahb3Clock>;
#endif
#if defined(RCC_APB1ENR1_CRSEN)
using CrsClock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_CRSEN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_FDCANEN)
using FdcanClock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_FDCANEN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_I2C1EN)
using I2c1Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_I2C1EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_I2C2EN)
using I2c2Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_I2C2EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_I2C3EN)
using I2c3Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_I2C3EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_LPTIM1EN)
using Lptim1Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_LPTIM1EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_PWREN)
using PwrClock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_PWREN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_RTCAPBEN)
using RtcapbClock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_RTCAPBEN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_SPI2EN)
using Spi2Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_SPI2EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_SPI3EN)
using Spi3Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_SPI3EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_TIM2EN)
using Tim2Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_TIM2EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_TIM3EN)
using Tim3Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_TIM3EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_TIM4EN)
using Tim4Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_TIM4EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_TIM5EN)
using Tim5Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_TIM5EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_TIM6EN)
using Tim6Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_TIM6EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_TIM7EN)
using Tim7Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_TIM7EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_UART4EN)
using Uart4Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_UART4EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_UART5EN)
using Uart5Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_UART5EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_USART2EN)
using Usart2Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_USART2EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_USART3EN)
using Usart3Clock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_USART3EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_USBEN)
using UsbClock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_USBEN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR1_WWDGEN)
using WwdgClock = ClockControl<PeriphClockEnable11, RCC_APB1ENR1_WWDGEN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR2_I2C4EN)
using I2c4Clock = ClockControl<PeriphClockEnable12, RCC_APB1ENR2_I2C4EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR2_LPUART1EN)
using Lpuart1Clock = ClockControl<PeriphClockEnable12, RCC_APB1ENR2_LPUART1EN, Apb1Clock>;
#endif
#if defined(RCC_APB1ENR2_UCPD1EN)
using Ucpd1Clock = ClockControl<PeriphClockEnable12, RCC_APB1ENR2_UCPD1EN, Apb1Clock>;
#endif
#if defined(RCC_APB2ENR_HRTIM1EN)
using Hrtim1Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_HRTIM1EN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_SAI1EN)
using Sai1Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_SAI1EN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_SPI1EN)
using Spi1Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_SPI1EN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_SPI4EN)
using Spi4Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_SPI4EN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_SYSCFGEN)
using SyscfgClock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_SYSCFGEN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_TIM15EN)
using Tim15Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_TIM15EN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_TIM16EN)
using Tim16Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_TIM16EN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_TIM17EN)
using Tim17Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_TIM17EN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_TIM1EN)
using Tim1Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_TIM1EN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_TIM20EN)
using Tim20Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_TIM20EN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_TIM8EN)
using Tim8Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_TIM8EN, Apb2Clock>;
#endif
#if defined(RCC_APB2ENR_USART1EN)
using Usart1Clock = ClockControl<PeriphClockEnable2, RCC_APB2ENR_USART1EN, Apb2Clock>;
#endif

} // namespace Zhele::Clock
#endif //! ZHELE_CLOCK_H
