/**
 * @file
 * Contains instances of gpio pins (as typedefs)
 * 
 * @author Konstantin Chizhov
 * @date 2010
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IOPINS_H
#define ZHELE_PLATFORM_STM32_COMMON_IOPINS_H

#include <zhele/common/iopin.h>

#include <zhele/ioports.h>

#include <meta>
#include <string_view>

namespace Zhele::IO
{
    using NullPin = Zhele::IO::TPin<NullPort, 0>;
    // Pa0..Ph15 and Pa0Inv..Ph15Inv; a port that the device lacks maps to NullPort
    consteval
    {
        for (char letter = 'A'; letter <= 'H'; ++letter)
        {
            const char lower[] = { char(letter - 'A' + 'a'), 0 };
            const char portName[] = { 'P', 'o', 'r', 't', lower[0], 0 };
            std::meta::info port = ^^NullPort;
            for (std::meta::info member : std::meta::members_of(^^Zhele::IO, std::meta::access_context::unchecked()))
                if (std::meta::has_identifier(member) && std::meta::identifier_of(member) == std::string_view(portName))
                    port = member;

            for (unsigned pin = 0; pin < 16; ++pin)
            {
                std::meta::queue_injection(^^{ using \["P", lower, pin] = TPin<typename \[:port:], \val(pin)>; });
                std::meta::queue_injection(^^{ using \["P", lower, pin, "Inv"] = InvertedPin<typename \[:port:], \val(pin)>; });
            }
        }
    }
}

#endif // !ZHELE_PLATFORM_STM32_COMMON_IOPINS_H