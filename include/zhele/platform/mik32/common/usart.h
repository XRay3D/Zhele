
/// @file
///  Implement UART protocol
/// @author Konstantin Chizhov / X-Ray / X-Ray
/// @date 202626
/// @license MIT

#pragma once

#include "ioreg.h"
#include <cstddef>
#include <span>
#include <zhele/clock.h>
#include <zhele/common/template_utils/enum.h>
// #include <zhele/dma.h>
#include <zhele/iopins.h>
#include <zhele/pinlist.h>

namespace Zhele {
#define UART_TYPE_1
// #define STATUS_REG        ISR
// #define TRANSMIT_DATA_REG TDR
// #define RECEIVE_DATA_REG  RDR

template <uint32_t N>
using Isr = std::integral_constant<uint32_t, N>;

class UsartBase {
public:
    struct UsartMode {

        /// @brief CONTROL1
        enum _CONTROL1 : uint32_t {
            DataBits7 = UART_CONTROL1_M_7BIT_M,
            DataBits8 = UART_CONTROL1_M_8BIT_M,
            DataBits9 = UART_CONTROL1_M_9BIT_M,

            NoneParity = 0,
            EvenParity = UART_CONTROL1_PCE_M,
            OddParity  = UART_CONTROL1_PS_M | UART_CONTROL1_PCE_M,

            Disabled   = 0,
            RxEnable   = UART_CONTROL1_RE_M,
            TxEnable   = UART_CONTROL1_TE_M,
            RxTxEnable = UART_CONTROL1_RE_M | UART_CONTROL1_TE_M | UART_CONTROL1_UE_M,
            Default    = RxTxEnable,
        } CONTROL1;

        enum _CONTROL2 : uint32_t {
            NoClock = 0,
            Clock   = UART_CONTROL2_CLKEN_M,

            OneStopBit = 0,
            // HalfStopBit        = UART_CONTROL2_STOP_0_M,
            TwoStopBits = UART_CONTROL2_STOP_1_M,
            // OneAndHalfStopBits = (UART_CONTROL2_STOP_0_M | UART_CONTROL2_STOP_1_M)
            Default2 = UART_CONTROL2_CPOL_M | UART_CONTROL2_CPHA_M,
        } CONTROL2;

        enum _CONTROL3 : uint32_t {
            FullDuplex = 0,
            HalfDuplex = UART_CONTROL3_HDSEL_M,

            // OneSampleBitDisable = 0,
            // OneSampleBitEnable  = UART_CONTROL3_ONEBIT,
            // Rs485DEControl      = UART_CONTROL3_DEM,

        } CONTROL3;

        constexpr UsartMode(UsartMode::_CONTROL1 control1): CONTROL1{control1} { }
        constexpr UsartMode(UsartMode::_CONTROL2 control2): CONTROL2{control2} { }
        constexpr UsartMode(UsartMode::_CONTROL3 control3): CONTROL3{control3} { }

        constexpr UsartMode(UsartMode::_CONTROL1 control1, UsartMode::_CONTROL2 control2, UsartMode::_CONTROL3 control3)
            : CONTROL1{control1}, CONTROL2{control2}, CONTROL3{control3} { }

        constexpr UsartMode operator|(UsartMode::_CONTROL1 flag) {
            return {CONTROL1 | flag, CONTROL2, CONTROL3};
        }

        constexpr UsartMode operator|(UsartMode::_CONTROL2 flag) {
            return {CONTROL1, CONTROL2 | flag, CONTROL3};
        }

        constexpr UsartMode operator|(UsartMode::_CONTROL3 flag) {
            return {CONTROL1, CONTROL2, CONTROL3 | flag};
        }
    };

