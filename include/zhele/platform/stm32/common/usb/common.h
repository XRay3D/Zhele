/**
 * @file
 * Implement USB common data types
 * 
 * @author Aleksei Zhelonkin
 * @date 2021
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_USB_COMMON_H
#define ZHELE_PLATFORM_STM32_COMMON_USB_COMMON_H

#include <stdint.h>
#include <array>
#include <cstddef>
#include <cstring>
#include <meta>
#include <type_traits>
#include <utility>

namespace Zhele::Usb
{
    /**
     * @brief Serialize a packed descriptor struct into its wire bytes (little-endian),
     * member by member via reflection. A member class type must provide `Byte()`.
     *
     * @param descriptor Descriptor struct (packed, no padding)
     * @returns std::array<uint8_t, sizeof(T)>
     */
    template<typename T>
    constexpr std::array<uint8_t, sizeof(T)> DescriptorBytes(const T& descriptor)
    {
        std::array<uint8_t, sizeof(T)> bytes{};
        std::size_t offset = 0;
        template for (constexpr std::meta::info member : std::define_static_array(
            std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked())))
        {
            using Member = typename [:std::meta::type_of(member):];
            const auto& value = descriptor.[:member:];
            if constexpr (std::is_enum_v<Member> || std::is_integral_v<Member>)
            {
                uint32_t raw;
                if constexpr (std::is_enum_v<Member>)
                    raw = static_cast<uint32_t>(std::to_underlying(value));
                else
                    raw = static_cast<uint32_t>(value);
                for (std::size_t i = 0; i < sizeof(Member); ++i)
                    bytes[offset++] = static_cast<uint8_t>(raw >> (8 * i));
            }
            else
            {
                bytes[offset++] = value.Byte();
            }
        }
        return bytes;
    }

/// Thanks ST very much O_o
#if defined (STM32F1) || defined (STM32F3)
    inline constexpr unsigned PmaAlignMultiplier = 2;
#else
    inline constexpr unsigned PmaAlignMultiplier = 1;
