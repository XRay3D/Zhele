/**
 * @file
 * MIK32 K1948VK018 GPIO: NativePortBase, NullPort, PortImplementation.
 *
 * Register layout (GPIO_TypeDef):
 *   union                            — pin input  (read-only)
 *   {                                — pin output (read-write)
 *       volatile uint32_t SET;       — set output bits   (write-only)
 *       volatile uint32_t STATE;     — clear output bits (write-only)
 *   };                               — toggle output bits (write-only)
 *   volatile uint32_t CLEAR;         — pull-up enable, 1 bit/pin (read-write; 1 = pull-up)
 *   volatile uint32_t DIRECTION_OUT; — output mode, 2 bits/pin  (read-write; 0=push-pull, 1=open-drain, 2=open-source)
 *   volatile uint32_t DIRECTION_IN;  — output-enable set  (read-write)
 *   volatile uint32_t OUTPUT;        — output-enable clear (write-only)
 *   volatile uint32_t CONTROL;       — alt-function enable  (read-write)
 */
#pragma once

#include "../platform_detector.h"

#include <cstdint>
#include <limits>

namespace Zhele::IO {
class NativePortBase {
public:
    using DataType = uint16_t;

    enum Configuration {
        INPUT,        //= 0b100, // Режим вывода - GPIO. Вход.
        OUTPUT,       //= 0b000, // Режим вывода - GPIO. Выход.
        SERIAL,       //= 0b01,  // Режим вывода - последовательный интерфейс.
        TIMER_SERIAL, //= 0b10,  // Режим вывода - последовательный интерфейс или таймер.
        ANALOG,       //= 0b11   // Аналоговый сигнал.
    };

    enum PullMode {
        NONE, //= 0b00, // Подтяжка не подключаются.
        UP,   //= 0b01, // Подтяжка к питанию.
        DOWN, //= 0b10  // Подтяжка к земле.
    };

    enum DSType {
        DS_2MA, //= 0b00, // Режим драйвера 2мА.
        DS_4MA, //= 0b01, // Режим драйвера 4мА.
        DS_8MA, //= 0b10  // Режим драйвера 8мА.
    };

    enum DriverType {
        PushPull,   //= 0,
        OpenDrain,  //= 1,
        OpenSource, //= 2
    };

    // No Speed register on K1948VK018 GPIO — Speed type intentionally omitted
    // so that speed_io_port<> concept is not satisfied and TPin::SetSpeed() is disabled.

    static void Write(DataType value);
    template <DataType value>
    static void Write();

    static DataType Read();

    static void Clear(DataType value);
    template <DataType value>
    static void Clear();

    static void Set(DataType value);
    template <DataType value>
    static void Set();

    static void ClearAndSet(DataType clearMask, DataType setMask);
    template <DataType clearMask, DataType setMask>
    static void ClearAndSet();

    static void Toggle(DataType value);
    template <DataType value>
    static void Toggle();

    static DataType PinRead();

    static void SetConfiguration(Configuration configuration, DataType mask = std::numeric_limits<DataType>::max());
    template <Configuration configuration, DataType mask = std::numeric_limits<DataType>::max()>
    static void SetConfiguration();

    static void SetDriverType(DriverType driver, DataType mask = std::numeric_limits<DataType>::max());
    template <DriverType driver, DataType mask = std::numeric_limits<DataType>::max()>
    static void SetDriverType();

    static void SetPullMode(PullMode mode, DataType mask = std::numeric_limits<DataType>::max());
    template <PullMode mode, DataType mask = std::numeric_limits<DataType>::max()>
    static void SetPullMode();

    static void AltFuncNumber(uint8_t number, DataType mask = std::numeric_limits<DataType>::max());
    template <uint8_t number, DataType mask = std::numeric_limits<DataType>::max()>
    static void AltFuncNumber();

    static void Enable();
    static void Disable();
};

class NullPort : public NativePortBase {
public:
    static void Write(DataType) { }
    template <DataType>
    static void Write() { }

    static DataType Read() { return DataType{}; }

    static void Clear(DataType) { }
    template <DataType>
    static void Clear() { }

    static void Set(DataType) { }
    template <DataType>
    static void Set() { }

    static void ClearAndSet(DataType, DataType) { }
    template <DataType, DataType>
    static void ClearAndSet() { }

    static void Toggle(DataType) { }
    template <DataType>
    static void Toggle() { }

    static DataType PinRead() { return DataType{}; }

    static void SetConfiguration(Configuration, DataType = std::numeric_limits<DataType>::max()) { }
    template <Configuration, DataType = std::numeric_limits<DataType>::max()>
    static void SetConfiguration() { }

    static void SetDriverType(DriverType, DataType = std::numeric_limits<DataType>::max()) { }
    template <DriverType, DataType = std::numeric_limits<DataType>::max()>
    static void SetDriverType() { }

    static void SetPullMode(PullMode, DataType = std::numeric_limits<DataType>::max()) { }
    template <PullMode, DataType = std::numeric_limits<DataType>::max()>
    static void SetPullMode() { }

    static void AltFuncNumber(uint8_t, DataType = std::numeric_limits<DataType>::max()) { }
    template <uint8_t, DataType = std::numeric_limits<DataType>::max()>
    static void AltFuncNumber() { }

    static void Enable() { }
    static void Disable() { }

    enum {
        Id = '-'
    };
};

namespace Private {
template <typename _Regs, typename _ClkEnReg, uint8_t ID>
class PortImplementation : public NativePortBase {
public:
    static DataType Read() { return static_cast<DataType>(_Regs()->OUTPUT); }

