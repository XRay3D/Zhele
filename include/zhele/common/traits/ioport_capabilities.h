/**
 * @file
 * Optional `Speed` type alias via empty base class — used by TPin and PinList
 *
 * @license MIT
 */

#pragma once

#include <type_traits>

namespace Zhele::IO::detail {
template <typename _Port, typename = void>
struct IoSpeed { };
template <typename _Port>
struct IoSpeed<_Port, std::void_t<typename _Port::Speed>> {
    using Speed = typename _Port::Speed;
};

template <typename _Port, typename = void>
struct IoDriverType { };
template <typename _Port>
struct IoDriverType<_Port, std::void_t<typename _Port::DriverType>> {
    using DriverType = typename _Port::DriverType;
};

} // namespace Zhele::IO::detail
