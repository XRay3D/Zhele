/**
 * @file
 * Implement USB protocol for stm32f0 series
 * 
 * @author Alexey Zhelonkin
 * @date 2021
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_F0_USB_H
#define ZHELE_PLATFORM_STM32_F0_USB_H

#include <stm32f0xx.h>

#include "../common/usb/device.h"

namespace Zhele::Usb
{
    enum class ClockSource
    {
        Hsi48,
        Pll,
        PllDividedOneAndHalf
    };
    
    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    template<auto clockSource>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::SelectClockSource()
    {
        static_assert(std::is_same_v<decltype(clockSource), Zhele::Usb::ClockSource>, "Clock source argument must be ClockSource enum value.");
        if constexpr(clockSource == Zhele::Usb::ClockSource::Hsi48)
        {
            RCC->APB1ENR |= RCC_APB1ENR_CRSEN;
            CRS->CR |= CRS_CR_AUTOTRIMEN;
            CRS->CR |= CRS_CR_CEN;
        }
        if constexpr (clockSource == Zhele::Usb::ClockSource::Pll || clockSource == Zhele::Usb::ClockSource::PllDividedOneAndHalf)
        {
            RCC->CFGR3 |= RCC_CFGR3_USBSW_PLLCLK;
        }
        if constexpr (clockSource == Zhele::Usb::ClockSource::PllDividedOneAndHalf)
        {
            RCC->CFGR |= RCC_CFGR_USBPRE;
        }
    }
}

#endif //! ZHELE_PLATFORM_STM32_F0_USB_H