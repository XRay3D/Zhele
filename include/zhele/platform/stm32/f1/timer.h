/**
 * @file
 * @brief Macros, templates for timers for stm32f1 series
 * @author Alexey Zhelonkin
 * @date 2019
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_F1_TIMER_H
#define ZHELE_PLATFORM_STM32_F1_TIMER_H

#include "../common/timer.h"

#include "iopins.h"
#include "remap.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace Zhele::Timers
{
    namespace Private
    {
        /// AFIO remap bit field of a timer, by its register block address
        consteval std::meta::info TimerRemapOf(uintptr_t base)
        {
            if (base == TIM1_BASE) return ^^Zhele::IO::Timer1Remap;
            if (base == TIM2_BASE) return ^^Zhele::IO::Timer2Remap;
            if (base == TIM3_BASE) return ^^Zhele::IO::Timer3Remap;
        #if defined (TIM4)
            if (base == TIM4_BASE) return ^^Zhele::IO::Timer4Remap;
        #endif
            return ^^Zhele::NullReg<uint32_t>;
        }

        template<typename Regs>
        using GetTimerRemap = typename [:TimerRemapOf(Regs::Address):];

        template <typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
        template <unsigned _ChannelNumber>
        void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::SelectPins(int pinNumber)
        {
            using Pins = GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::Pins;
            using PinsConfig = GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::PinsConfig;
            using Type = typename Pins::DataType;

            Type mask = 1 << pinNumber;

            Pins::Enable();
            Pins::SetConfiguration(Pins::Configuration::AltFunc, mask);
            Pins::SetDriverType(Pins::DriverType::PushPull, mask);
            GetTimerRemap<_Regs>::Set(PinsConfig::alt_functions[static_cast<size_t>(pinNumber)]);
        }

        template <typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
        template <unsigned _ChannelNumber>
        template <unsigned PinNumber>
        void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::SelectPins()
        {
            using Pins = GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::Pins;
            using PinsConfig = GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::PinsConfig;
            using Pin = Pins::template Pin<PinNumber>;
            
            Pin::Port::Enable();
            Pin::template SetConfiguration<Pins::Configuration::AltFunc>();
            Pin::template SetDriverType<Pins::DriverType::PushPull>();
            GetTimerRemap<_Regs>::Set(PinsConfig::alt_functions[PinNumber]);
        }

        template <typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
        template <unsigned _ChannelNumber>
        template <typename Pin>
        void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::SelectPins()
        {
            using Pins = GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::Pins;
            static_assert(Pins::template IndexOf<Pin> >= 0);
            
            SelectPins<Pins::template IndexOf<Pin>>();
        }

        template <typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
        template <unsigned _ChannelNumber>
        void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::SelectPins(int pinNumber)
        {
            using Pins = GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::Pins;
            using PinsConfig = GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::PinsConfig;
            using Type = typename Pins::DataType;

            Type mask = 1 << pinNumber;

            Pins::Enable();
            Pins::SetConfiguration(Pins::Configuration::AltFunc, mask);
            GetTimerRemap<_Regs>::Set(PinsConfig::alt_functions[static_cast<size_t>(pinNumber)]);
        }

        template <typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
        template <unsigned _ChannelNumber>
        template <unsigned PinNumber>
        void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::SelectPins()
        {
            using Pins = GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::Pins;
            using PinsConfig = GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::PinsConfig;
            using Pin = typename Pins::template Pin<PinNumber>;
            
            Pin::Port::Enable();
            Pin::template SetConfiguration<Pin::Configuration::AltFunc>();
            GetTimerRemap<_Regs>::Set(PinsConfig::alt_functions[PinNumber]);
        }

        template <typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
        template <unsigned _ChannelNumber>
        template <typename Pin>
        void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::SelectPins()
        {
            using Pins = GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::Pins;
            static_assert(Pins::template IndexOf<Pin> >= 0);
            
            SelectPins<Pins::template IndexOf<Pin>>();
        }

        using namespace Zhele::IO;

        template<unsigned ChannelNumber> struct Tim1ChPins;
        template<> struct Tim1ChPins<0>{ struct Pins { using io_pins = IO::PinList<Pa8, Pe9>; static constexpr std::array<uint8_t, 2> alt_functions{0, 3}; }; };
        template<> struct Tim1ChPins<1>{ struct Pins { using io_pins = IO::PinList<Pa9, Pe11>; static constexpr std::array<uint8_t, 2> alt_functions{0, 3}; }; };
        template<> struct Tim1ChPins<2>{ struct Pins { using io_pins = IO::PinList<Pa10, Pe13>; static constexpr std::array<uint8_t, 2> alt_functions{0, 3}; }; };
        template<> struct Tim1ChPins<3>{ struct Pins { using io_pins = IO::PinList<Pa11, Pe14>; static constexpr std::array<uint8_t, 2> alt_functions{0, 3}; }; };

        template<unsigned ChannelNumber> struct Tim2ChPins;
        template<> struct Tim2ChPins<0>{ struct Pins { using io_pins = IO::PinList<Pa0, Pa15, Pa0, Pa15>; static constexpr std::array<uint8_t, 4> alt_functions{0, 1, 2, 3}; }; };
        template<> struct Tim2ChPins<1>{ struct Pins { using io_pins = IO::PinList<Pa1, Pb3, Pa1, Pb3>; static constexpr std::array<uint8_t, 4> alt_functions{0, 1, 2, 3}; }; };
        template<> struct Tim2ChPins<2>{ struct Pins { using io_pins = IO::PinList<Pa2, Pb10>; static constexpr std::array<uint8_t, 2> alt_functions{0, 2}; }; };
        template<> struct Tim2ChPins<3>{ struct Pins { using io_pins = IO::PinList<Pa3, Pb11>; static constexpr std::array<uint8_t, 2> alt_functions{0, 2}; }; };

        template<unsigned ChannelNumber> struct Tim3ChPins;
        template<> struct Tim3ChPins<0>{ struct Pins { using io_pins = IO::PinList<Pa6, Pb4, Pc6>; static constexpr std::array<uint8_t, 3> alt_functions{0, 2, 3}; }; };
        template<> struct Tim3ChPins<1>{ struct Pins { using io_pins = IO::PinList<Pa7, Pb5, Pc7>; static constexpr std::array<uint8_t, 3> alt_functions{0, 2, 3}; }; };
        template<> struct Tim3ChPins<2>{ struct Pins { using io_pins = IO::PinList<Pb0, Pc8>; static constexpr std::array<uint8_t, 2> alt_functions{0, 3}; }; };
        template<> struct Tim3ChPins<3>{ struct Pins { using io_pins = IO::PinList<Pb1, Pc9>; static constexpr std::array<uint8_t, 2> alt_functions{0, 3}; }; };

#if defined (TIM4)
        template<unsigned ChannelNumber> struct Tim4ChPins;
        template<> struct Tim4ChPins<0>{ struct Pins { using io_pins = IO::PinList<Pb6, Pd12>; static constexpr std::array<uint8_t, 2> alt_functions{0, 1}; }; };
        template<> struct Tim4ChPins<1>{ struct Pins { using io_pins = IO::PinList<Pb7, Pd13>; static constexpr std::array<uint8_t, 2> alt_functions{0, 1}; }; };
        template<> struct Tim4ChPins<2>{ struct Pins { using io_pins = IO::PinList<Pb8, Pd14>; static constexpr std::array<uint8_t, 2> alt_functions{0, 1}; }; };
        template<> struct Tim4ChPins<3>{ struct Pins { using io_pins = IO::PinList<Pb9, Pd15>; static constexpr std::array<uint8_t, 2> alt_functions{0, 1}; }; };
#endif
        using Tim1Regs = IoStruct<TIM1_BASE, TIM_TypeDef>;
        using Tim2Regs = IoStruct<TIM2_BASE, TIM_TypeDef>;
        using Tim3Regs = IoStruct<TIM3_BASE, TIM_TypeDef>;
#if defined (TIM4)
        using Tim4Regs = IoStruct<TIM4_BASE, TIM_TypeDef>;
#endif
#if defined (TIM6)
        using Tim6Regs = IoStruct<TIM6_BASE, TIM_TypeDef>;
#endif
#if defined (TIM7)        
        using Tim7Regs = IoStruct<TIM7_BASE, TIM_TypeDef>;
#endif
    }

    using Timer1 = Private::AdvancedTimer<Private::Tim1Regs, Clock::Tim1Clock, TIM1_UP_IRQn, Private::Tim1ChPins>;
    using Timer2 = Private::GPTimer<Private::Tim2Regs, Clock::Tim2Clock, TIM2_IRQn, Private::Tim2ChPins>;
    using Timer3 = Private::GPTimer<Private::Tim3Regs, Clock::Tim3Clock, TIM3_IRQn, Private::Tim3ChPins>;
#if defined (TIM4)
    using Timer4 = Private::GPTimer<Private::Tim4Regs, Clock::Tim4Clock, TIM4_IRQn, Private::Tim4ChPins>;
#endif
#if defined (TIM6)
    using Timer6 = Private::BaseTimer<Private::Tim6Regs, Clock::Tim6Clock, TIM6_IRQn>;
#endif
#if defined (TIM7)
    using Timer7 = Private::BaseTimer<Private::Tim7Regs, Clock::Tim7Clock, TIM7_IRQn>;
#endif
}

#endif //! ZHELE_PLATFORM_STM32_F1_TIMER_H