    /// @brief All possible interrupts
    enum InterruptFlags {
        NoInterrupt = 0,
#if defined(UART_FLAGS_PE_M)
        ParityErrorInt = UART_FLAGS_PE_M,   //  Parity error
        TxEmptyInt     = UART_FLAGS_TXE_M,  //  Transmission register empty
        TxCompleteInt  = UART_FLAGS_TC_M,   //  Transmission complete
        RxNotEmptyInt  = UART_FLAGS_RXNE_M, //  Read data register not empty
        IdleInt        = UART_FLAGS_IDLE_M, //  Idle mode (all operations finished)
    #if defined(UART_FLAGS_LBDF_M)
        LineBreakInt = UART_FLAGS_LBDF_M,
    #else
        LineBreakInt = 0,
    #endif
        ErrorInt = UART_FLAGS_FE_M | UART_FLAGS_NF_M | UART_FLAGS_ORE_M,
        CtsInt   = UART_FLAGS_CTS_M,
#endif

// UART with FIFO
#if defined(UART_CONTROL1_FIFOEN)
        RxFifoFull      = UART_FLAGS_RXFF_M,
        TxFifoEmpty     = UART_FLAGS_TXFE_M,
        RxFifoThreshold = UART_FLAGS_RXFT_M,
        TxFifoThreshold = UART_FLAGS_TXFT_M,
#endif

#if defined(UART_CONTROL2_RTOEN)
        ReceiveTimeout = UART_FLAGS_RTOF_M,
#endif

        // AllInterrupts = ParityErrorInt | TxEmptyInt | TxCompleteInt | RxNotEmptyInt | IdleInt | LineBreakInt | ErrorInt | CtsInt
#if defined(UART_CONTROL1_FIFOEN)
        | RxFifoFull | TxFifoEmpty | RxFifoThreshold | TxFifoThreshold
#endif
#if defined(UART_CONTROL2_RTOEN)
            | ReceiveTimeout
#endif
    };

    enum Error {
        NoError = 0,
#if defined(UART_TYPE_1)
        OverrunError = UART_FLAGS_ORE_M,
        NoiseError   = UART_FLAGS_NF_M,
        FramingError = UART_FLAGS_FE_M,
        ParityError  = UART_FLAGS_PE_M
#endif
#if defined(UART_TYPE_2)
            OverrunError
            = UART_SR_ORE,
        NoiseError   = UART_SR_NE,
        FramingError = UART_SR_FE,
        ParityError  = UART_SR_PE
#endif
    };

protected:
    static const unsigned ErrorMask = OverrunError | NoiseError | FramingError | ParityError;

    static const unsigned InterruptMask = ParityErrorInt | TxEmptyInt | TxCompleteInt | RxNotEmptyInt | IdleInt | LineBreakInt | ErrorInt | CtsInt
#if defined(UART_CONTROL2_RTOEN)
        | ReceiveTimeout
#endif
        ;
};

constexpr UsartBase::UsartMode DefaultUsartMode{
    UsartBase::UsartMode::RxTxEnable,
    UsartBase::UsartMode::Default2,
    {},
};

template <typename Ty>
using WrData = std::span<const Ty>;

template <typename Ty>
using RdData = std::span<Ty>;

template <typename Ty>
using TransferCallback = std::add_pointer_t<void(WrData<Ty> data, bool success)>;

namespace Private {

template <typename... Rs>
class Usart : public UsartBase {

    // enum Arg {
    //     _Regs,
    //     _IRQNumber,
    //     _ClockCtrl,
    //     _TxPins,
    //     _RxPins,
    //     _DmaTx,
    //     _DmaRx,
    // };

    using IRQNumber = Rs...[1];
    using ClockCtrl = Rs...[2];
    using TxPins    = Rs...[3];
    using RxPins    = Rs...[4];

public:
    using Regs  = Rs...[0];
    using DmaTx = Rs...[5];
    using DmaRx = Rs...[6];

    /// @brief Initialize UART
    /// @tparam baud Baud rate
    /// @param [in] mode Mode
    /// @par Returns Nothing
    template <uint32_t baud>
    static inline void Init(UsartMode mode = DefaultUsartMode);

    /// @brief Initialize UART
    /// @param [in] baud Baud rate
    /// @param[in] mode Mode
    /// @par Returns NothingExceptions
    static void Init(uint32_t baud, UsartMode mode = DefaultUsartMode);

    /// @brief Set config
    /// @param [in] modeMask Mode mask
    static void SetConfig(UsartMode modeMask);

    /// @brief Clear config
    /// @param [in] modeMask Mode mask
    static void ClearConfig(UsartMode modeMask);

    /// @brief Set baud rate
    /// @param [in] baud Baud rate
    /// @par Returns
    ///   Nothing
    static void SetBaud(uint32_t baud);

    /// @brief Check that UART ready to read
    /// @retval true UART ready for read
    /// @retval false UART is not ready for read
    static bool ReadReady();

