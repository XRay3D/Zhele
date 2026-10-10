/**
 * @file
 * Implement DMA protocol
 *
 * @author Konstantin Chizhov
 * @date ??
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_DMA_H
#define ZHELE_PLATFORM_STM32_COMMON_DMA_H

#include <zhele/common/template_utils/enum.h>
#include "ioreg.h"

#include <zhele/clock.h>

#include <cstddef>
#include <span>

namespace Zhele
{
    /// Stream-based DMA (F4/L4) or channel-select DMA (L4 CSELR) takes a request/channel number
    inline constexpr bool dmaStreamSupported =
    #if defined (DMA_SxCR_EN) || defined (DMA_CSELR_C1S)
        true;
    #else
        false;
    #endif
    /// Stream-based DMA registers (CR/NDTR/PAR/M0AR) instead of channel registers (CCR/CNDTR/CPAR/CMAR)
    inline constexpr bool dmaHasStreams =
    #if defined (DMA_SxCR_EN)
        true;
    #else
        false;
    #endif

    class DmaBase
    {
    public:
        /**
         * @brief DMA settings
         *
         * @note There are only two DMA types
         */
        enum Mode : uint32_t
        {
        #if defined (DMA_CCR_EN)
            PriorityLow = 0, ///< Low priority
            PriorityMedium = DMA_CCR_PL_0, ///< Medium priority
            PriorityHigh = DMA_CCR_PL_1, ///< High priority
            PriorityVeryHigh = DMA_CCR_PL_1 | DMA_CCR_PL_0, ///< Very high priority

            MSize8Bits = 0,	///< Memory size 8 bits
            MSize16Bits = DMA_CCR_MSIZE_0, ///< Memory size 16 bits
            MSize32Bits = DMA_CCR_MSIZE_1, ///< Memory size 32 bits

            PSize8Bits = 0,	///< Peripheral size 8 bits
            PSize16Bits = DMA_CCR_PSIZE_0, ///< Peripheral size 16 bits
            PSize32Bits = DMA_CCR_PSIZE_1, ///< Peripheral size 32 bits

            MemIncrement = DMA_CCR_MINC, ///< Enable memory increment
            PeriphIncrement = DMA_CCR_PINC,	///< Peripheral increment
            Circular = DMA_CCR_CIRC, ///< Circular transfer

            Periph2Mem = 0,	///< Peripheral to memory transfer
            Mem2Periph = DMA_CCR_DIR, ///< Memory to peripheral transfer
            Mem2Mem = DMA_CCR_MEM2MEM, ///< Memory to memory transfer

            TransferErrorInterrupt = DMA_CCR_TEIE,	///< Transfer error interrupt
            HalfTransferInterrupt = DMA_CCR_HTIE,	///< Half transfer interrupt
            TransferCompleteInterrupt = DMA_CCR_TCIE	///< Transfer complete interrupt
        #endif
        #if defined(DMA_SxCR_EN)
            PriorityLow = 0,
            PriorityMedium = DMA_SxCR_PL_0,
            PriorityHigh = DMA_SxCR_PL_1,
            PriorityVeryHigh = DMA_SxCR_PL_1 | DMA_SxCR_PL_0,

            MSize8Bits = 0,
            MSize16Bits = DMA_SxCR_MSIZE_0,
            MSize32Bits = DMA_SxCR_MSIZE_1,

            PSize8Bits = 0,
            PSize16Bits = DMA_SxCR_PSIZE_0,
            PSize32Bits = DMA_SxCR_PSIZE_1,

            MemIncrement = DMA_SxCR_MINC,
            PeriphIncrement = DMA_SxCR_PINC,
            Circular = DMA_SxCR_CIRC,

            Periph2Mem = 0,
            Mem2Periph = DMA_SxCR_DIR_0,
            Mem2Mem = DMA_SxCR_DIR_1,

            TransferErrorInterrupt = DMA_SxCR_TEIE,
            HalfTransferInterrupt = DMA_SxCR_HTIE,
            TransferCompleteInterrupt = DMA_SxCR_TCIE,
            DirectModeErrorInterrupt = DMA_SxCR_DMEIE,
        #endif
        };
    };

    /**
     * @brief DMA mode wrapper for support logic operations (AND, OR and more)
     */

    /**
     * @brief DMA channel data
     */
    struct DmaChannelData
    {
        using TransferCallback = std::add_pointer_t<void(void* data, unsigned size, bool success)>;
        /**
         * @brief Default constructor
         *
         * @par Returns
         *	Nothing
         */
        DmaChannelData()
            :transferCallback(nullptr),
            data(nullptr),
            size(0)
        {}

        TransferCallback transferCallback; ///< Transfer complete/error callback pointer

        void *data;	///< Data buffer
        uint16_t size; ///< Data buffer size

        /**
         * @brief Transfer complete handler. Call user`s callback if it has been set
         *
         * @par Returns
         *	Nothing
         */
        inline void NotifyTransferComplete();

        /**
         * @brief Transfer error handler. Call user`s callback if it has been set
         *
         * @par Returns
         *	Nothing
         */
        inline void NotifyError();
    };

    /**
     * @brief Implements DMA channel
     *
     * @tparam _Module Parent DMA module
     * @tparam _ChannelRegs Pointer to associated DMA_Channel_TypeDef structure
     * @tparam _Channel Channel number (start with 1)
     * @tparam _IRQNumber DMA channel IRQ number
     */
    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQNumber>
    class DmaChannel : public DmaBase
    {
        static_assert(_Channel <= _Module::Channels);
        static DmaChannelData Data;
    public:
        using Module = _Module;
        using DmaBase::Mode;
        static constexpr unsigned Channel = _Channel;

        /**
         * @brief Initialize DMA channel and start transfer
         *
         * @param [in] mode Channel mode (support logic operations, OR ("||") for example)
         * @param [in] buffer Memory buffer
         * @param [in] periph Peripheral address (or second memory buffer in Mem2Mem case)
         * @param [in] bufferSize Memory buffer size
         * @param [in] channel Channel (for DMA with streams)
         * @par Returns
         *	Nothing
         */
        static void Transfer(Mode mode, const void* buffer, volatile void* periph, uint32_t bufferSize);

        /**
         * @brief Start transfer with explicit stream request/channel number (stream DMA only)
         */
        static void Transfer(Mode mode, const void* buffer, volatile void* periph, uint32_t bufferSize, uint8_t channel)
            requires (dmaStreamSupported);

        /**
         * @brief Start transfer of a buffer; the element count comes from the span
         *
         * @param [in] mode Channel mode (sizes must match sizeof(T))
         * @param [in] buffer Memory buffer
         * @param [in] periph Peripheral address (or second memory buffer in Mem2Mem case)
         */
        template<typename T>
        static void Transfer(Mode mode, std::span<T> buffer, volatile void* periph)
        {
            Transfer(mode, buffer.data(), periph, buffer.size());
        }

        /**
         * @brief Set transfer callback function
         *
         * @par [in] callback Pointer to callback function
         *
         * @par Returns
         *	Nothing
         */
        static void SetTransferCallback(DmaChannelData::TransferCallback callback);

        /**
         * @brief Check that DMA ready to transfer data
         *
         * @retval true DMA ready
         * @retval false DMA not ready
         */
        static bool Ready();

        /**
         * @brief Check DMA channel for state
         *
         * @retval true DMA channel enabled
         * @retval false DMA channel disabled
         */
        static bool Enabled();

        /**
         * @brief Enable DMA channel
         *
         * @par Returns
         *	Nothing
         */
        static void Enable();

        /**
         * @brief Disable DMA channel
         *
         * @par Returns
         *	Nothing
         */
        static void Disable();

        /**
         * @brief Returns remaining bytes to transfer
         *
         * @details
         *	Bytes remaining = initial size (initial CNDTR value) - current CNDTR value
         *
         *	@returns Byte to transfer remaining
         */
        static uint32_t RemainingTransfers();

        /**
         * @brief Returns peripheral address
         *
         * @returns Peripheral address
         */
        static void* PeriphAddress();

        /**
         * @brief Returns memory buffer address
         *
         * @returns Memory buffer address
         */
        static void* MemAddress();

        /**
         * @brief Returns transfer error state
         *
         * @retval true Transfer error was occured (flag is set)
         * @retval false No transfer error was occured (flag is reset)
         */
        static bool TransferError();

        /**
         * @brief Returns transfer half of data
         *
         * @retval true If half of data was transfered (flag is set)
         * @retval false If not half of data was transfered (flag is reset)
         */
        static bool HalfTransfer();

        /**
         * @brief Returns transfer complete
         *
         * @retval true If transfer complete (flag is set)
         * @retval false If transfer not complete (flag is reset)
         */
        static bool TransferComplete();
    #if defined(DMA_CCR_EN)
        /**
         * @brief Returns interrupt state (occured or not)
         *
         * @retval true Interrupt was occured
         * @retval false Interrupt was not occured
         */
        static bool Interrupt();
    #endif
        /**
         * @brief Clear channel flags
         *
         * @details
         * Clear channel`s flags in IFCR register
         *
         * @par Returns
         *	Nothing
         */
        static void ClearFlags();

        /**
         * @brief Clear transfer error flag
         *
         * @par Returns
         *	Nothing
         */
        static void ClearTransferError();

        /**
         * @brief Clear half transfer complete flag
         *
         * @par Returns
         *	Nothing
         */
        static void ClearHalfTransfer();

        /**
         * @brief Clear transfer complete flag
         *
         * @par Returns
         *	Nothing
         */
        static void ClearTransferComplete();

        /**
         * @brief Clear interrupt flag
         *
         * @par Returns
         *	Nothing
         */
        static void ClearInterrupt();

        /**
         * @brief DMA channel IRQ handler
         *
         * @par Returns
         *	Nothing
         */
        static void IrqHandler();

    private:
        [[gnu::always_inline]] static void TransferImpl(Mode mode, const void* buffer, volatile void* periph, uint32_t bufferSize, uint8_t channel);

        static constexpr uint32_t EnableBit =
        #if defined (DMA_CCR_EN)
            DMA_CCR_EN;
        #else
            DMA_SxCR_EN;
        #endif
        /// Channel/stream register members: CCR/CNDTR/CPAR/CMAR or CR/NDTR/PAR/M0AR
    #if defined (DMA_SxCR_EN)
        static constexpr std::meta::info ControlMember = ^^DMA_Stream_TypeDef::CR;
        static constexpr std::meta::info CountMember = ^^DMA_Stream_TypeDef::NDTR;
        static constexpr std::meta::info PeriphAddrMember = ^^DMA_Stream_TypeDef::PAR;
        static constexpr std::meta::info MemAddrMember = ^^DMA_Stream_TypeDef::M0AR;
    #else
        static constexpr std::meta::info ControlMember = ^^DMA_Channel_TypeDef::CCR;
        static constexpr std::meta::info CountMember = ^^DMA_Channel_TypeDef::CNDTR;
        static constexpr std::meta::info PeriphAddrMember = ^^DMA_Channel_TypeDef::CPAR;
        static constexpr std::meta::info MemAddrMember = ^^DMA_Channel_TypeDef::CMAR;
    #endif
    };

    /**
     * @brief Implements DMA module
     *
     * @tparam DmaRegs Pointer to associated DMA_TypeDef structure
     * @tparam Clock Source clock class
     * @tparam _Channels DMA channels count
     */
    template<typename _DmaRegs, typename _Clock, unsigned _Channels>
    class DmaModule : public DmaBase
    {
        enum class Flags : uint8_t
        {
        #if defined(DMA_CCR_EN)
            Global = DMA_IFCR_CGIF1,
            TransferComplete = DMA_IFCR_CTCIF1,
            HalfTransfer = DMA_IFCR_CHTIF1,
            TransferError = DMA_IFCR_CTEIF1,
            All = Global | TransferComplete | HalfTransfer | TransferError
        #endif
        #if defined(DMA_SxCR_EN)
            TransferComplete = DMA_LISR_TCIF0,
            HalfTransfer = DMA_LISR_HTIF0,
            TransferError = DMA_LISR_TEIF0,
            FifoError = DMA_LISR_FEIF0,
            DirectError = DMA_LISR_DMEIF0,
            All = TransferComplete | HalfTransfer | TransferError | FifoError | DirectError
        #endif
        };
        /**
         * @brief Returns channel flag state
         *
         * @tparam ChannelNum Target channel number
         * @tparam FlagMask Flag mask
         *
         * @retval true If flag is set
         * @retval false If flag is reset
         */
        template<int ChannelNum, Flags FlagMask>
        static bool ChannelFlag();

        /**
         * @brief Clear channel flag
         *
         * @tparam ChannelNum Target channel number
         * @tparam FlagMask Flag mask
         *
         * @par Returns
         *	Nothing
         */
        template<int ChannelNum, Flags FlagMask>
        static void ClearChannelFlag();

    public:
        static const int Channels = _Channels;

        /**
         * @brief Returns transfer error state
         *
         * @tparam ChannelNum Target channel number
         *
         * @retval true Transfer error was occured (flag is set)
         * @retval false No transfer error was occured (flag is reset)
         */
        template<int ChannelNum>
        static bool TransferError();

        /**
         * @brief Returns transfer half of data
         *
         * @tparam ChannelNum Target channel number
         *
         * @retval true If half of data was transfered (flag is set)
         * @retval false If not half of data was transfered (flag is reset)
         */
        template<int ChannelNum>
        static bool HalfTransfer();

        /**
         * @brief Returns transfer complete
         *
         * @tparam ChannelNum Target channel number
         *
         * @retval true If transfer complete (flag is set)
         * @retval false If transfer not complete (flag is reset)
         */
        template<int ChannelNum>
        static bool TransferComplete();

    #if defined(DMA_SxCR_EN)
        template<int ChannelNum>
        static bool FifoError();

        template<int ChannelNum>
        static bool DirectError();
    #endif

    #if defined(DMA_CCR_EN)
        /**
         * @brief Returns interrupt state (occured or not)
         *
         * @tparam ChannelNum Target channel number
         *
         * @retval true Interrupt was occured
         * @retval false Interrupt was not occured
         */
        template<int ChannelNum>
        static bool Interrupt();
    #endif
        /**
         * @brief Clear channel flags
         *
         * @tparam ChannelNum Target channel number
         *
         * @par Returns
         *	Nothing
         */
        template<int ChannelNum>
        static void ClearChannelFlags();

        /**
         * @brief Clear transfer error flag
         *
         * @tparam ChannelNum Target channel number
         *
         * @par Returns
         *	Nothing
         */
        template<int ChannelNum>
        static void ClearTransferError();

        /**
         * @brief Clear half transfer complete flag
         *
         * @tparam ChannelNum Target channel number
         *
         * @par Returns
         *	Nothing
         */
        template<int ChannelNum>
        static void ClearHalfTransfer();

        /**
         * @brief Clear transfer complete flag
         *
         * @tparam ChannelNum Target channel number
         *
         * @par Returns
         *	Nothing
         */
        template<int ChannelNum>
        static void ClearTransferComplete();
    #if defined(DMA_CCR_EN)
        /**
         * @brief Clear interrupt flag
         *
         * @tparam ChannelNum Target channel number
         *
         * @par Returns
         *	Nothing
         */
        template<int ChannelNum>
        static void ClearInterrupt();
    #endif
    #if defined(DMA_SxCR_EN)
        template<int ChannelNum>
        static void ClearFifoError();

        template<int ChannelNum>
        static void ClearDirectError();
    #endif
        /**
         * @brief Enable DMA module clocking
         *
         * @par Returns
         *	Nothing
         */
        static void Enable();

        /**
         * @brief Disable DMA module clocking
         *
         * @par Returns
         *	Nothing
         */
        static void Disable();

    #if defined (DMA_CSELR_C1S)
        /**
         * @brief Set CSELR value for channel
         *
         * @tparam channel Channel number
         *
         * @param channelSelect CSEL value
         *
         * @par Returns
         *  Nothing
         */
        template<uint8_t channel>
        static void SetChannelSelect(uint8_t channelSelect);
    #endif
    };

    template<typename _Module, typename _ChannelRegs, unsigned _Channel, IRQn_Type _IRQnumber>
    DmaChannelData DmaChannel<_Module, _ChannelRegs, _Channel, _IRQnumber>::Data;
}

#include "impl/dma.h"

#endif //! ZHELE_PLATFORM_STM32_COMMON_DMA_H
