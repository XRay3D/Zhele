/**
 * @file
 * IOPorts methods implementation
 * @author Konstantin Chizhov
 * @date 2012
 * @license FreeBSD
 */

#ifndef ZHELE_IOPORTS_IMPL_COMMON_H
#define ZHELE_IOPORTS_IMPL_COMMON_H

namespace Zhele::IO::Private
{
    #define PORTIMPL_TEMPLATE_ARGS template<typename _Regs, typename _ClkEnReg, int ID>
    #define PORTIMPL_TEMPLATE_QUALIFIER PortImplementation<_Regs, _ClkEnReg, ID>

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::Write(PORTIMPL_TEMPLATE_QUALIFIER::DataType value)
    {
        _Regs()->ODR = value;
    }

    PORTIMPL_TEMPLATE_ARGS
    template<PORTIMPL_TEMPLATE_QUALIFIER::DataType value>
    void PORTIMPL_TEMPLATE_QUALIFIER::Write()
    {
        _Regs()->ODR = value;
    }

    PORTIMPL_TEMPLATE_ARGS
    typename PORTIMPL_TEMPLATE_QUALIFIER::DataType PORTIMPL_TEMPLATE_QUALIFIER::Read()
    {
        return _Regs()->ODR;
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::Clear(PORTIMPL_TEMPLATE_QUALIFIER::DataType value)
    {
        _Regs()->BSRR = value << 16;
    }

    PORTIMPL_TEMPLATE_ARGS
    template<typename PORTIMPL_TEMPLATE_QUALIFIER::DataType value>
    void PORTIMPL_TEMPLATE_QUALIFIER::Clear()
    {
        _Regs()->BSRR = value << 16;
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::Set(PORTIMPL_TEMPLATE_QUALIFIER::DataType value)
    {
        _Regs()->BSRR = value;
    }

    PORTIMPL_TEMPLATE_ARGS
    template<typename PORTIMPL_TEMPLATE_QUALIFIER::DataType value>
    void PORTIMPL_TEMPLATE_QUALIFIER::Set()
    {
        _Regs()->BSRR = value;
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::ClearAndSet(PORTIMPL_TEMPLATE_QUALIFIER::DataType clearMask, PORTIMPL_TEMPLATE_QUALIFIER::DataType setMask)
    {
        _Regs()->BSRR = clearMask << 16 | setMask;
    }

    PORTIMPL_TEMPLATE_ARGS
    template<typename PORTIMPL_TEMPLATE_QUALIFIER::DataType clearMask, typename PORTIMPL_TEMPLATE_QUALIFIER::DataType setMask>
    void PORTIMPL_TEMPLATE_QUALIFIER::ClearAndSet()
    {
        _Regs()->BSRR = clearMask << 16 | setMask;
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::Toggle(PORTIMPL_TEMPLATE_QUALIFIER::DataType value)
    {
        _Regs()->BSRR = value << 16 | ~_Regs()->ODR & value;
    }

    PORTIMPL_TEMPLATE_ARGS
    template<typename PORTIMPL_TEMPLATE_QUALIFIER::DataType value>
    void PORTIMPL_TEMPLATE_QUALIFIER::Toggle()
    {
        _Regs()->BSRR = value << 16 | ~_Regs()->ODR & value;
    }

    PORTIMPL_TEMPLATE_ARGS
    typename PORTIMPL_TEMPLATE_QUALIFIER::DataType PORTIMPL_TEMPLATE_QUALIFIER::PinRead()
    {
        return _Regs()->IDR;
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::SetConfiguration(PORTIMPL_TEMPLATE_QUALIFIER::Configuration configuration, PORTIMPL_TEMPLATE_QUALIFIER::DataType mask)
    {
        _Regs()->MODER = UnpackConfig2bits(mask, _Regs()->MODER, configuration);
    }

    PORTIMPL_TEMPLATE_ARGS
    template<typename PORTIMPL_TEMPLATE_QUALIFIER::Configuration configuration, typename PORTIMPL_TEMPLATE_QUALIFIER::DataType mask>
    void PORTIMPL_TEMPLATE_QUALIFIER::SetConfiguration()
    {
        _Regs()->MODER = UnpackConfig2bits(mask, _Regs()->MODER, configuration);
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::SetDriverType(PORTIMPL_TEMPLATE_QUALIFIER::DriverType driver, typename PORTIMPL_TEMPLATE_QUALIFIER::DataType mask)
    {
        _Regs()->OTYPER = (_Regs()->OTYPER & ~mask) | mask * driver;
    }

    PORTIMPL_TEMPLATE_ARGS
    template <typename PORTIMPL_TEMPLATE_QUALIFIER::DriverType driver, typename PORTIMPL_TEMPLATE_QUALIFIER::DataType mask>
    void PORTIMPL_TEMPLATE_QUALIFIER::SetDriverType()
    {
        _Regs()->OTYPER = (_Regs()->OTYPER & ~mask) | mask * driver;
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::SetPullMode(PORTIMPL_TEMPLATE_QUALIFIER::PullMode mode, typename PORTIMPL_TEMPLATE_QUALIFIER::DataType mask)
    {
        _Regs()->PUPDR = UnpackConfig2bits(mask, _Regs()->PUPDR, mode);
    }

    PORTIMPL_TEMPLATE_ARGS
    template <typename PORTIMPL_TEMPLATE_QUALIFIER::PullMode mode, typename PORTIMPL_TEMPLATE_QUALIFIER::DataType mask>
    void PORTIMPL_TEMPLATE_QUALIFIER::SetPullMode()
    {
        _Regs()->PUPDR = UnpackConfig2bits(mask, _Regs()->PUPDR, mode);
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::SetSpeed(PORTIMPL_TEMPLATE_QUALIFIER::Speed speed, typename PORTIMPL_TEMPLATE_QUALIFIER::DataType mask)
    {
        _Regs()->OSPEEDR = UnpackConfig2bits(mask, _Regs()->OSPEEDR, speed);
    }

    PORTIMPL_TEMPLATE_ARGS
    template<typename PORTIMPL_TEMPLATE_QUALIFIER::Speed speed, typename PORTIMPL_TEMPLATE_QUALIFIER::DataType mask>
    void PORTIMPL_TEMPLATE_QUALIFIER::SetSpeed()
    {
        _Regs()->OSPEEDR = UnpackConfig2bits(mask, _Regs()->OSPEEDR, speed);
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::AltFuncNumber(uint8_t number, typename PORTIMPL_TEMPLATE_QUALIFIER::DataType mask)
    {
        _Regs()->AFR[0] = UnpackConfig4Bit(mask & 0xff, _Regs()->AFR[0], number);
        _Regs()->AFR[1] = UnpackConfig4Bit((mask >> 8) & 0xff, _Regs()->AFR[1], number);
    }

    PORTIMPL_TEMPLATE_ARGS
    template<uint8_t number, typename PORTIMPL_TEMPLATE_QUALIFIER::DataType mask>
    void PORTIMPL_TEMPLATE_QUALIFIER::AltFuncNumber()
    {
        _Regs()->AFR[0] = UnpackConfig4Bit(mask & 0xff, _Regs()->AFR[0], number);
        _Regs()->AFR[1] = UnpackConfig4Bit((mask >> 8) & 0xff, _Regs()->AFR[1], number);
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::Enable()
    {
        _ClkEnReg::Enable();
    }

    PORTIMPL_TEMPLATE_ARGS
    void PORTIMPL_TEMPLATE_QUALIFIER::Disable()
    {
        _ClkEnReg::Disable();
    }
}
#endif //! ZHELE_IOPORTS_IMPL_COMMON_H