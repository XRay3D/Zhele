#include <zhele/iopins.h>
#include <zhele/pinlist.h>

using Leds = Zhele::IO::PinList<
    Zhele::IO::Pd2,
    Zhele::IO::Pa5
>;

int main() {
    Leds::Enable();
    Leds::SetConfiguration<Leds::Configuration::Out>();
    Leds::SetDriverType<Leds::DriverType::PushPull>();

    uint8_t val = 4;
    for(;;) {
        Leds::Write(--val);
        if (val == 0) {
            val = 4;
        }
    }
}
