/**
 * @file
 * IOPorts methods implementation
 * @author Konstantin Chizhov
 * @date 2012
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IMPL_IOPORTS_H
#define ZHELE_PLATFORM_STM32_COMMON_IMPL_IOPORTS_H

namespace Zhele::IO::Private
{

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::Write(PortImplementation<_Regs, _ClkEnReg, ID>::DataType value)
    {
        _Regs()->ODR = value;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    template<typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType value>
    void PortImplementation<_Regs, _ClkEnReg, ID>::Write()
    {
        _Regs()->ODR = value;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType PortImplementation<_Regs, _ClkEnReg, ID>::Read()
    {
        return _Regs()->ODR;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::Clear(PortImplementation<_Regs, _ClkEnReg, ID>::DataType value)
    {
        _Regs()->BSRR = value << 16;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    template<typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType value>
    void PortImplementation<_Regs, _ClkEnReg, ID>::Clear()
    {
        _Regs()->BSRR = value << 16;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::Set(PortImplementation<_Regs, _ClkEnReg, ID>::DataType value)
    {
        _Regs()->BSRR = value;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    template<typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType value>
    void PortImplementation<_Regs, _ClkEnReg, ID>::Set()
    {
        _Regs()->BSRR = value;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::ClearAndSet(PortImplementation<_Regs, _ClkEnReg, ID>::DataType clearMask, PortImplementation<_Regs, _ClkEnReg, ID>::DataType setMask)
    {
        _Regs()->BSRR = clearMask << 16 | setMask;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    template<typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType clearMask, typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType setMask>
    void PortImplementation<_Regs, _ClkEnReg, ID>::ClearAndSet()
    {
        _Regs()->BSRR = clearMask << 16 | setMask;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::Toggle(PortImplementation<_Regs, _ClkEnReg, ID>::DataType value)
    {
        _Regs()->BSRR = value << 16 | (~_Regs()->ODR & value);
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    template<typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType value>
    void PortImplementation<_Regs, _ClkEnReg, ID>::Toggle()
    {
        _Regs()->BSRR = value << 16 | (_Regs()->ODR ^ value);
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType PortImplementation<_Regs, _ClkEnReg, ID>::PinRead()
    {
        return _Regs()->IDR;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::SetConfiguration(PortImplementation<_Regs, _ClkEnReg, ID>::Configuration configuration, PortImplementation<_Regs, _ClkEnReg, ID>::DataType mask)
    {
        _Regs()->MODER = UnpackConfig2bits(mask, _Regs()->MODER, configuration);
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    template<typename PortImplementation<_Regs, _ClkEnReg, ID>::Configuration configuration, typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType mask>
    void PortImplementation<_Regs, _ClkEnReg, ID>::SetConfiguration()
    {
        _Regs()->MODER = UnpackConfig2bits(mask, _Regs()->MODER, configuration);
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::SetDriverType(PortImplementation<_Regs, _ClkEnReg, ID>::DriverType driver, typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType mask)
    {
        _Regs()->OTYPER = (_Regs()->OTYPER & ~mask) | mask * driver;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    template <typename PortImplementation<_Regs, _ClkEnReg, ID>::DriverType driver, typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType mask>
    void PortImplementation<_Regs, _ClkEnReg, ID>::SetDriverType()
    {
        _Regs()->OTYPER = (_Regs()->OTYPER & ~mask) | mask * driver;
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::SetPullMode(PortImplementation<_Regs, _ClkEnReg, ID>::PullMode mode, typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType mask)
    {
        _Regs()->PUPDR = UnpackConfig2bits(mask, _Regs()->PUPDR, mode);
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    template <typename PortImplementation<_Regs, _ClkEnReg, ID>::PullMode mode, typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType mask>
    void PortImplementation<_Regs, _ClkEnReg, ID>::SetPullMode()
    {
        _Regs()->PUPDR = UnpackConfig2bits(mask, _Regs()->PUPDR, mode);
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::SetSpeed(PortImplementation<_Regs, _ClkEnReg, ID>::Speed speed, typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType mask)
    {
        _Regs()->OSPEEDR = UnpackConfig2bits(mask, _Regs()->OSPEEDR, speed);
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    template<typename PortImplementation<_Regs, _ClkEnReg, ID>::Speed speed, typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType mask>
    void PortImplementation<_Regs, _ClkEnReg, ID>::SetSpeed()
    {
        _Regs()->OSPEEDR = UnpackConfig2bits(mask, _Regs()->OSPEEDR, speed);
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::AltFuncNumber(uint8_t number, typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType mask)
    {
        _Regs()->AFR[0] = UnpackConfig4Bit(mask & 0xff, _Regs()->AFR[0], number);
        _Regs()->AFR[1] = UnpackConfig4Bit((mask >> 8) & 0xff, _Regs()->AFR[1], number);
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    template<uint8_t number, typename PortImplementation<_Regs, _ClkEnReg, ID>::DataType mask>
    void PortImplementation<_Regs, _ClkEnReg, ID>::AltFuncNumber()
    {
        _Regs()->AFR[0] = UnpackConfig4Bit(mask & 0xff, _Regs()->AFR[0], number);
        _Regs()->AFR[1] = UnpackConfig4Bit((mask >> 8) & 0xff, _Regs()->AFR[1], number);
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::Enable()
    {
        _ClkEnReg::Enable();
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    void PortImplementation<_Regs, _ClkEnReg, ID>::Disable()
    {
        _ClkEnReg::Disable();
    }

    template<typename _Regs, typename _ClkEnReg, int ID>
    constexpr inline unsigned PortImplementation<_Regs, _ClkEnReg, ID>::UnpackConfig2bits(unsigned mask, unsigned value, unsigned configuration)
    {
        mask = (mask & 0xff00)     << 8 | (mask & 0x00ff);
        mask = (mask & 0x00f000f0) << 4 | (mask & 0x000f000f);
        mask = (mask & 0x0C0C0C0C) << 2 | (mask & 0x03030303);
        mask = (mask & 0x22222222) << 1 | (mask & 0x11111111);
        return (value & ~(mask * 0x03)) | mask * configuration;
    }
    
    template<typename _Regs, typename _ClkEnReg, int ID>
    constexpr inline unsigned PortImplementation<_Regs, _ClkEnReg, ID>::UnpackConfig4Bit(unsigned mask, unsigned value, unsigned configuration)
    {
        mask = (mask & 0xf0) << 12 | (mask & 0x0f);
        mask = (mask & 0x000C000C) << 6 | (mask & 0x00030003);
        mask = (mask & 0x02020202) << 3 | (mask & 0x01010101);
        return (value & ~(mask * 0x0f)) | mask * configuration;
    }
}
#endif //! ZHELE_PLATFORM_STM32_COMMON_IMPL_IOPORTS_H