    static void Write(DataType value) { _Regs()->OUTPUT = value; }

    static void Set(DataType value) { _Regs()->SET = value; }

    static void Clear(DataType value) { _Regs()->CLEAR = value; }

    static void Toggle(DataType value) { _Regs()->OUTPUT ^= value; }

    // Clear before set so a pin in both masks ends up set (matches STM32 BSRR semantics).
    static void ClearAndSet(DataType clearMask, DataType setMask) {
        _Regs()->CLEAR = clearMask;
        _Regs()->SET   = setMask;
    }

    static DataType PinRead() { return static_cast<DataType>(_Regs()->DATA); }

    template <DataType value>
    static void Write() { _Regs()->OUTPUT = value; }

    template <DataType value>
    static void Set() { _Regs()->SET = value; }

    template <DataType value>
    static void Clear() { _Regs()->CLEAR = value; }

    template <DataType value>
    static void Toggle() { _Regs()->OUTPUT ^= value; }

    template <DataType clearMask, DataType setMask>
    static void ClearAndSet() {
        _Regs()->CLEAR = clearMask;
        _Regs()->SET   = setMask;
    }

    static void SetConfiguration(Configuration cfg, DataType mask = std::numeric_limits<DataType>::max()) {
        switch(cfg) {
        case Analog:
        case In:
            _Regs()->DIRECTION_IN = mask;
            _Regs()->ALTFUNCCLR   = mask;
            break;
        case Out:
            _Regs()->DIRECTION_OUT = mask;
            _Regs()->ALTFUNCCLR    = mask;
            break;
        case AltFunc:
            _Regs()->ALTFUNCSET = mask;
            break;
        }
    }

    template <Configuration cfg, DataType mask = std::numeric_limits<DataType>::max()>
    static void SetConfiguration() {
        if constexpr(cfg == Analog || cfg == In) {
            _Regs()->OUTENCLR   = mask;
            _Regs()->ALTFUNCCLR = mask;
        } else if constexpr(cfg == Out) {
            _Regs()->OUTENSET   = mask;
            _Regs()->ALTFUNCCLR = mask;
        } else if constexpr(cfg == AltFunc) {
            _Regs()->ALTFUNCSET = mask;
        }
    }

    static void SetPullMode(PullMode mode, DataType mask = std::numeric_limits<DataType>::max()) {
        if(mode == PullUp)
            _Regs()->PULLMODE |= static_cast<uint32_t>(mask);
        else
            _Regs()->PULLMODE &= ~static_cast<uint32_t>(mask);
    }

    template <PullMode mode, DataType mask = std::numeric_limits<DataType>::max()>
    static void SetPullMode() {
        if constexpr(mode == PullUp)
            _Regs()->PULLMODE |= static_cast<uint32_t>(mask);
        else
            _Regs()->PULLMODE &= ~static_cast<uint32_t>(mask);
    }

    // OUTMODE: 2 bits per pin (0=push-pull, 1=open-drain, 2=open-source).
    static void SetDriverType(DriverType driver, DataType mask = std::numeric_limits<DataType>::max()) {
        const uint32_t m = Expand2Bit(mask);
        _Regs()->OUTMODE = (_Regs()->OUTMODE & ~m) | (m / 3u * static_cast<uint32_t>(driver));
    }

    template <DriverType driver, DataType mask = std::numeric_limits<DataType>::max()>
    static void SetDriverType() {
        constexpr uint32_t m = Expand2Bit(mask);
        _Regs()->OUTMODE     = (_Regs()->OUTMODE & ~m) | (m / 3u * static_cast<uint32_t>(driver));
    }

    // ALTFUNCNUM: 2 bits per pin, selects AF 0-3.
    static void AltFuncNumber(uint8_t n, DataType mask = std::numeric_limits<DataType>::max()) {
        const uint32_t m    = Expand2Bit(mask);
        _Regs()->ALTFUNCNUM = (_Regs()->ALTFUNCNUM & ~m) | (m / 3u * static_cast<uint32_t>(n));
    }

    template <uint8_t n, DataType mask = std::numeric_limits<DataType>::max()>
    static void AltFuncNumber() {
        constexpr uint32_t m = Expand2Bit(mask);
        _Regs()->ALTFUNCNUM  = (_Regs()->ALTFUNCNUM & ~m) | (m / 3u * static_cast<uint32_t>(n));
    }

    static void Enable() { _ClkEnReg::Enable(); }
    static void Disable() { _ClkEnReg::Disable(); }

    enum {
        Id = ID
    };

private:
    // Expand a 16-bit pin mask to a 32-bit mask with 2 bits per pin.
    // Each set bit i in `mask` becomes bits 2i and 2i+1 set in the result.
    // Unset bits become 00.  The result is a valid "2-bit mask" for registers
    // like OUTMODE and ALTFUNCNUM.
    //
    // Property used by SetDriverType / AltFuncNumber:
    //   (m / 3) * value  writes `value` into every selected 2-bit slot,
    //   because each 0b11 group equals 3 * 4^i, dividing by 3 gives 4^i,
    //   and multiplying by the desired value (0-3) places it correctly.
    static constexpr uint32_t Expand2Bit(uint16_t mask) {
        uint32_t m = static_cast<uint32_t>(mask);
        m          = (m | (m << 8)) & 0x00FF00FFu;
        m          = (m | (m << 4)) & 0x0F0F0F0Fu;
        m          = (m | (m << 2)) & 0x33333333u;
        m          = (m | (m << 1)) & 0x55555555u;
        return m | (m << 1);
    }
};
} // namespace Private
} // namespace Zhele::IO
