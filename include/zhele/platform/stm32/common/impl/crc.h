/**
 * @file
 * @brief Implements CRC32
 * @author Alezey Zhelonkin
 * @date 2024
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IMPL_CRC_H
#define ZHELE_PLATFORM_STM32_COMMON_IMPL_CRC_H

#include <stdint.h>
#include <limits>
#include <type_traits>

namespace Zhele::Private
{
#if defined (CRC_POL_POL)
    template<typename _Clock>
    inline void Crc32<_Clock>::SetPolynomSize(PolynomSize polynomSize)
    {
        CRC->CR = (CRC->CR & ~CRC_CR_POLYSIZE_Msk) | (static_cast<uint32_t>(polynomSize));
    }

    template<typename _Clock>
    inline void Crc32<_Clock>::SetPolynom(auto polynom)
    {
        CRC->POL = polynom;
    }

    template<typename _Clock>
    inline uint32_t Crc32<_Clock>::CalculateCrc32(uint32_t polynom, const uint8_t* data, unsigned size)
    {
        SetPolynom(polynom);
        return CalculateCrc32(data, size);
    }
#else

#endif

#if defined (CRC_INIT_INIT)
    template<typename _Clock>
    inline void Crc32<_Clock>::SetInitialValue(uint32_t initialValue)
    {
        CRC->INIT = initialValue;
    }
#endif

    template<typename _Clock>
    inline void Crc32<_Clock>::Enable()
    {
        _Clock::Enable();

    #if defined (CRC_CR_REV_IN) && defined (CRC_CR_REV_OUT)
        CRC->CR |= (0b11 << CRC_CR_REV_IN_Pos) | CRC_CR_REV_OUT;
    #endif
    }

    template<typename _Clock>
    inline uint32_t Crc32<_Clock>::GetPolynom()
    {
    #if defined (CRC_POL_POL)
        return CRC->POL;
    #else
        return 0x4c11db7;
    #endif
    }

    template<typename _Clock>
    inline uint32_t Crc32<_Clock>::GetInitialValue()
    {
    #if defined (CRC_INIT_INIT)
        return CRC->INIT;
    #else
        return 0xffffffff;
    #endif
    }

    /// Little-endian word at any alignment (Cortex-M0 faults on unaligned word loads)
    inline uint32_t LoadWord(const uint8_t* data)
    {
        return uint32_t(data[0]) | uint32_t(data[1]) << 8 | uint32_t(data[2]) << 16 | uint32_t(data[3]) << 24;
    }

    /// The last 1..3 bytes as a little-endian word, without reading past the buffer
    inline uint32_t LoadTail(const uint8_t* data, unsigned count)
    {
        uint32_t word = 0;
        for (unsigned i = 0; i < count; ++i)
            word |= uint32_t(data[i]) << (8 * i);
        return word;
    }

    template<typename _Clock>
    inline uint32_t Crc32<_Clock>::CalculateCrc32(const uint8_t* data, unsigned size)
    {
        Reset();
        const unsigned words = size / sizeof(uint32_t);
        const unsigned tail = size % sizeof(uint32_t);

#if defined (CRC_CR_REV_IN) && defined (CRC_CR_REV_OUT)
        if ((reinterpret_cast<uintptr_t>(data) & 3u) == 0) {
            const uint32_t* aligned = reinterpret_cast<const uint32_t*>(data);
            for (unsigned i = 0; i < words; ++i)
                CRC->DR = aligned[i];
        } else {
            for (unsigned i = 0; i < words; ++i)
                CRC->DR = LoadWord(data + i * sizeof(uint32_t));
        }

        uint32_t result = CRC->DR;
        if (tail) {
            const unsigned bits_remain = tail * std::numeric_limits<uint8_t>::digits;
            CRC->DR = CRC->DR;
            CRC->DR = (LoadTail(data + words * sizeof(uint32_t), tail) ^ result) << (32 - bits_remain);
            result = (result >> bits_remain) ^ CRC->DR;
        }
#else
        if ((reinterpret_cast<uintptr_t>(data) & 3u) == 0) {
            const uint32_t* aligned = reinterpret_cast<const uint32_t*>(data);
            for (unsigned i = 0; i < words; ++i)
                CRC->DR = __RBIT(aligned[i]);
        } else {
            for (unsigned i = 0; i < words; ++i)
                CRC->DR = __RBIT(LoadWord(data + i * sizeof(uint32_t)));
        }

        uint32_t result = __RBIT(CRC->DR);
        if (tail) {
            const unsigned bits_remain = tail * std::numeric_limits<uint8_t>::digits;
            CRC->DR = CRC->DR;
            CRC->DR = __RBIT(LoadTail(data + words * sizeof(uint32_t), tail) ^ result) >> (32 - bits_remain);
            result = (result >> bits_remain) ^ __RBIT(CRC->DR);
        }
#endif

        return ~result;
    }

    template<typename _Clock>
    inline void Crc32<_Clock>::Reset()
    {
        CRC->CR |= CRC_CR_RESET;
        while(CRC->CR & CRC_CR_RESET) continue;
    }

    template<typename _Clock>
    inline void Crc32<_Clock>::SetIDR(std::remove_cv_t<decltype(CRC_TypeDef::IDR)> data)
    {
        CRC->IDR = data;
    }

    template<typename _Clock>
    inline std::remove_cv_t<decltype(CRC_TypeDef::IDR)> Crc32<_Clock>::GetIDR()
    {
        return CRC->IDR;
    }
}

#endif //! ZHELE_PLATFORM_STM32_COMMON_IMPL_CRC_H