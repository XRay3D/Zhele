// NUCLEO-F031K6 self-check. Runs on HSI 8 MHz, reports over the ST-LINK VCP (USART1 PA2/PA15,
// 115200 8N1) one line per peripheral: "<name>: PASS" or "<name>: FAIL <detail>". After "done"
// it echoes every received byte, so the host can verify RX as well.
//
// The same source builds against master, stm32-cxx26 and stm32-token-injection: the span API is
// exercised only where the library provides it (detected with a requires-expression).
#include <zhele/crc.h>
#include <zhele/dma.h>
#include <zhele/exti.h>
#include <zhele/flash.h>
#include <zhele/iopins.h>
#include <zhele/timer.h>
#include <zhele/usart.h>

#include <cstdint>
#include <cstring>
#include <span>

using namespace Zhele;
using namespace Zhele::IO;

using Led = Pb3;
using Vcp = Usart1;

namespace
{
    volatile uint32_t timerTicks = 0;
    volatile uint32_t extiHits = 0;
    volatile uint32_t extiPendingOther = 0; // EXTI->PR line 1 as seen inside the line-0 handler

    void Print(const char* text)
    {
        Vcp::Write(text, std::strlen(text));
    }

    void PrintHex(uint32_t value)
    {
        char buffer[11] = "0x";
        for (int i = 0; i < 8; ++i)
            buffer[2 + i] = "0123456789ABCDEF"[(value >> (28 - 4 * i)) & 0xF];
        buffer[10] = 0;
        Print(buffer);
    }

    void Report(const char* name, bool ok, const char* detail = nullptr)
    {
        Print(name);
        Print(ok ? ": PASS" : ": FAIL");
        if (detail && !ok) { Print(" "); Print(detail); }
        Print("\r\n");
    }

    void SpinDelay(uint32_t loops)
    {
        for (volatile uint32_t i = 0; i < loops; ++i) { }
    }

    // Standard CRC-32 (reflected 0x04C11DB7, init and final XOR 0xFFFFFFFF) bit by bit; "123456789"
    // gives 0xCBF43926. Zhele::Crc configures the CRC unit (REV_IN/REV_OUT) to match it.
    uint32_t SoftCrc32(const uint8_t* data, unsigned size)
    {
        uint32_t crc = 0xFFFFFFFFu;
        for (unsigned i = 0; i < size; ++i)
        {
            crc ^= data[i];
            for (int b = 0; b < 8; ++b)
                crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
        }
        return ~crc;
    }

    bool TestGpio()
    {
        Led::Port::Enable();
        Led::SetConfiguration(Led::Configuration::Out);
        Led::SetDriverType(Led::DriverType::PushPull);
        bool ok = true;
        for (int i = 0; i < 6; ++i)
        {
            Led::Toggle();
            ok = ok && (Led::IsSet() == ((i & 1) == 0));
            SpinDelay(60000);
        }
        Led::Clear();
        return ok;
    }

    bool TestDma()
    {
        static char source[] = "Zhele DMA mem2mem on F031K6";
        static char destination[sizeof(source)];
        std::memset(destination, 0, sizeof(destination));

        Dma1Channel1::Transfer(
            Dma1Channel1::Mem2Mem | Dma1Channel1::MSize8Bits | Dma1Channel1::PSize8Bits
                | Dma1Channel1::MemIncrement | Dma1Channel1::PeriphIncrement,
            destination, source, sizeof(source));

        for (uint32_t i = 0; i < 100000 && !Dma1Channel1::TransferComplete(); ++i) { }
        const bool complete = Dma1Channel1::TransferComplete();
        Dma1Channel1::ClearFlags();
        return complete && std::memcmp(source, destination, sizeof(source)) == 0;
    }

    bool TestCrc(uint32_t& hardware, uint32_t& software)
    {
        alignas(4) static const uint8_t vector[] = "123456789";
        Crc::Enable();
        hardware = Crc::CalculateCrc32(vector, 9);
        software = SoftCrc32(vector, 9);
        return hardware == software;
    }

    bool TestTimer()
    {
        using namespace Zhele::Timers;
        Timer3::Enable();
        Timer3::SetPrescaler(8 - 1);     // 1 MHz
        Timer3::SetPeriod(1000 - 1);     // 1 kHz update
        Timer3::EnableInterrupt();
        Timer3::Start();
        SpinDelay(40000);                // ~ tens of ms
        Timer3::Stop();
        Timer3::DisableInterrupt();
        return timerTicks >= 5;
    }