    /// @brief Synch read data
    /// @returns Readed byte
    static uint8_t Read();

    template <typename Ty>
    static uint8_t Read(WrData<Ty> data, uint32_t tmeout = {});

    /// @brief Enable async read (by DMA)
    /// @param [out] receiveBuffer Output buffer
    /// @param [in] bufferSize Output buffer size
    /// @param [in] callback Transfer complete callback (optional parameter)
    /// @par Returns
    ///  	Nothing
    template <typename Ty>
    static void EnableAsyncRead(WrData<Ty> data, TransferCallback<Ty> callback = nullptr);

    /// @brief Check that UART ready to write
    /// @retval true UART ready for write
    /// @retval false UART is not ready for write
    static bool WriteReady();

    /// @brief Write data to UART
    /// @param [in] data Data to write
    /// @param [in] size Data size
    /// @param [in] async Write async or no
    /// @par Returns
    ///  	Nothing
    // [[deprecated("Replaced by Write/WriteAsync methods")]]
    // template <typename Ty>
    // static void Write(WrData<Ty> data, bool async);

    /// @brief Write data to UART
    /// @param [in] data Data to write
    /// @param [in] size Data size
    /// @par Returns
    ///  	Nothing
    template <typename Ty>
    static void Write(WrData<Ty> data);

    /// @brief Write data to UART async (via DMA)
    /// @param [in] data Data to write
    /// @param [in] size Data size
    /// @param [in] callback Transfer complete callback
    /// @par Returns
    ///  	Nothing
    template <typename Ty>
    static void WriteAsync(WrData<Ty> data, TransferCallback<Ty> callback = nullptr);

    /// @brief Synch write byte
    /// @param [in] data Byte to write
    /// @par Returns Nothing
    static void Write(uint8_t data);

    /// @brief Enables RTOR feature with given timeout value
    /// @param [in] bitCount Timeout in bits
    /// @par Returns
    ///   Nothing
    // static void EnableReceiverTimeout(uint32_t bitCount);

    /// @brief Set RTOR feature with given timeout value
    /// @param [in] bitCount Timeout in bits
    /// @par Returns
    ///   Nothing
    // static void SetReceiverTimeout(uint32_t bitCount);

    /// @brief Enables one or more interrupts
    /// @param [in] interruptFlags Interrupt flags (supports OR ("||") logic operation)
    /// @par Returns Nothing
    static void EnableInterrupt(InterruptFlags interruptFlags);

    /// @brief Disables one or more interrupts
    /// @param [in] interruptFlags Interrupt flags (supports OR ("||") logic operation)
    /// @par Returns Nothing
    static void DisableInterrupt(InterruptFlags interruptFlags);

    /// @brief Returns caused interrupts
    /// @returns Mask of interrupts
    static InterruptFlags InterruptSource();

    /// @brief Returns caused errors
    /// @returns Mask of errors
    static Error GetError();

    /// @brief Clears interrupts
    /// @param [in] interruptFlags Interrupts mask
    /// @par Returns Nothing
    static void ClearInterruptFlag(InterruptFlags interruptFlags);

    /// @brief Clears all interrupts
    /// @par Returns Nothing
    static void ClearAllInterruptFlags();

    /// @brief Select RX and TX pins (set settings)
    /// @param [in] txPinNumber pin number in Txs PinList
    /// @param [in] rxPinNumber pin number in Rxs PinList
    /// @par Returns Nothing
    static void SelectTxRxPins(int8_t txPinNumber, int8_t rxPinNumber = -1);

    /// @brief Template clone of SelectTxRxPins method
    /// @tparam TxPinNumber pin number in Txs PinList
    /// @tparam RxPinNumber pin number in Rxs PinList
    /// @par Returns Nothing
    template <int8_t TxPinNumber, int8_t RxPinNumber = -1>
    static void SelectTxRxPins();

    /// @brief Template clone of SelectTxRxPins method (params are TPin instances)
    /// @tparam TxPin TxPin
    /// @tparam RxPin RxPin
    /// @par Returns Nothing
    template <typename TxPin, typename RxPin = typename IO::NullPin>
    static void SelectTxRxPins();
};
} // namespace Private
} // namespace Zhele

#include "impl/usart.h"
