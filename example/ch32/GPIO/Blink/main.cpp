#include <zhele/iopins.h>

using Led = Zhele::IO::Pd2;

static void delay() {
    for (unsigned i = 1'000'000u; i != 0u; --i) {
        __asm volatile ("nop");
    }
}

int main() {
    Led::Port::Enable();
    Led::SetConfiguration<Led::Configuration::Out>();
    Led::SetDriverType<Led::DriverType::PushPull>();

    for(;;) {
        Led::Toggle();
        delay();
    }
}