    // Lines 0 and 1 share EXTI0_1_IRQn. Both are armed and triggered by software in one write while
    // the NVIC line is masked, so the handler enters with both pending. It clears line 0 through
    // Zhele; because EXTI->PR is rc_w1 that must leave line 1 pending (a read-modify-write would
    // wipe it). The handler records that and then clears line 1 itself.
    bool TestExti()
    {
        Exti0::Init(Exti0::Trigger::Rising, 'A');
        Exti1::Init(Exti1::Trigger::Rising, 'A');
        Exti0::EnableInterrupt();
        Exti1::EnableInterrupt();

        NVIC_DisableIRQ(EXTI0_1_IRQn);
        EXTI->SWIER = (1u << 1) | (1u << 0);
        NVIC_EnableIRQ(EXTI0_1_IRQn);
        SpinDelay(2000);

        Exti0::DisableInterrupt();
        Exti1::DisableInterrupt();
        return extiHits == 1 && extiPendingOther == 1;
    }

    volatile bool dmaRxDone = false;
    volatile unsigned dmaRxSize = 0;

    void DmaRxCallback(void*, unsigned size, bool success)
    {
        dmaRxSize = size;
        dmaRxDone = success;
    }

    void WaitTxIdle()
    {
        while (!(USART1->ISR & USART_ISR_TC)) { }
    }

    // Transmit through USART1 TX DMA (channel 2); the host sees the text in the log
    bool TestUsartDmaTx()
    {
        static const char text[] = "usart_dma_tx payload\r\n";
        Vcp::WriteAsync(text, sizeof(text) - 1);
        for (uint32_t i = 0; i < 200000 && !Vcp::DmaTx::TransferComplete(); ++i) { }
        const bool complete = Vcp::DmaTx::TransferComplete();
        WaitTxIdle();
        USART1->CR3 &= ~USART_CR3_DMAT;
        return complete;
    }

    // Receive 4 bytes ("abcd") from the host through USART1 RX DMA (channel 3) with a callback
    bool TestUsartDmaRx(const char*& detail)
    {
        static uint8_t buffer[4];
        while (Vcp::ReadReady()) (void)Vcp::Read();
        USART1->ICR = USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_NCF | USART_ICR_PECF;

        Vcp::EnableAsyncRead(buffer, sizeof(buffer), DmaRxCallback);
        Print("usart_dma_rx: send 4 bytes\r\n");
        for (uint32_t i = 0; i < 4000000 && !dmaRxDone; ++i) { }
        USART1->CR3 &= ~USART_CR3_DMAR;

        if (!dmaRxDone) { detail = "timeout"; return false; }
        if (dmaRxSize != 4) { detail = "size"; return false; }
        if (std::memcmp(buffer, "abcd", 4) != 0) { detail = "data"; return false; }
        return true;
    }

    template<typename U, typename C>
    concept HasSpanApi = requires(std::span<const uint8_t> bytes) { U::Write(bytes); C::CalculateCrc32(bytes); };

