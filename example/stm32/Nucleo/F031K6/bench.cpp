// NUCLEO-F031K6 cycle benchmark of library primitives (HSI 8 MHz, 0 wait states).
//
// TIM2 (32-bit) runs at HCLK with prescaler 1, so one tick is one CPU cycle; it is driven through
// raw CMSIS registers so the measurement does not depend on the library under test. Every case runs
// N iterations with interrupts disabled; the cost of an empty loop is subtracted and the best of
// several runs is reported as "bench <name> <cycles per operation, 1 decimal>".
//
// Built unchanged against master, stm32-cxx26 and stm32-token-injection.
#include <zhele/crc.h>
#include <zhele/dma.h>
#include <zhele/exti.h>
#include <zhele/iopins.h>
#include <zhele/timer.h>
#include <zhele/usart.h>

#include <cstdint>
#include <cstring>

using namespace Zhele;
using namespace Zhele::IO;

using Vcp = Usart1;
using Led = Pb3;

namespace
{
    constexpr unsigned Iterations = 256;
    constexpr unsigned Runs = 5;

    void Print(const char* text) { Vcp::Write(text, std::strlen(text)); }

    void PrintUnsigned(uint32_t value)
    {
        char buffer[11];
        int i = 10;
        buffer[i] = 0;
        do { buffer[--i] = '0' + value % 10; value /= 10; } while (value);
        Print(buffer + i);
    }

    void StartCycleCounter()
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
        TIM2->CR1 = 0;
        TIM2->PSC = 0;
        TIM2->ARR = 0xFFFFFFFFu;
        TIM2->EGR = TIM_EGR_UG;
        TIM2->CR1 = TIM_CR1_CEN;
    }

    template<typename F>
    [[gnu::noinline]] uint32_t Measure(F op)
    {
        __disable_irq();
        const uint32_t start = TIM2->CNT;
        for (unsigned i = 0; i < Iterations; ++i)
        {
            op();
            asm volatile("" ::: "memory");
        }
        const uint32_t end = TIM2->CNT;
        __enable_irq();
        return end - start;
    }

    template<typename F>
    uint32_t Best(F op)
    {
        uint32_t best = 0xFFFFFFFFu;
        for (unsigned r = 0; r < Runs; ++r)
        {
            const uint32_t t = Measure(op);
            if (t < best) best = t;
        }
        return best;
    }

    uint32_t loopOverhead = 0;

    template<typename F>
    void Bench(const char* name, F op)
    {
        const uint32_t total = Best(op);
        const uint32_t net = total > loopOverhead ? total - loopOverhead : 0;
        const uint32_t tenths = (net * 10 + Iterations / 2) / Iterations;
        Print("bench "); Print(name); Print(" ");
        PrintUnsigned(tenths / 10); Print("."); PrintUnsigned(tenths % 10); Print("\r\n");
    }

    alignas(4) uint8_t crcData[256];
    uint8_t dmaSource[64];
    uint8_t dmaDestination[64];
}

int main()
{
    Vcp::Init(115200);
    Vcp::SelectTxRxPins<Pa2, Pa15>();
    StartCycleCounter();

    Led::Port::Enable();
    Led::SetConfiguration(Led::Configuration::Out);
    Pa6::Port::Enable();
    Crc::Enable();
    Exti4::Init(Exti4::Trigger::Rising, 'A');
    Timers::Timer3::Enable();
    for (unsigned i = 0; i < sizeof(crcData); ++i) crcData[i] = static_cast<uint8_t>(i * 7 + 3);

    loopOverhead = Best([] {});

    Print("\r\nbench start\r\n");
    Bench("gpio_toggle", [] { Led::Toggle(); });
    Bench("gpio_set_clear", [] { Led::Set(); Led::Clear(); });
    Bench("gpio_set_mode", [] { Pa6::SetConfiguration(Pa6::Configuration::Out); });
    Bench("gpio_set_mode_tpl", [] { Pa6::SetConfiguration<Pa6::Configuration::Out>(); });
    Bench("gpio_altfunc_tpl", [] { Pa6::AltFuncNumber<1>(); });
    Bench("clock_enable_disable", [] { Clock::Tim14Clock::Enable(); Clock::Tim14Clock::Disable(); });
    Timers::Timer3::Enable();
    Bench("timer_start_stop", [] { Timers::Timer3::Start(); Timers::Timer3::Stop(); });
    Bench("timer_set_period", [] { Timers::Timer3::SetPeriod(1000); });
    Bench("exti_irq_enable_disable", [] { Exti4::EnableInterrupt(); Exti4::DisableInterrupt(); });
    Bench("exti_clear_flag", [] { Exti4::ClearInterruptFlag(); });
    Bench("usart_write_ready", [] { (void)Vcp::WriteReady(); });
    Bench("usart_set_baud", [] { Vcp::SetBaud(115200); });
    Bench("crc32_256B", [] { (void)Crc::CalculateCrc32(crcData, sizeof(crcData)); });
    Bench("dma_m2m_64B", [] {
        Dma1Channel1::Transfer(
            Dma1Channel1::Mem2Mem | Dma1Channel1::MSize8Bits | Dma1Channel1::PSize8Bits
                | Dma1Channel1::MemIncrement | Dma1Channel1::PeriphIncrement,
            dmaDestination, dmaSource, sizeof(dmaSource));
        while (!Dma1Channel1::TransferComplete()) { }
        Dma1Channel1::ClearFlags();
    });
    Print("bench done\r\n");

    for (;;)
    {
    }
}

extern "C" void DMA1_Channel1_IRQHandler()
{
    Dma1Channel1::IrqHandler();
}