#endif

    inline void CopyFromUsbPma(void* destination, const void* source, unsigned size)
    {
        if constexpr(PmaAlignMultiplier != 1) {
            for(int i = 0; i < size / 2; ++i) {
                reinterpret_cast<uint16_t*>(destination)[i] = reinterpret_cast<const uint16_t*>(source)[PmaAlignMultiplier * i];
            }
            if(size & 0x01) {
                reinterpret_cast<uint8_t*>(&destination)[size - 1] = reinterpret_cast<const uint8_t*>(source)[PmaAlignMultiplier * (size - 1)];
            }
        }
        else {
            memcpy(destination, source, size);
        }
    }

    inline void CopyToUsbPma(void* destination, const void* source, unsigned size)
    {
        if constexpr(PmaAlignMultiplier != 1) {
            for(int i = 0; i < size / 2; ++i) {
                reinterpret_cast<uint16_t*>(destination)[PmaAlignMultiplier * i] = reinterpret_cast<const uint16_t*>(source)[i];
            }
            if(size & 0x01) {
                reinterpret_cast<uint8_t*>(&destination)[PmaAlignMultiplier * (size - 1)] = reinterpret_cast<const uint8_t*>(source)[size - 1];
            }
        }
        else {
            memcpy(destination, source, size);
        }
    }

    /**
     * @brief Decription type constant
     */
    enum class DescriptorType : uint8_t
    {
        Device = 0x01, ///< Device descriptor
        Configuration = 0x02, ///< Configuration descriptor
        String = 0x03, ///< String descriptor
        Interface = 0x04, ///< Interface descriptor
        Endpoint = 0x05 ///< Endpoint descriptor
    };

    /**
     * @brief Device and interface class
     */
    enum class DeviceAndInterfaceClass : uint8_t
    {
        InterfaceSpecified = 0x00, ///< Interface specified device
        Audio = 0x01, ///< Audio device
        Comm = 0x02, ///< Communication device (CDC)
        Hid = 0x03, ///< Human input device (HID)
        Monitor = 0x04, ///< Monitor
        Physic = 0x05, ///< Physic device
        Power = 0x06, ///< Power device
        Printer = 0x07, ///< Printer
        Storage = 0x08, ///< Storage device
        Hub = 0x09, ///< Hub
        CdcData = 0x0a, ///< CDC Data
        VendorSpecified = 0xff ///< Vendor specified device
    };
    using InterfaceClass = DeviceAndInterfaceClass; // legacy

    /**
     * @brief USB Standard Request Codes
     */
    enum class StandartRequestCode : uint8_t
    {
        GetStatus = 0x00, ///< Get status
        ClearFeature = 0x01, ///< Clear feature
        SetFeature = 0x03, ///< Set feature
        SetAddress = 0x05, ///< Set address
        GetDescriptor = 0x06, ///< Get descriptor
        SetDescriptor = 0x07, ///< Set descriptor
        GetConfiguration = 0x08, ///< Get configuration
        SetConfiguration = 0x09, ///< Set configuration
        GetInterface = 0x0a, ///< Get interface
        SetInterface = 0x0b, ///< Set interface
        SyncFrame = 0x0c, ///< Syncronisation frame

    };

    /**
     * @brief GET_DESCRIPTOR parameter
     */
    enum class GetDescriptorParameter : uint16_t
    {
        DeviceDescriptor = 0x100, ///< Device descriptor
        ConfigurationDescriptor = 0x200, ///< Configuration descriptor
        HidReportDescriptor = 0x2200, ///< HID report descriptor
        StringLangDescriptor = 0x300, ///< String language descriptor
        StringManDescriptor = 0x301, ///< String manufacture descriptor
        StringProdDescriptor = 0x302, ///< String product descriptor
        StringSerialNumberDescriptor = 0x303, ///< String serial number descriptor
        StringMsOsDescriptor = 0x3ee, ///< MS OS Descriptor
        DeviceQualifierDescriptor = 0x600, ///< Device qualifier descriptor
    };

    /**
     * @brief String descriptor
     */
#pragma pack(push, 1)    
    struct StringDescriptor
    {
        uint8_t Length = 0;
        uint8_t DescriptorType = static_cast<uint8_t>(DescriptorType::String);
        uint8_t String[];
    };
#pragma pack(pop)

    /**
     * @brief Setup packet request type
     */
#pragma pack(push, 1)
    struct SetupRequestType
    {
        uint8_t Recipient : 5; ///< Recipient (0 - device, 1 - interface, 2 - endpoint, 3 - other, 4..31 - reserved)
        uint8_t Type : 2; ///< Type (0 - standart, 1 - class, 2 - vendor, 3 - reserved)
        uint8_t Dir : 1; ///< Direction (0 - out, 1 - in)
    };
#pragma pack(pop)

    /**
     * @brief Setup packet
     */
#pragma pack(push, 1)
#if defined (STM32F1) || defined (STM32F3)
    struct SetupPacket
    {
        SetupRequestType RequestType; ///< Request type
        StandartRequestCode Request; ///< Request
        uint16_t Dummy1; ///< Alignmentn
        uint16_t Value; ///< Value
        uint16_t Dummy2; ///< Alignmentn
        uint16_t Index; ///< Index
        uint16_t Dummy3; ///< Alignmentn
        uint16_t Length; ///< Length
    };
#else
    struct SetupPacket
    {
        SetupRequestType RequestType; ///< Request type
        StandartRequestCode Request; ///< Request
        uint16_t Value; ///< Value
        uint16_t Index; ///< Index
        uint16_t Length; ///< Length
    };
#endif
#pragma pack(pop)
}
#endif // ZHELE_PLATFORM_STM32_COMMON_USB_COMMON_H