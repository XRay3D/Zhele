/**
 * @file
 * Optional `Speed` type alias via empty base class — used by TPin and PinList
 *
 * @license MIT
 */

#ifndef ZHELE_COMMON_TRAITS_IO_SPEED_MIXIN_H
#define ZHELE_COMMON_TRAITS_IO_SPEED_MIXIN_H

#include <type_traits>

namespace Zhele::IO::detail {
  template<typename _Port, typename = void> struct IoSpeed {};
  template<typename _Port>
  struct IoSpeed<_Port, std::void_t<typename _Port::Speed>> {
    using Speed = typename _Port::Speed;
  };
} // namespace Zhele::IO::detail

#endif // ZHELE_COMMON_TRAITS_IO_SPEED_MIXIN_H
