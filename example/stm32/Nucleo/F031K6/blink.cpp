// NUCLEO-F031K6: blink LD3 (PB3) from the TIM3 update interrupt, HSI 8 MHz
#include <zhele/iopins.h>
#include <zhele/timer.h>

using namespace Zhele::IO;
using namespace Zhele::Timers;

using Led = Pb3;

int main()
{
    Led::Port::Enable();
    Led::SetConfiguration(Led::Configuration::Out);
    Led::SetDriverType(Led::DriverType::PushPull);
    Led::Set();

    Timer3::Enable();
    Timer3::SetPrescaler(8000 - 1);   // 8 MHz / 8000 = 1 kHz
    Timer3::SetPeriod(500 - 1);       // 2 Hz update -> 1 Hz blink
    Timer3::EnableInterrupt();
    Timer3::Start();

    for (;;)
    {
    }
}

extern "C" void TIM3_IRQHandler()
{
    Led::Toggle();
    Timer3::ClearInterruptFlag();
}
