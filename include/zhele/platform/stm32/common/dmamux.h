/**
 * @file
 * Implement DMAMUX module
 * 
 * @author Alexey Zhelonkin
 * @date 2024
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_DMAMUX_H
#define ZHELE_PLATFORM_STM32_COMMON_DMAMUX_H

#include "ioreg.h"

#include <concepts>
#include <cstddef>
#include <cstdint>

namespace Zhele
{
    /**
     * @brief Implements DMAMUX module
     * 
     * @tparam _Regs DMAMUX base regs
    */
    template<typename _BaseRegs, std::convertible_to<uint32_t> _SyncInput, std::convertible_to<uint32_t> _RequestInput>
    class DmaMux
    {
        static constexpr uint32_t ChannelsOffset = 0x00;
        static constexpr uint32_t ChannelStatusOffset = 0x080;
        static constexpr uint32_t RequestGeneratorOffset = 0x100;
        static constexpr uint32_t RequestGeneratorStatusOffset = 0x140;

        using ChannelRegs = IoStruct<_BaseRegs::Address + ChannelsOffset, DMAMUX_Channel_TypeDef>;
        using ChannelStatusReg = IoStruct<_BaseRegs::Address + ChannelStatusOffset, DMAMUX_ChannelStatus_TypeDef>;
        using RequestGeneratorRegs = IoStruct<_BaseRegs::Address + RequestGeneratorOffset, DMAMUX_RequestGen_TypeDef>;
        using RequestGeneratorStatusReg = IoStruct<_BaseRegs::Address + RequestGeneratorStatusOffset, DMAMUX_RequestGenStatus_TypeDef>;

    public:
        using RequestInput = _RequestInput;
        using SyncInput = _SyncInput;

        /**
         * @brief Implements DMAMUX channel
         * 
         * @tparam _ChannelNumber Channel number
        */
        template<unsigned _ChannelNumber>
        class Channel
        {
            using ChannelControlReg = PeriphReg<ChannelRegs::Address + _ChannelNumber * sizeof(DMAMUX_Channel_TypeDef), ^^DMAMUX_Channel_TypeDef::CCR>;
            using SyncInputBitfield = MaskedBitField<ChannelControlReg, DMAMUX_CxCR_SYNC_ID_Msk>;
            using SyncPolarityBitfield = MaskedBitField<ChannelControlReg, DMAMUX_CxCR_SPOL_Msk>;
            using RequestPolarityBitfield = MaskedBitField<ChannelControlReg, DMAMUX_CxCR_DMAREQ_ID_Msk>;
        public:
            /// @brief Synchronization polarity choise
            enum class SyncPolarity
            {
                None = 0b00,
                Rising = 0b01,
                Falling = 0b10,
                Both = 0b11
            };

            /**
             * @brief Select the synchronization input
             * 
             * @param [in] input Input
             * 
             * @par Returns
             *  Nothing
            */
            static void SetSyncInput(SyncInput input);

            /**
             * @brief Returns current synchronization input
             * 
             * @returns Synchronization input
            */
            static SyncInput GetSyncInput();

            /**
             * @brief Set synchronization polarity for channel
             * 
             * @param [in] polarity Target polarity
             * 
             * @par Returns
             *  Nothing
            */
            static void SetSyncPolarity(SyncPolarity polarity);

            /**
             * @brief Returns current synchronization polarity for channel
             * 
             * @returns Synchronization polarity
            */
            static SyncPolarity GetSyncPolarity();

            /**
             * @brief Enables synchronization
             * 
             * @par Returns
             *  Nothing
            */
            static void EnableSynchronization();

            /**
             * @brief Disables synchronization
             * 
             * @par Returns
             *  Nothing
            */
            static void DisableSynchronization();

            /**
             * @brief Set request input
             * 
             * @param [in] input Input value
             * 
             * @par Returns
             *  Nothing
            */
            static void SelectRequestInput(RequestInput input);

            /**
             * @brief Returns current request input
             * 
             * @returns Current request input
            */
            static RequestInput GetRequestInput();
        };
    };
}

#include "impl/dmamux.h"

#endif //! ZHELE_PLATFORM_STM32_COMMON_DMAMUX_H