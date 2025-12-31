/**
 * @file
 * @brief Template functions for enum bitwise operations
 *
 * @details
 * DECLARE_VIEWS_IOTA is used to declare incrementable_traits
 * to use enum type in to std::views::iota(Enum::Null, Enum::Size)
 *
 * @author Konstantin Chizhov / X-Ray
 * @date 2025
 * @license FreeBSD
 */

#pragma once

#include <utility>

template <typename E> concept Enum = std::is_enum_v<E>;

#if __cpp_lib_to_underlying >= 202102L
using std::to_underlying;
#else
template <Enum E>
[[nodiscard]] constexpr auto to_underlying(E e) noexcept { return static_cast<std::underlying_type_t<E>>(e); }
#endif

template <Enum E>
[[nodiscard]] constexpr auto operator+(E left) noexcept { return to_underlying(left); }

template <Enum E>
[[nodiscard]] constexpr E& operator++(E& e) noexcept { return e = static_cast<E>(+e + 1); }
template <Enum E>
[[nodiscard]] constexpr E operator++(E& e, int) noexcept { return std::exchange(e, static_cast<E>(+e + 1)); }
template <Enum E>
[[nodiscard]] constexpr E& operator--(E& e) noexcept { return e = static_cast<E>(+e - 1); }
template <Enum E>
[[nodiscard]] constexpr E operator--(E& e, int) noexcept { return std::exchange(e, static_cast<E>(+e - 1)); }

#define DECLARE_VIEWS_IOTA(ENUM)                                        \
template <> struct std::incrementable_traits<ENUM> {                \
      using difference_type = make_signed_t<underlying_type_t<ENUM>>; \
};

template <Enum E>
[[nodiscard]] constexpr E operator&(E left, E right) noexcept { return static_cast<E>(+left & +right); }
template <Enum E>
[[nodiscard]] constexpr E operator^(E left, E right) noexcept { return static_cast<E>(+left ^ +right); }
template <Enum E>
[[nodiscard]] constexpr E operator|(E left, E right) noexcept { return static_cast<E>(+left | +right); }
template <Enum E>
[[nodiscard]] constexpr E operator~(E left) noexcept { return static_cast<E>(~+left); }
template <Enum E>
[[nodiscard]] constexpr const E& operator&=(E& left, E right) noexcept { return left = left & right; }
template <Enum E>
[[nodiscard]] constexpr const E& operator^=(E& left, E right) noexcept { return left = left ^ right; }
template <Enum E>
[[nodiscard]] constexpr const E& operator|=(E& left, E right) noexcept { return left = left | right; }

namespace Zhele {
template <Enum Enum>
constexpr bool HasAllFlags(Enum value, Enum flags) noexcept { return (+value & +flags) == +flags; }

template <Enum Enum>
constexpr bool HasAnyFlag(Enum value, Enum flags) noexcept { return (+value & +flags) != 0; }
} // namespace Zhele

#if TEST
namespace Test {

enum class E : signed char {
  A,
  B,
  C,
  D
};

static_assert((E::B & E::C) == E::A);
static_assert((E::B ^ E::C) == E::D);
static_assert((E::B | E::C) == E::D);
static_assert(~E::A == static_cast<E>(-1));
static_assert(Zhele::HasAllFlags(E::D, E::B | E::C));
static_assert(Zhele::HasAnyFlag(E::D, E::C));

static_assert([](E e) consteval { return e++; }(E::B) == E::B);
static_assert([](E e) consteval { return e--; }(E::B) == E::B);
static_assert([](E e) consteval { return ++e; }(E::B) == E::C);
static_assert([](E e) consteval { return --e; }(E::B) == E::A);
static_assert([](E e) consteval { auto _ = e++; return e; }(E::B) == E::C);
static_assert([](E e) consteval { auto _ = e--; return e; }(E::B) == E::A);

} // namespace Test
#endif
