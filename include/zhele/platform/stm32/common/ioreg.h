/**
 * @file
 * @brief Register access templates: compile-time addressed registers,
 * peripheral struct wrappers, bit fields and Cortex-M3/M4 bit-banding
 * @author Konstantin Chizhov
 * @date 2018
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IOREG_H
#define ZHELE_PLATFORM_STM32_COMMON_IOREG_H

#include <bit>
#include <cstddef>
#include <cstdint>
#include <meta>
#include <type_traits>

namespace Zhele
{
    namespace Private
    {
        /// Peripheral bit-band window (Cortex-M3/M4). Only the first 1 MB of the peripheral
        /// region is aliased; CMSIS defines PERIPH_BB_BASE exactly for the cores that have it.
    #if defined(PERIPH_BB_BASE)
        inline constexpr bool bitBandSupported = true;
        inline constexpr uintptr_t bitBandRegionBase = PERIPH_BASE;
        inline constexpr uintptr_t bitBandAliasBase = PERIPH_BB_BASE;
    #else
        inline constexpr bool bitBandSupported = false;
        inline constexpr uintptr_t bitBandRegionBase = 0;
        inline constexpr uintptr_t bitBandAliasBase = 0;
    #endif

        constexpr bool InBitBandRegion(uintptr_t address)
        {
            return bitBandSupported
                && address >= bitBandRegionBase
                && address < bitBandRegionBase + 0x100000u;
        }

        constexpr uintptr_t BitBandAlias(uintptr_t address, unsigned bit)
        {
            return bitBandAliasBase + (address - bitBandRegionBase) * 32u + bit * 4u;
        }

        /// Type of a reflected data member with cv-qualifiers (CMSIS `__IO`) and array extents removed
        template<std::meta::info _Member>
        using MemberElementType = std::remove_all_extents_t<std::remove_cv_t<typename [:std::meta::type_of(_Member):]>>;

        /**
         * @brief Operations shared by every register wrapper.
         *
         * @tparam _Reg Wrapper providing the primitives `Get`, `Set`, `Or`, `And`, `Xor`.
         * The primitives never hand out a reference to the register: once the address is folded
         * into one constant, GCC cannot share the peripheral base pointer between neighbouring
         * registers, which costs a literal-pool entry per register on Thumb-1.
         */
        template<typename _Reg, typename _DataType, uintptr_t _Address>
        class RegisterOps
        {
        public:
            using DataT = _DataType;
            static constexpr uintptr_t Address = _Address;
            /// True when single-bit writes go through the bit-band alias (no read-modify-write)
            static constexpr bool BitBand = InBitBandRegion(_Address);

            static void AndOr(DataT andMask, DataT orMask) { _Reg::Set((_Reg::Get() & andMask) | orMask); }

            template<unsigned Bit>
            static bool IsBitSet() { return BitValue<Bit>(); }
            template<unsigned Bit>
            static bool IsBitClear() { return !BitValue<Bit>(); }

            /**
             * @brief Read one bit
             */
            template<unsigned Bit>
            static bool BitValue()
            {
                static_assert(Bit < sizeof(DataT) * 8);
                if constexpr (BitBand)
                    return *reinterpret_cast<volatile uint32_t*>(BitBandAlias(_Address, Bit)) != 0;
                else
                    return (_Reg::Get() & (DataT(1) << Bit)) != 0;
            }

            /**
             * @brief Write one bit (a single store on bit-band capable cores)
             */
            template<unsigned Bit>
            static void WriteBit(bool value)
            {
                static_assert(Bit < sizeof(DataT) * 8);
                if constexpr (BitBand)
                    *reinterpret_cast<volatile uint32_t*>(BitBandAlias(_Address, Bit)) = value;
                else if (value)
                    _Reg::Or(DataT(1) << Bit);
                else
                    _Reg::And(DataT(~(DataT(1) << Bit)));
            }

            template<unsigned Bit>
            static void SetBit() { WriteBit<Bit>(true); }

            template<unsigned Bit>
            static void ClearBit() { WriteBit<Bit>(false); }

            /**
             * @brief Set bits by mask; a single bit becomes a bit-band store where supported
             */
            template<DataT Mask>
            static void SetBits()
            {
                if constexpr (std::has_single_bit(Mask) && BitBand)
                    SetBit<std::countr_zero(Mask)>();
                else
                    _Reg::Or(Mask);
            }

            /**
             * @brief Clear bits by mask; a single bit becomes a bit-band store where supported
             */
            template<DataT Mask>
            static void ClearBits()
            {
                if constexpr (std::has_single_bit(Mask) && BitBand)
                    ClearBit<std::countr_zero(Mask)>();
                else
                    _Reg::And(DataT(~Mask));
            }
        };
    }

    /**
     * @brief Register at a compile-time address
     *
     * @tparam _Address Register address
     * @tparam _DataType Register data type
     */
    template<uintptr_t _Address, typename _DataType>
    class IoReg : public Private::RegisterOps<IoReg<_Address, _DataType>, _DataType, _Address>
    {
    public:
        static volatile _DataType& Ref() { return *reinterpret_cast<volatile _DataType*>(_Address); }

        static _DataType Get() { return Ref(); }
        static void Set(_DataType value) { Ref() = value; }
        static void Or(_DataType value) { Ref() |= value; }
        static void And(_DataType value) { Ref() &= value; }
        static void Xor(_DataType value) { Ref() ^= value; }
    };

    /**
     * @brief Register that is a member of a CMSIS peripheral struct, e.g.
     * `PeriphReg<RCC_BASE, ^^RCC_TypeDef::APB2ENR>`.
     *
     * Every access is spelled `base->member`, so neighbouring registers of one block share the
     * base pointer in generated code (as `RCC->APB2ENR` does). The address is still a
     * compile-time constant, which is what bit-banding needs.
     *
     * @tparam _Base Peripheral base address
     * @tparam _Member Reflection of the register field
     * @tparam _Index Element index for array fields (AFR, EXTICR, ...)
     */
    template<uintptr_t _Base, std::meta::info _Member, std::size_t _Index = 0>
    class PeriphReg : public Private::RegisterOps<
        PeriphReg<_Base, _Member, _Index>,
        Private::MemberElementType<_Member>,
        _Base + std::meta::offset_of(_Member).bytes + _Index * sizeof(Private::MemberElementType<_Member>)>
    {
        using Struct = typename [:std::meta::parent_of(_Member):];
        static constexpr bool IsArray = std::is_array_v<typename [:std::meta::type_of(_Member):]>;
        static Struct* Base() { return reinterpret_cast<Struct*>(_Base); }
    public:
        using DataT = Private::MemberElementType<_Member>;

        static DataT Get()
        {
            if constexpr (IsArray) return Base()->[:_Member:][_Index]; else return Base()->[:_Member:];
        }
        static void Set(DataT value)
        {
            if constexpr (IsArray) Base()->[:_Member:][_Index] = value; else Base()->[:_Member:] = value;
        }
        static void Or(DataT value)
        {
            if constexpr (IsArray) Base()->[:_Member:][_Index] |= value; else Base()->[:_Member:] |= value;
        }
        static void And(DataT value)
        {
            if constexpr (IsArray) Base()->[:_Member:][_Index] &= value; else Base()->[:_Member:] &= value;
        }
        static void Xor(DataT value)
        {
            if constexpr (IsArray) Base()->[:_Member:][_Index] ^= value; else Base()->[:_Member:] ^= value;
        }
    };

    /**
     * @brief Peripheral register block at a compile-time address
     *
     * @tparam _Address Block base address
     * @tparam _StructType CMSIS `*_TypeDef`
     */
    template<uintptr_t _Address, typename _StructType>
    class IoStruct
    {
    public:
        using DataT = _StructType;
        static constexpr uintptr_t Address = _Address;

        static DataT* Get() { return reinterpret_cast<DataT*>(_Address); }
        DataT* operator->() const { return Get(); }

        /// Register of this block: `Regs::Reg<^^TIM_TypeDef::CCMR1>`
        template<std::meta::info _Member, std::size_t _Index = 0>
        using Reg = PeriphReg<_Address, _Member, _Index>;

        /// Set bits of a register: `Regs::SetBits<^^TIM_TypeDef::CR1, TIM_CR1_CEN>()`
        template<std::meta::info _Member, uint32_t _Mask>
        static void SetBits() { Reg<_Member>::template SetBits<_Mask>(); }

        /// Clear bits of a register: `Regs::ClearBits<^^TIM_TypeDef::CR1, TIM_CR1_CEN>()`
        template<std::meta::info _Member, uint32_t _Mask>
        static void ClearBits() { Reg<_Member>::template ClearBits<_Mask>(); }
    };

    /**
     * @brief Bit field inside a register
     *
     * @tparam _Reg Register (IoReg / PeriphReg)
     * @tparam _Offset Field offset in bits
     * @tparam _Length Field length in bits
     * @tparam _DataType Type the field is exposed as
     */
    template<typename _Reg, unsigned _Offset, unsigned _Length, typename _DataType = typename _Reg::DataT>
    class IoBitField
    {
    public:
        using DataT = _DataType;
        using RegT = typename _Reg::DataT;
        using Reg = _Reg;
        static constexpr unsigned Offset = _Offset;
        static constexpr unsigned Length = _Length;
        static constexpr RegT Mask = _Length >= sizeof(RegT) * 8 ? RegT(~RegT(0)) : RegT((RegT(1) << _Length) - 1);
        static constexpr unsigned MaxValue = Mask;

        static DataT Get() { return static_cast<DataT>((_Reg::Get() >> _Offset) & Mask); }
        static void Set(DataT value) { _Reg::AndOr(RegT(~(Mask << _Offset)), (static_cast<RegT>(value) & Mask) << _Offset); }
    };

    namespace Private
    {
        template<uint32_t _Mask>
        consteval unsigned FieldLength()
        {
            constexpr uint32_t shifted = _Mask >> std::countr_zero(_Mask);
            static_assert((shifted & (shifted + 1)) == 0, "Bitfield mask must be contiguous");
            return std::popcount(_Mask);
        }
    }

    /**
     * @brief Bit field described by a CMSIS `*_Msk` value: `MaskedBitField<Reg, RCC_CFGR_HPRE_Msk>`
     */
    template<typename _Reg, uint32_t _Mask, typename _DataType = uint32_t>
    using MaskedBitField = IoBitField<_Reg, std::countr_zero(_Mask), Private::FieldLength<_Mask>(), _DataType>;

    /**
     * @brief Dummy register: writes are ignored, reads return zero
     */
    template<typename _DataType = unsigned char>
    class NullReg
    {
    public:
        using DataT = _DataType;
        static DataT Get() { return DataT(0); }
        static void Set(DataT) {}
        static void Or(DataT) {}
        static void And(DataT) {}
        static void Xor(DataT) {}
        static void AndOr(DataT, DataT) {}
        template<unsigned Bit>
        static bool IsBitSet() { return false; }
        template<unsigned Bit>
        static bool IsBitClear() { return true; }
        template<unsigned Bit>
        static bool BitValue() { return false; }
        template<unsigned Bit>
        static void WriteBit(bool) {}
        template<unsigned Bit>
        static void SetBit() {}
        template<unsigned Bit>
        static void ClearBit() {}
        template<DataT Mask>
        static void SetBits() {}
        template<DataT Mask>
        static void ClearBits() {}
    };

    /**
     * @brief Single bit of a register at a compile-time address
     */
    template<uintptr_t _RegAddr, typename _DataType, unsigned _BitfieldOffset>
    class IoBit
    {
        using Reg = IoReg<_RegAddr, _DataType>;
    public:
        static volatile _DataType& Value() { return Reg::Ref(); }
        static bool IsSet() { return Reg::template BitValue<_BitfieldOffset>(); }
        static void Set() { Reg::template SetBit<_BitfieldOffset>(); }
        static void Clear() { Reg::template ClearBit<_BitfieldOffset>(); }
    };

    /**
     * @brief Bit field length (for a contiguous, right-aligned mask) at compile time
     */
    template<uint32_t _Mask>
    inline constexpr uint32_t GetBitFieldLength = Private::FieldLength<_Mask>();
}
#endif //! ZHELE_PLATFORM_STM32_COMMON_IOREG_H
