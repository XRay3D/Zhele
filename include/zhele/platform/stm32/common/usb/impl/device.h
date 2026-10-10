/**
 * @file
 * Implement USB device class methods
 * 
 * @author Aleksei Zhelonkin
 * @date 2021
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_USB_IMPL_DEVICE_H
#define ZHELE_PLATFORM_STM32_COMMON_USB_IMPL_DEVICE_H

namespace Zhele::Usb
{
#if defined (USB)
    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::Enable()
    {
        _ClockCtrl::Enable();
        _epBufferManager.Init();

        _Regs()->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM;
        _Regs()->ISTR = 0;
        _Regs()->BTABLE = 0;
#if defined (USB_BCDR_DPPU)
        _Regs()->BCDR |= USB_BCDR_DPPU;
#endif
        NVIC_EnableIRQ(_IRQNumber);
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::Reset()
    {
        _Ep0::Reset();
        
        (_Configurations::Reset(), ...);

        _Regs()->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM;
        _Regs()->ISTR = 0;
        _Regs()->BTABLE = 0;
        _Regs()->DADDR = USB_DADDR_EF;
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::CommonHandler()
    {
        NVIC_ClearPendingIRQ(_IRQNumber);

        if(_Regs()->ISTR & USB_ISTR_RESET)
        {
            Reset();
        }
        if (_Regs()->ISTR & USB_ISTR_CTR)
        {
            uint8_t endpoint = _Regs()->ISTR & USB_ISTR_EP_ID;
            _epHandlers.Handle(endpoint, ((_Regs()->ISTR & USB_ISTR_DIR) != 0 ? EndpointDirection::Out : EndpointDirection::In));
        }
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::Handler()
    {
        if(_Ep0::Reg::Get() & USB_EP_CTR_RX)
        {
            _Ep0::ClearCtrRx();
            
            if(_Ep0::Reg::Get() & USB_EP_SETUP)
            {
                HandleSetupRequest(reinterpret_cast<SetupPacket*>(_Ep0::RxBuffer));
            }
            else
            {
                _Ep0::TryHandleDataTransfer();
            }
        }
        if(_Ep0::Reg::Get() & USB_EP_CTR_TX)
        {
            _Ep0::ClearCtrTx();
            _Ep0::HandleTx();
        }
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::SetAddress(uint16_t address)
    {
        _tempAddressStorage = address;
        _Ep0::SendZLP([](){
            _Regs()->DADDR = (USB_DADDR_EF | (_tempAddressStorage & USB_DADDR_ADD));
            _Ep0::SetRxStatus(EndpointStatus::Valid);
        });
    }
#elif defined (USB_OTG_FS)
    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::Enable()
    {
        _ClockCtrl::Enable();
        
        _epBufferManager.Init();

        while (!(_Regs()->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL)) continue;

        _DeviceRegs()->DCFG = USB_OTG_DCFG_DSPD;

        _Regs()->GUSBCFG = USB_OTG_GUSBCFG_FDMOD // Force device mode
                        | (0x06 << USB_OTG_GUSBCFG_TRDT_Pos) // ??
                        | USB_OTG_GUSBCFG_PHYSEL; // USB 2.0

        _Regs()->GCCFG = USB_OTG_GCCFG_NOVBUSSENS; // Exit Power Down mode
        _Regs()->GINTSTS = 0xfffffffful;

        _Regs()->GINTMSK = USB_OTG_GINTMSK_IEPINT   // Enable USB IN TX endpoint interrupt
                            | USB_OTG_GINTMSK_OEPINT   // Enable USB OUT RX endpoint interrupt
                            | USB_OTG_GINTMSK_RXFLVLM // USB reciving
                            | USB_OTG_GINTMSK_USBRST
                            | USB_OTG_GINTMSK_ENUMDNEM;    // Reset interrupt

        _Regs()->GCCFG |= USB_OTG_GCCFG_PWRDWN; // Enable USB
        _DeviceRegs()->DCTL = 0;

        NVIC_EnableIRQ(_IRQNumber);
        _Regs()->GAHBCFG = USB_OTG_GAHBCFG_GINT;       // On USB general interrupt
    }

    template<typename _Regs>
    void FlushTx()
    {
        while ((_Regs()->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0U) continue;

        _Regs()->GRSTCTL = USB_OTG_GRSTCTL_TXFFLSH | (1 << 10);
        while(_Regs()->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) continue;;
    }
        
    template<typename _Regs>
    void FlushRx()
    {
        while ((_Regs()->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0U) continue;
        _Regs()->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
        while (_Regs()->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH);
    }  

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::Reset()
    {
        for (auto i = 0; i < 3; ++i)
            _Regs()->DIEPTXF[i] = 0;

        _epBufferManager.Init();

        _Ep0::Reset();
        (_Configurations::Reset(), ...);

        _DeviceRegs()->DAINTMSK = CalculateDaintMask(_endpoints.template push_back<This>());
        _DeviceRegs()->DOEPMSK = USB_OTG_DOEPMSK_STUPM // Enable setup-done irq
                                | USB_OTG_DOEPMSK_XFRCM;  // Enable tx-done irq

        _DeviceRegs()->DIEPMSK = USB_OTG_DIEPMSK_XFRCM; // Enable transfer complete irq

        FlushTx<_Regs>();
        FlushRx<_Regs>();
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::CommonHandler()
    {
        if (_Regs()->GINTSTS & USB_OTG_GINTSTS_USBRST) {
            _Regs()->GINTSTS = USB_OTG_GINTSTS_USBRST;
            Reset();
        }

        if (_Regs()->GINTSTS & USB_OTG_GINTSTS_ENUMDNE) { 
            _Regs()->GINTSTS = USB_OTG_GINTSTS_ENUMDNE;
            _isDeviceConfigured = true;
        } 

        if (_Regs()->GINTSTS & USB_OTG_GINTSTS_RXFLVL) {
            uint32_t status = _Regs()->GRXSTSP;
            uint16_t size = (status & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;
            uint8_t enpointNumber = status & USB_OTG_GRXSTSP_EPNUM;

            _epFifoNotEmptyHandlers.HandleRxFifoNotEmpty(enpointNumber, size);
        }

        if(_Regs()->GINTSTS & USB_OTG_GINTSTS_OEPINT) {
            uint32_t endpoints = _DeviceRegs()->DAINT & _DeviceRegs()->DAINTMSK;

            if (endpoints & (1 << 16)) {
                _epHandlers.Handle(0, EndpointDirection::Out);
            }
            if (endpoints & (1 << 17)) {
                _epHandlers.Handle(1, EndpointDirection::Out);
            }
            if (endpoints & (1 << 18)) {
                _epHandlers.Handle(2, EndpointDirection::Out);
            }
            if (endpoints & (1 << 19)) {
                _epHandlers.Handle(3, EndpointDirection::Out);
            }
        }

        if(_Regs()->GINTSTS & (USB_OTG_GINTSTS_IEPINT | USB_OTG_GINTSTS_NPTXFE)) {
            uint32_t endpoints = _DeviceRegs()->DAINT & _DeviceRegs()->DAINTMSK;

            if (endpoints & (1 << 0)) {
                _epHandlers.Handle(0, EndpointDirection::In);
            }
            if (endpoints & (1 << 1)) {
                _epHandlers.Handle(1, EndpointDirection::In);
            }
            if (endpoints & (1 << 2)) {
                _epHandlers.Handle(2, EndpointDirection::In);
            }
            if (endpoints & (1 << 3)) {
                _epHandlers.Handle(3, EndpointDirection::In);
            }
        }
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::Handler()
    {
        if (_Ep0::GetOutInterrupts() & USB_OTG_DOEPINT_STUP) {
            HandleSetupRequest(reinterpret_cast<SetupPacket*>(_Ep0::RxBuffer));
#if defined(USB_OTG_FS)
        _Ep0::SetRxStatus(EndpointStatus::Valid);
#endif
        }
        if (_Ep0::GetOutInterrupts() & USB_OTG_DOEPINT_XFRC) {
            _Ep0::TryHandleDataTransfer();
        }

        _Ep0::ClearAllRxInterrupts();

        _Ep0::HandleTx();
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::SetAddress(uint16_t address)
    {        
        _DeviceRegs()->DCFG |= static_cast<uint32_t>(address) << USB_OTG_DCFG_DAD_Pos;
        _Ep0::SendZLP();
    }

#endif
    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    bool DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::IsDeviceConfigured()
    {
        return _isDeviceConfigured;
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    consteval DeviceDescriptor DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::BuildDeviceDescriptor()
    {
        return DeviceDescriptor {
            .UsbVersion = _UsbVersion,
            .Class = _Class,
            .SubClass = _SubClass,
            .Protocol = _Protocol,
            .MaxPacketSize = _Ep0::MaxPacketSize,
            .VendorId = _VendorId,
            .ProductId = _ProductId,
            .DeviceReleaseNumber = _DeviceReleaseNumber,
            .ManufacturerStringIndex = (std::is_same_v<decltype(_Manufacturer), decltype(template_utils::EmptyFixedString16)>) ? 0 : 1,
            .ProductStringIndex = (std::is_same_v<decltype(_Product), decltype(template_utils::EmptyFixedString16)>) ? 0 : 2,
            .SerialNumberStringIndex = (std::is_same_v<decltype(_Serial), decltype(template_utils::EmptyFixedString16)>) ? 0 : 3,
            .ConfigurationsCount = sizeof...(_Configurations)
        };
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    consteval unsigned DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::ConfigurationDescriptorSize()
    {
        uint16_t size = 0;

        _configurations.foreach([&size](auto configuration) {
            size += template_utils::type_unbox<configuration>::GetDescriptor().size();
        });

        return size;
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    consteval auto DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::BuildConfigurationDescriptor()
    {
        constexpr auto size = ConfigurationDescriptorSize();

        std::array<uint8_t, size> result;
        auto dst = result.begin();

        _configurations.foreach([&dst](auto configuration) {
            auto nextConfigurationDescriptor = template_utils::type_unbox<configuration>::GetDescriptor();
            dst = std::copy(nextConfigurationDescriptor.begin(), nextConfigurationDescriptor.end(), dst);
        });

        return result;
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    inline consteval auto DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::BuildStringDescriptor(auto str)
    {
        std::array<uint16_t, str.Size / 2 + 1> result {
            ((2 + str.Size) << 8) | static_cast<uint8_t>(DescriptorType::String)
        };
        auto dst = result.begin();
        ++dst;

        std::copy(str.begin(), str.end(), dst);

        return result;
    }

    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    void DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::HandleSetupRequest(SetupPacket* setupRequest)
    {
        if (setupRequest->RequestType.Recipient == 1) {
            _ifHandlers.HandleSetupRequest(setupRequest->Index & 0xff);
            return;
        }
        
        switch (setupRequest->Request) {
        case StandartRequestCode::GetStatus: {
            static constexpr uint16_t status = 0;
            _Ep0::SendData(&status, sizeof(status));
            break;
        }
        case StandartRequestCode::SetAddress: {
            SetAddress(setupRequest->Value);
            break;
        }

        case StandartRequestCode::GetDescriptor: {
            switch (static_cast<GetDescriptorParameter>(setupRequest->Value)) {
            case GetDescriptorParameter::DeviceDescriptor: {
                static constexpr auto descriptor = BuildDeviceDescriptor();
                _Ep0::SendData(&descriptor, setupRequest->Length < sizeof(descriptor) ? setupRequest->Length : sizeof(descriptor));
                break;
            }

            case GetDescriptorParameter::ConfigurationDescriptor: {
                static constexpr auto descriptor = BuildConfigurationDescriptor();
                _Ep0::SendData(descriptor.data(), setupRequest->Length < descriptor.size() ? setupRequest->Length : descriptor.size());
                break;
            }
            case GetDescriptorParameter::StringLangDescriptor: {
                static constexpr LangIdDescriptor langIdDescriptor;
                _Ep0::SendData(&langIdDescriptor, setupRequest->Length < sizeof(langIdDescriptor) ? setupRequest->Length : sizeof(langIdDescriptor));
                break;
            }
            case GetDescriptorParameter::StringManDescriptor: {
                if constexpr (!std::is_same_v<decltype(_Manufacturer), decltype(template_utils::EmptyFixedString16)>)
                {
                    static constexpr auto descriptor = BuildStringDescriptor(_Manufacturer);
                    static constexpr auto size = descriptor.size() * sizeof(descriptor[0]);
                    _Ep0::SendData(descriptor.data(), setupRequest->Length < size ? setupRequest->Length : size);
                    break;
                }
            }

            case GetDescriptorParameter::StringProdDescriptor: {
                if constexpr (!std::is_same_v<decltype(_Product), decltype(template_utils::EmptyFixedString16)>)
                {
                    static constexpr auto descriptor = BuildStringDescriptor(_Product);
                    static constexpr auto size = descriptor.size() * sizeof(descriptor[0]);
                    _Ep0::SendData(descriptor.data(), setupRequest->Length < size ? setupRequest->Length : size);
                    break;
                }
            }
            case GetDescriptorParameter::StringSerialNumberDescriptor: {
                if constexpr (!std::is_same_v<decltype(_Serial), decltype(template_utils::EmptyFixedString16)>)
                {
                    static constexpr auto descriptor = BuildStringDescriptor(_Serial);
                    static constexpr auto size = descriptor.size() * sizeof(descriptor[0]);
                    _Ep0::SendData(descriptor.data(), setupRequest->Length < size ? setupRequest->Length : size);
                    break;
                }
            }
            default:
                _Ep0::SetTxStatus(EndpointStatus::Stall);
                break;
            }
            break;
        }
        case StandartRequestCode::GetConfiguration: {
            static uint8_t response = _isDeviceConfigured ? 1 : 0;
            _Ep0::SendData(&response, 1);
            break;
        }
        case StandartRequestCode::SetConfiguration: {
            _isDeviceConfigured = true;
            _Ep0::SendZLP();
            break;
        }
        default:
            _Ep0::SetTxStatus(EndpointStatus::Stall);
            break;
        }
    }

#if defined (USB)
    using UsbRegs = IoStruct<USB_BASE, USB_TypeDef>;
    using UsbDeviceRegs = void;
#elif defined (USB_OTG_FS)
    using UsbRegs = IoStruct<USB_OTG_FS_PERIPH_BASE, USB_OTG_GlobalTypeDef>;
    using UsbDeviceRegs = IoStruct<USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE, USB_OTG_DeviceTypeDef>;
#endif

#if defined (USB_LP_IRQn)
    inline constexpr IRQn_Type UsbIrq = USB_LP_IRQn;
#elif defined (USB)
    inline constexpr IRQn_Type UsbIrq = USB_IRQn;
#elif defined (USB_OTG_FS)
    inline constexpr IRQn_Type UsbIrq = OTG_FS_IRQn;
#endif

#if defined (USB)
    using UsbClock = Zhele::Clock::UsbClock;
#elif defined (USB_OTG_FS)
    using UsbClock = Zhele::Clock::OtgFsClock;
#endif
    template<
        uint16_t _UsbVersion,
        DeviceAndInterfaceClass _Class,
        uint8_t _SubClass,
        uint8_t _Protocol,
        uint16_t _VendorId,
        uint16_t _ProductId,
        uint16_t _DeviceReleaseNumber,
        auto _Manufacturer,
        auto _Product,
        auto _Serial,
        typename _Ep0,
        typename... _Configurations>
    using DeviceWithStrings = DeviceBase<UsbRegs, UsbDeviceRegs, UsbIrq, UsbClock, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>;
    template<
        uint16_t _UsbVersion,
        DeviceAndInterfaceClass _Class,
        uint8_t _SubClass,
        uint8_t _Protocol,
        uint16_t _VendorId,
        uint16_t _ProductId,
        uint16_t _DeviceReleaseNumber,
        typename _Ep0,
        typename... _Configurations>
    using Device = DeviceBase<UsbRegs, UsbDeviceRegs, UsbIrq, UsbClock, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, template_utils::EmptyFixedString16, template_utils::EmptyFixedString16, template_utils::EmptyFixedString16, _Ep0, _Configurations...>;
#if defined (USB)
    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    uint8_t DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::_tempAddressStorage = 0x00;
#endif
    template<typename _Regs, typename _DeviceRegs, IRQn_Type _IRQNumber, typename _ClockCtrl, uint16_t _UsbVersion, DeviceAndInterfaceClass _Class, uint8_t _SubClass, uint8_t _Protocol, uint16_t _VendorId, uint16_t _ProductId, uint16_t _DeviceReleaseNumber, auto _Manufacturer, auto _Product, auto _Serial, typename _Ep0, typename... _Configurations>
    volatile bool DeviceBase<_Regs, _DeviceRegs, _IRQNumber, _ClockCtrl, _UsbVersion, _Class, _SubClass, _Protocol, _VendorId, _ProductId, _DeviceReleaseNumber, _Manufacturer, _Product, _Serial, _Ep0, _Configurations...>::_isDeviceConfigured = false;
}

#endif //! ZHELE_PLATFORM_STM32_COMMON_USB_IMPL_DEVICE_H