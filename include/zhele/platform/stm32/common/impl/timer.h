/**
 * @file
 * @brief Macros, templates for timers
 * @author Konstantin Chizhov
 * @date 2016
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IMPL_TIMER_H
#define ZHELE_PLATFORM_STM32_COMMON_IMPL_TIMER_H

namespace Zhele::Timers::Private
{

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    unsigned BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::GetClockFreq()
    {
        return _ClockEnReg::ClockFreq();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Enable()
    {
        _ClockEnReg::Enable();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Disable()
    {
        _ClockEnReg::Disable();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::SetCounterMode(CounterMode counterMode)
    {
        _Regs()->CR1 = (_Regs()->CR1 & ~TIM_CR1_CMS_Msk) | static_cast<uint16_t>(counterMode);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::SetCounterValue(BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Counter counter)
    {
        _Regs()->CNT = counter;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    typename BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Counter BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::GetCounterValue()
    {
        return _Regs()->CNT;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::ResetCounterValue()
    {
        _Regs()->CNT = 0;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::SetPrescaler(BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Prescaler prescaler)
    {
        _Regs()->PSC = prescaler;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    typename BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Counter BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::GetPrescaler()
    {
        return _Regs()->PSC;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::SetPeriod(BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Counter period)
    {
        _Regs()->ARR = period;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::SetPeriodAndUpdate(BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Counter value)
    {
        _Regs()->ARR = value;
        _Regs()->EGR = TIM_EGR_UG;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    typename BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Counter BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::GetPeriod()
    {
        return _Regs()->ARR;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::EnableOnePulseMode()
    {
        _Regs::template SetBits<^^TIM_TypeDef::CR1, TIM_CR1_OPM>();
    }    

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::DisableOnePulseMode()
    {
        _Regs::template ClearBits<^^TIM_TypeDef::CR1, TIM_CR1_OPM>();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Stop()
    {
        _Regs::template ClearBits<^^TIM_TypeDef::CR1, TIM_CR1_CEN>();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::Start()
    {
        _Regs::template SetBits<^^TIM_TypeDef::CR1, TIM_CR1_CEN>();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::EnableInterrupt(Interrupt interruptMask)
    {
        _Regs()->DIER |= static_cast<uint16_t>(interruptMask);
        NVIC_EnableIRQ(_IRQNumber);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::DisableInterrupt(Interrupt interruptMask)
    {
        _Regs()->DIER &= ~(static_cast<uint16_t>(interruptMask));
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    bool BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::IsInterrupt()
    {
        return (_Regs()->SR & TIM_SR_UIF) != 0;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::ClearInterruptFlag()
    {
        _Regs()->SR = 0;
        NVIC_ClearPendingIRQ(_IRQNumber);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::SetMasterMode(BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::MasterMode mode)
    {
        _Regs()->CR2 |= static_cast<uint32_t>(mode);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::DmaRequestEnable()
    {
        _Regs::template SetBits<^^TIM_TypeDef::DIER, TIM_DIER_UDE>();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber>
    void BaseTimer<_Regs, _ClockEnReg, _IRQNumber>::DmaRequestDisable()
    {
        _Regs::template ClearBits<^^TIM_TypeDef::DIER, TIM_DIER_UDE>();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::SlaveMode::EnableSlaveMode(Mode mode)
    {
        _Regs()->SMCR = (_Regs()->SMCR & ~TIM_SMCR_SMS_Msk) | static_cast<uint16_t>(mode);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::SlaveMode::DisableSlaveMode()
    {
        _Regs()->SMCR &= ~TIM_SMCR_SMS_Msk;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::SlaveMode::SelectTrigger(Trigger trigger)
    {
        _Regs()->SMCR = (_Regs()->SMCR & ~TIM_SMCR_TS_Msk) | static_cast<uint16_t>(trigger);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::SlaveMode::SetTriggerPrescaler(ExternalTriggerPrescaler prescaler)
    {
        _Regs()->SMCR = (_Regs()->SMCR & ~TIM_SMCR_ETPS_Msk) | static_cast<uint16_t>(prescaler);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::ChannelBase<_ChannelNumber>::EnableInterrupt()
    {
        _Regs()->DIER |= (TIM_DIER_CC1IE << _ChannelNumber);
        NVIC_EnableIRQ(_IRQNumber);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::ChannelBase<_ChannelNumber>::DisableInterrupt()
    {
        _Regs()->DIER &= ~(TIM_DIER_CC1IE << _ChannelNumber);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    bool GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::ChannelBase<_ChannelNumber>::IsInterrupt()
    {
        return _Regs()->SR & (TIM_SR_CC1IF << _ChannelNumber);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::ChannelBase<_ChannelNumber>::ClearInterruptFlag()
    {
        _Regs()->SR = ~(TIM_SR_CC1IF << _ChannelNumber);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::ChannelBase<_ChannelNumber>::EnableDmaRequest()
    {
        _Regs()->DIER |= (TIM_DIER_CC1DE << _ChannelNumber);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::ChannelBase<_ChannelNumber>::DisableDmaRequest()
    {
        _Regs()->DIER &= ~(TIM_DIER_CC1DE << _ChannelNumber);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::ChannelBase<_ChannelNumber>::Enable()
    {
        _Regs()->CCER |= (TIM_CCER_CC1E << (_ChannelNumber * 4));
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::ChannelBase<_ChannelNumber>::Disable()
    {
        _Regs()->CCER &= ~(TIM_CCER_CC1E << (_ChannelNumber * 4));
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::SetCapturePolarity(CapturePolarity polarity)
    {
        _Regs()->CCER = (_Regs()->CCER & ~((TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC1NP) << (_ChannelNumber * 4))) | (polarity << (_ChannelNumber * 4));
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::SetCaptureMode(CaptureMode mode)
    {
        Channel::ModeBitField::Set(mode);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    typename GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::Base::Counter GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::InputCapture<_ChannelNumber>::GetValue()
    {
        return (&_Regs()->CCR1)[_ChannelNumber];
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::SetPulse(typename Base::Counter pulse)
    {
        (&_Regs()->CCR1)[_ChannelNumber] = pulse;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    typename GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::Base::Counter GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::GetPulse()
    {
        return (&_Regs()->CCR1)[_ChannelNumber];
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::SetOutputPolarity(OutputPolarity polarity)
    {
        _Regs()->CCER = (_Regs()->CCER & ~((TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC1NP) << (_ChannelNumber * 4))) | (polarity << (_ChannelNumber * 4));
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::OutputCompare<_ChannelNumber>::SetOutputMode(OutputMode mode)
    {
        _Regs()->CCER = (_Regs()->CCER & ~(TIM_CCER_CC1NP << (_ChannelNumber * 4))) | (TIM_CCER_CC1E << (_ChannelNumber * 4));
        Channel::ModeBitField::Set(mode);
        _Regs::template SetBits<^^TIM_TypeDef::BDTR, TIM_BDTR_MOE>();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::PWMGeneration<_ChannelNumber>::SetOutputFastMode(FastMode mode)
    {
        _Regs()->CCER = (_Regs()->CCER & ~TIM_CCMR1_OC1FE) | static_cast<uint16_t>(mode);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::PWMGeneration<_ChannelNumber>::SelectPins(int pinNumber)
    {
        using Type = typename Pins::DataType;
        Type mask = 1 << pinNumber;
        Pins::Enable();
        Pins::SetSpeed(Pins::Speed::Slow, mask);
        Pins::SetDriverType(Pins::DriverType::PushPull, mask);
        Base::SelectPins(pinNumber);
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    template<unsigned PinNumber>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::PWMGeneration<_ChannelNumber>::SelectPins()
    {
        using Pin = typename Pins::template Pin<PinNumber>;
        Pin::Port::Enable();
        Pin::template SetSpeed<Pin::Speed::Slow>();
        Pin::template SetDriverType<Pins::DriverType::PushPull>();
        Base::template SelectPins<Pin>();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    template<unsigned _ChannelNumber>
    template<typename Pin>
    void GPTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::PWMGeneration<_ChannelNumber>::SelectPins()
    {
        const int index = Pins::template IndexOf<Pin>;
        static_assert(index >= 0);
        Pin::Port::Enable();
        Pin::template SetSpeed<Pin::Speed::Slow>();
        Pin::template SetDriverType<Pins::DriverType::PushPull>();
        Base::template SelectPins<Pin>();
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    void AdvancedTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::SetRepetitionCounter(uint8_t repetitionCounter)
    {
        _Regs()->RCR = repetitionCounter;
    }

    template<typename _Regs, typename _ClockEnReg, IRQn_Type _IRQNumber, template<unsigned> typename _ChPins>
    uint8_t AdvancedTimer<_Regs, _ClockEnReg, _IRQNumber, _ChPins>::GetRepetitionCounter()
    {
        return _Regs()->RCR & 0xff;
    }
}
#endif //! ZHELE_PLATFORM_STM32_COMMON_IMPL_TIMER_H