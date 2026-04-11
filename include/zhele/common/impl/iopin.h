/**
 * @file
 * IOPin methods implementation
 *
 * @author Konstantin Chizhov
 * @date 2013
 * @license MIT
 */
#ifndef ZHELE_IOPIN_IMPL_COMMON_H
#define ZHELE_IOPIN_IMPL_COMMON_H

namespace Zhele::IO {
  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::Set() {
    Set(true);
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::Set(bool state) {
    state ? Port::template Set<1u << _Pin>() : Port::template Clear<1u << _Pin>();
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::SetDir(bool isWrite) {
    isWrite ? SetDirWrite() : SetDirRead();
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::Clear() {
    Set(false);
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::Toggle() {
    Port::template Toggle<1u << _Pin>();
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::SetDirRead() {
    Port::template SetConfiguration(1u << _Pin, _Port::Configuration::In);
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::SetDirWrite() {
    _ConfigPort::template SetConfiguration<1u << _Pin, _Port::Configuration::Out>();
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::SetConfiguration(typename _ConfigPort::Configuration configuration) {
    _ConfigPort::SetConfiguration(configuration, 1u << _Pin);
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  template<typename _ConfigPort::Configuration configuration>
  void TPin<_Port, _Pin, _ConfigPort>::SetConfiguration() {
    _ConfigPort::template SetConfiguration<configuration, 1u << _Pin>();
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::SetDriverType(typename _ConfigPort::DriverType driverType) {
    _ConfigPort::SetDriverType(driverType, 1u << _Pin);
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  template<typename _ConfigPort::DriverType driverType>
  void TPin<_Port, _Pin, _ConfigPort>::SetDriverType() {
    _ConfigPort::template SetDriverType<driverType, 1u << _Pin>();
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::SetPullMode(typename _ConfigPort::PullMode pullMode) {
    _ConfigPort::SetPullMode(pullMode, 1u << _Pin);
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  template<typename _ConfigPort::PullMode pullMode>
  void TPin<_Port, _Pin, _ConfigPort>::SetPullMode() {
    _ConfigPort::template SetPullMode<pullMode, 1u << _Pin>();
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::SetSpeed(typename _ConfigPort::Speed speed)
  requires requires { typename _ConfigPort::Speed; } {
    _ConfigPort::SetSpeed(speed, 1u << _Pin);
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  template<typename _ConfigPort::Speed speed>
  void TPin<_Port, _Pin, _ConfigPort>::SetSpeed() requires requires { typename _ConfigPort::Speed; } {
    _ConfigPort::template SetSpeed<speed, 1u << _Pin>();
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::AltFuncNumber(uint8_t funcNumber)
  requires requires(uint8_t n) { _ConfigPort::AltFuncNumber(n); } {
    _ConfigPort::AltFuncNumber(funcNumber, 1u << _Pin);
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  template<uint8_t funcNumber>
  void TPin<_Port, _Pin, _ConfigPort>::AltFuncNumber()
  requires requires { _ConfigPort::template AltFuncNumber<0, 0>(); } {
    _ConfigPort::template AltFuncNumber<funcNumber, 1u << _Pin>();
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  bool TPin<_Port, _Pin, _ConfigPort>::IsSet() {
    return (Port::PinRead() & static_cast<typename Port::DataType>(1u << _Pin)) != 0;
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::WaitForSet() {
    while (!IsSet()) continue;
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void TPin<_Port, _Pin, _ConfigPort>::WaitForClear() {
    while (IsSet()) continue;
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void InvertedPin<_Port, _Pin, _ConfigPort>::Set(bool val) {
    TPin<_Port, _Pin, _ConfigPort>::Set(!val);
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void InvertedPin<_Port, _Pin, _ConfigPort>::Set() {
    TPin<_Port, _Pin, _ConfigPort>::Clear();
  }

  template<typename _Port, uint8_t _Pin, typename _ConfigPort>
  void InvertedPin<_Port, _Pin, _ConfigPort>::Clear() {
    TPin<_Port, _Pin, _ConfigPort>::Set();
  }
} // namespace Zhele::IO

#endif // ZHELE_IOPIN_IMPL_COMMON_H
