/**
 * @file
 * GPIO clock and reset control via PM->CGCFGAHB / PM->RSTDISAHB for MIK32 K1948VK018.
 *
 * Enable()  — gates on the AHB clock and releases the peripheral from reset.
 * Disable() — puts the peripheral back in reset and gates off the clock.
 */
#pragma once

#include "ioreg.h"

namespace Zhele::Clock {
  IO_REG_WRAPPER(PM->CLK_APB_P_SET,  ApbClockEnable,  uint32_t);
  IO_REG_WRAPPER(PM->CLK_APB_P_CLEAR, ApbResetDisable, uint32_t);

  // Both CGCFGAHB and RSTDISAHB use the same bit positions for GPIO (bits 8-10).
  template <uint32_t Mask>
  struct PortClock {
    static void Enable() {
      ApbClockEnable::Or(Mask);
      ApbResetDisable::Or(Mask);
    }
    static void Disable() {
      ApbResetDisable::And(static_cast<uint32_t>(~Mask));
      ApbClockEnable::And(static_cast<uint32_t>(~Mask));
    }
  };

#ifdef PM_CLOCK_APB_P_GPIO_0_M
  using Port0Clock = PortClock<PM_CLOCK_APB_P_GPIO_0_M>;
#endif
#ifdef PM_CLOCK_APB_P_GPIO_1_M
  using Port1Clock = PortClock<PM_CLOCK_APB_P_GPIO_1_M>;
#endif
#ifdef PM_CLOCK_APB_P_GPIO_2_M
  using Port2Clock = PortClock<PM_CLOCK_APB_P_GPIO_2_M>;
#endif

} // namespace Zhele::Clock