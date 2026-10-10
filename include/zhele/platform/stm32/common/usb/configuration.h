/**
 * @file
 * Implement USB configuration
 * 
 * @author Aleksei Zhelonkin
 * @date 2021
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_USB_CONFIGURATION_H
#define ZHELE_PLATFORM_STM32_COMMON_USB_CONFIGURATION_H

#include "interface.h"

namespace Zhele::Usb
{
    /**
     * @brief Configuration attributes
     */
    struct ConfigurationAttributes
    {
        uint8_t Reserved : 5; ///< Reserved
        bool RemoteWakeup : 1 = false; ///< Remote wakeup support
        bool SelfPowered : 1 = false; ///< Self-powered
        uint8_t Reserved2 : 1; ///< Reserved

        /// Wire form: bit 5 remote wakeup, bit 6 self-powered
        constexpr uint8_t Byte() const { return (RemoteWakeup ? 1u << 5 : 0u) | (SelfPowered ? 1u << 6 : 0u); }
    };

#pragma pack(push, 1)
    /**
     * @brief Configuration descriptor
     */
    struct ConfigurationDescriptor
    {
        uint8_t Length = 9; ///< Length (always 9)
        DescriptorType Type = DescriptorType::Configuration; ///< Descriptor type (always 0x02)
        uint16_t TotalLength; ///< Total descriptor length (included all interfaces and endpoints descriptors)
        uint8_t InterfacesCount; ///< Interfaces count
        uint8_t Number; ///< Configuration number
        uint8_t StringIndex = 0; ///< Configuration string index
        ConfigurationAttributes Attributes; ///< Configuration attributes
        uint8_t MaxPower; ///< Max power (in 2mA units)

        /**
         * @brief Returns descriptor bytes
         * 
         * @returns Bytes of descriptor
        */
        constexpr auto GetBytes() const { return DescriptorBytes(*this); }
    };
#pragma pack(pop)

    /**
     * @brief Implements configuration
     * 
     * @tparam _Number Configuration number
     * @tparam _MaxPower Max power (in 2mA units)
     * @tparam _RemoteWakeup Remote wakeup feature
     * @tparam _SelfPowered Self-powered feature
     * @tparam _Interfaces Interfaces
     */
    template <uint8_t _Number, uint8_t _MaxPower, bool _RemoteWakeup = false, bool _SelfPowered = false, typename... _Interfaces>
    class Configuration
    {
        /**
         * @brief Returns nested interface descriptor total size
         * 
         * @returns Size in bytes
        */
        static consteval unsigned NestedDescriptorSize()
        {
            unsigned size = 0;

            Interfaces.foreach([&size](auto interface) {
                size += interface.GetDescriptor().size();
            });

            return size;
        }
    public:
        static constexpr auto Interfaces = Zhele::template_utils::type_list<_Interfaces...>{};
        static constexpr auto Endpoints = (Zhele::template_utils::type_list<>{} + ... + _Interfaces::Endpoints);

        /**
         * @brief Resets configuration
         * 
         * @par Returns
         *  Nothing
         */
        static void Reset()
        {
            Interfaces.foreach([](auto interface) {
                interface.Reset();
            });
        }

        /**
         * @brief Build descriptor
         * 
         * @returns Bytes of descriptor
         */
        static consteval auto GetDescriptor()
        {
            constexpr uint16_t size = sizeof(ConfigurationDescriptor) + NestedDescriptorSize();
            std::array<uint8_t, size> result;

            constexpr auto head = ConfigurationDescriptor {
                .TotalLength = size,
                .InterfacesCount = Interfaces.size(),
                .Number = _Number,
                .Attributes = {.RemoteWakeup = _RemoteWakeup, .SelfPowered = _SelfPowered},
                .MaxPower = _MaxPower
            }.GetBytes();

            auto dst = std::copy(head.begin(), head.end(), result.begin());

            Interfaces.foreach([&dst](auto interface) {
                auto nextInterfaceDescriptor = Zhele::template_utils::type_unbox<interface>::GetDescriptor();
                dst = std::copy(nextInterfaceDescriptor.begin(), nextInterfaceDescriptor.end(), dst);
            });

            return result;
        }
    };
}
#endif // ZHELE_PLATFORM_STM32_COMMON_USB_CONFIGURATION_H