    // span overloads: USART write, CRC and DMA transfer (a template, so the branch is discarded
    // where the library has no span API)
    template<typename U = Vcp, typename C = Crc, typename D = Dma1Channel1>
    int TestSpanApi()
    {
        if constexpr (HasSpanApi<U, C>)
        {
            static const uint8_t vector[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
            const bool crcOk = C::CalculateCrc32(std::span<const uint8_t>(vector)) == 0xCBF43926u;

            static uint32_t source[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
            static uint32_t destination[8];
            D::Transfer(
                D::Mem2Mem | D::MSize32Bits | D::PSize32Bits
                    | D::MemIncrement | D::PeriphIncrement,
                std::span<uint32_t>(destination), source);
            for (uint32_t i = 0; i < 100000 && !D::TransferComplete(); ++i) { }
            D::ClearFlags();
            const bool dmaOk = std::memcmp(source, destination, sizeof(source)) == 0;

            static const uint8_t line[] = { 's', 'p', 'a', 'n', ' ', 'w', 'r', 'i', 't', 'e', '\r', '\n' };
            U::Write(std::span<const uint8_t>(line));
            return crcOk && dmaOk ? 1 : 0;
        }
        else
        {
            return -1;
        }
    }

    // TIM3 CH1 PWM on PA6 (8 MHz / 100 = 80 kHz, 25 % duty); the pin is sampled through IDR
    bool TestPwm(unsigned& dutyPercent)
    {
        using namespace Zhele::Timers;
        using Pwm = Timer3::PWMGeneration<0>;

        Timer3::Enable();
        Timer3::Stop();
        Timer3::SetPrescaler(0);
        Timer3::ResetCounterValue();          // TestTimer left CNT above the new ARR
        Timer3::SetPeriodAndUpdate(100 - 1);  // UG also loads the preloaded PSC
        Pwm::SelectPins<Pa6>();
        Pwm::Enable();
        Pwm::SetOutputMode(Pwm::OutputMode::PWM1);
        Pwm::SetPulse(25);
        Timer3::Start();

        unsigned high = 0;
        constexpr unsigned samples = 4000;
        for (unsigned i = 0; i < samples; ++i)
            high += Pa6::IsSet() ? 1 : 0;

        Timer3::Stop();
        dutyPercent = high * 100 / samples;
        return dutyPercent >= 15 && dutyPercent <= 35;
    }

    // Same vector at an odd address: a word load from it faults on Cortex-M0 (run last on purpose)
    bool TestCrcUnaligned()
    {
        alignas(4) static uint8_t buffer[12];
        std::memcpy(buffer + 1, "123456789", 9);
        return Crc::CalculateCrc32(buffer + 1, 9) == 0xCBF43926u;
    }

    bool TestFlash(const char*& detail)
    {
        const unsigned page = Flash::PageCount() - 1; // last 1 KB page of the 32 KB F031K6
        static const uint32_t pattern[] = { 0x5A5AA5A5u, 0x01234567u, 0x89ABCDEFu, 0xDEADBEEFu };
        auto* address = reinterpret_cast<const uint32_t*>(Flash::PageAddress(page));

        static char pages[] = "page count 00";
        pages[11] = '0' + Flash::PageCount() / 10 % 10;
        pages[12] = '0' + Flash::PageCount() % 10;
        if (Flash::PageCount() != 32) { detail = pages; return false; }
        if (!Flash::ErasePage(page)) { detail = "erase"; return false; }
        for (unsigned i = 0; i < 4; ++i)
            if (address[i] != 0xFFFFFFFFu) { detail = "not blank after erase"; return false; }
        if (!Flash::WriteFlash(const_cast<uint32_t*>(address), pattern, sizeof(pattern))) { detail = "program"; return false; }
        if (std::memcmp(address, pattern, sizeof(pattern)) != 0) { detail = "verify"; return false; }
        return true;
    }
}

int main()
{
    Vcp::Init(115200);
    Vcp::SelectTxRxPins<Pa2, Pa15>();

    Print("\r\nhwtest start\r\n");

    Report("gpio", TestGpio());
    Report("dma", TestDma());

    uint32_t hw = 0, sw = 0;
    const bool crcOk = TestCrc(hw, sw);
    Print("crc: "); Print(crcOk ? "PASS " : "FAIL "); PrintHex(hw); Print(" "); PrintHex(sw); Print("\r\n");

    Report("timer", TestTimer());
    Report("exti", TestExti(), extiHits == 1 ? "line 1 pending flag was cleared" : "no interrupt");

    const char* detail = nullptr;
    Report("flash", TestFlash(detail), detail);

    Report("usart_dma_tx", TestUsartDmaTx());
    detail = nullptr;
    Report("usart_dma_rx", TestUsartDmaRx(detail), detail);

    const int span = TestSpanApi();
    if (span < 0)
        Print("span: SKIP (no span API)\r\n");
    else
        Report("span", span == 1);

    unsigned duty = 0;
    const bool pwmOk = TestPwm(duty);
    char dutyText[] = "duty 00%";
    dutyText[5] = '0' + duty / 10 % 10;
    dutyText[6] = '0' + duty % 10;
    Print("pwm: "); Print(pwmOk ? "PASS " : "FAIL "); Print(dutyText); Print("\r\n");

    Print("crc_unaligned: ...\r\n");
    Report("crc_unaligned", TestCrcUnaligned());

    Print("done\r\n");

    for (;;)
    {
        if (Vcp::ReadReady())
        {
            const uint8_t byte = Vcp::Read();
            Vcp::Write(byte);
            Led::Toggle();
        }
    }
}

extern "C"
{
    void TIM3_IRQHandler()
    {
        ++timerTicks;
        Zhele::Timers::Timer3::ClearInterruptFlag();
    }

    void DMA1_Channel2_3_IRQHandler()
    {
        Vcp::DmaTx::IrqHandler();
        Vcp::DmaRx::IrqHandler();
    }

    void EXTI0_1_IRQHandler()
    {
        ++extiHits;
        const bool bothPending = (EXTI->PR & 3u) == 3u;
        Exti0::ClearInterruptFlag();
        if (bothPending && (EXTI->PR & (1u << 1)))
            extiPendingOther = 1;
        EXTI->PR = 1u << 1;
    }
}
