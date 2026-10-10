/**
 * @file
 * Base for GPIO ports
 * @author Konstantin Chizhov
 * @date 2012
 * @license MIT
 */

#ifndef ZHELE_PLATFORM_STM32_COMMON_IOPORTS_H
#define ZHELE_PLATFORM_STM32_COMMON_IOPORTS_H

#include "ioreg.h"
#include <zhele/common/template_utils/type_list.h>

#include <zhele/clock.h>

#include <numeric>

namespace Zhele
{
    namespace IO
    {
        /**
         * @brief
         * Base class for GPIO port
         */
        class NativePortBase
        {
        public:
            using DataType = uint16_t;

            // Port configuartion
            enum Configuration
            {
                In = 0,
                Out = 1,
                AltFunc = 2,
                Analog = 3
            };

            // Pull mode
            enum PullMode
            {
                NoPull = 0,
                PullUp   = 1,
                PullDown = 2
            };

            // Port driver type
            enum DriverType
            {
                PushPull = 0,	///< Push-pull
                OpenDrain = 1	///< Open-drain
            };

            // Port speed
            enum Speed
            {
                Slow = 0, ///< Slow (< 2 MHz)
                Medium = 1, ///< Medium (< 10 MHz)
                Fast = 2, ///< Fast (< 50MHz)
                Fastest = 3 ///< Fastest (< 50MHz)
            };
            
            /**
             * @brief Send value to port
             * 
             * @param [in] value Value to write
             * 
             * @par Returns
             *  Nothing
            */
            static void Write(DataType value);

            /**
             * @brief Template @ref Write method
             * 
             * @tparam value Value to write
             * 
             * @par Returns
             *  Nothing
            */
            template<DataType value>
            static void Write();

            /**
             * @brief Read output port value
             * @details
             * Method returns ODR register value
             * 
             * @return Port value
             */
            static DataType Read();

            /**
             * @brief Clear (reset) bits by mask
             * 
             * @param [in] value Clear mask
             * 
             * @par Returns
             *  Nothing
            */
            static void Clear(DataType value);

            /**
             * @brief Template @ref Clear method
             * 
             * @tparam value Clear mask
             * 
             * @par Returns
             *  Nothing
            */
            template<DataType value>
            static void Clear();

            /**
             * @brief Set bits by mask
             * 
             * @param [in] value Set mask
             * 
             * @par Returns
             *  Nothing
            */
            static void Set(DataType value);

            /**
             * @brief Template @ref Set method
            */
            template<DataType value>
            static void Set();

            /**
             * @brief Clear and set bits
             * 
             * @param [in] clearMask Clear mask
             * @param [in] setMask Set mask
             * 
             * @par Returns
             *  Nothing
            */
            static void ClearAndSet(DataType clearMask, DataType setMask);

            /**
             * @brief Template @ref ClearAndSet method
             * 
             * @tparam clearMask Clear mask
             * @tparam setMask Set mask
             * 
             * @par Returns
             *  Nothing
            */
            template<DataType clearMask, DataType setMask>
            static void ClearAndSet();

            /**
             * @brief Toggle output bits
             * 
             * @details Inverse output register
             * 
             * @param [in] value Toggle mask
             * 
             * @par Returns
             *  Nothing
            */
            static void Toggle(DataType value);

            /**
             * @brief Template @ref Toggle method
             * 
             * @tparam value Toggle mask
             * 
             * @par Returns
             *  Nothing
            */
            template<DataType value>
            static void Toggle();

            /**
             * @brief Returns input port value
             * 
             * @details
             * Read and return IDR register
             * 
             * @returns IDR register value
            */
            static DataType PinRead();

            /**
             * @brief Set port configuration
             * 
             * @param [in] mask Pin mask
             * @param [in] configuration Selected pins configuration
             * 
             * @par Returns
             *  Nothing
            */
            static void SetConfiguration(Configuration configuration, DataType mask = std::numeric_limits<DataType>::max());

            /**
             * @brief Template @ref SetConfiguration method
             * 
             * @tparam mask Pin mask
             * @tparam configuration Selected pins configuration
             * 
             * @par Returns
             *  Nothing
            */
            template<Configuration configuration, DataType mask = std::numeric_limits<DataType>::max()>
            static void SetConfiguration();

            /**
             * @brief Set driver type
             * 
             * @param [in] mask Pin mask
             * @param [in] driver Driver type for selected pins
             * 
             * @par Returns
             *  Nothing
            */
            static void SetDriverType(DriverType driver, DataType mask = std::numeric_limits<DataType>::max());

            /**
             * @brief Template @ref SetDriverType method
             * 
             * @tparam mask Pin mask
             * @tparam driver Driver type for selected pins
             * 
             * @par Returns
             *  Nothing
            */
            template <DriverType driver, DataType mask = std::numeric_limits<DataType>::max()>
            static void SetDriverType();

            /**
             * @brief Set pull smode
             * 
             * @param [in] mask Pin mask
             * @param [in] mode Pull mode for selected pins
             * 
             * @par Returns
             *  Nothing
            */
            static void SetPullMode(PullMode mode, DataType mask = std::numeric_limits<DataType>::max());

            /**
             * @brief Template @ref SetPullMode method
             * 
             * @tparam mask Pin mask
             * @tparam mode Pull mode for selected pins
             * 
             * @par Returns
             *  Nothing
            */
            template <PullMode mode, DataType mask = std::numeric_limits<DataType>::max()>
            static void SetPullMode();

            /**
             * @brief Set port speed
             * 
             * @param [in] mask Pin mask
             * @param [in] speed Selected pins speed
             * 
             * @par Returns
             *  Nothing
            */
            static void SetSpeed(Speed speed, DataType mask = std::numeric_limits<DataType>::max());

            /**
             * @brief Template @ref SetSpeed method
             * 
             * @tparam mask Pin mask
             * @tparam speed Selected pins speed
             * 
             * @par Returns
             *  Nothing
            */
            template<Speed speed, DataType mask = std::numeric_limits<DataType>::max()>
            static void SetSpeed();

            /**
             * @brief Set alternate function number
             * 
             * @param [in] mask Pin mask
             * @param [in] number Alternate function number for selected pins
             * 
             * @par Returns
             *  Nothing
            */
            static void AltFuncNumber(uint8_t number, DataType mask = std::numeric_limits<DataType>::max());

            /**
             * @brief Template @ref AltFuncNumber method
             * 
             * @tparam mask Pin mask
             * @tparam number Alternate function number for selected pins
             * 
             * @par Returns
             *  Nothing
            */
            template<uint8_t number, DataType mask = std::numeric_limits<DataType>::max()>
            static void AltFuncNumber();

            /**
             * @brief Enable clock for port
             * 
             * @par Returns
             *  Nothing
            */
            static void Enable();

            /**
             * @brief Disable clock for port
             * 
             * @par Returns
             *  Nothing
            */
            static void Disable();
        };

        class NullPort : public NativePortBase
        {
        public:
            using DataT = uint8_t;

            static void Write(DataT) {}
            template<DataT>
            static void Write() {}
            
            static DataT Read() { return DataT(); }

            static void Clear(DataT) {}
            template<DataT>
            static void Clear() {}

            static void Set(DataT) {}
            template<DataT>
            static void Set() {}

            static void ClearAndSet(DataT, DataT) {}
            template<DataT, DataT>
            static void ClearAndSet() {}

            static void Toggle(DataT) {}
            template<DataT>
            static void Toggle() {}

            static DataT PinRead() { return DataT(); }

            static void SetConfiguration(Configuration, DataT) {}
            template<Configuration, DataT>
            static void SetConfiguration() {}

            static void SetDriverType(DriverType, DataT) {}
            template<DriverType, DataT>
            static void SetDriverType() {}

            static void SetPullMode(PullMode, DataT) {}
            template<PullMode, DataT>
            static void SetPullMode() {}

            static void SetSpeed(Speed, DataT) {}
            template<Speed, DataT>
            static void SetSpeed() {}

            static void AltFuncNumber(uint8_t, DataT) {}
            template<uint8_t, DataT>
            static void AltFuncNumber() {}

            static void Enable() {}
            static void Disable() {}

            enum{Id = '-'};
        };

        namespace Private
        {
            /**
             * @brief Implement GPIO port
             */
            template<typename _Regs, typename _ClkEnReg, int ID>
            class PortImplementation : public NativePortBase
            {
            public:
                /**
                 * @brief Send value to port
                 * 
                 * @param [in] value Value to write
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void Write(DataType value);

                /**
                 * @brief Template @ref Write method
                 * 
                 * @tparam value Value to write
                 * 
                 * @par Returns
                 *  Nothing
                */
                template<DataType value>
                static void Write();

                /**
                 * @brief Read output port value
                 * @details
                 * Method returns ODR register value
                 * 
                 * @return Port value
                 */
                static DataType Read();

                /**
                 * @brief Clear (reset) bits by mask
                 * 
                 * @param [in] value Clear mask
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void Clear(DataType value);

                /**
                 * @brief Template @ref Clear method
                 * 
                 * @tparam value Clear mask
                 * 
                 * @par Returns
                 *  Nothing
                */
                template<DataType value>
                static void Clear();

                /**
                 * @brief Set bits by mask
                 * 
                 * @param [in] value Set mask
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void Set(DataType value);

                /**
                 * @brief Template @ref Set method
                */
                template<DataType value>
                static void Set();

                /**
                 * @brief Clear and set bits
                 * 
                 * @param [in] clearMask Clear mask
                 * @param [in] setMask Set mask
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void ClearAndSet(DataType clearMask, DataType setMask);

                /**
                 * @brief Template @ref ClearAndSet method
                 * 
                 * @tparam clearMask Clear mask
                 * @tparam setMask Set mask
                 * 
                 * @par Returns
                 *  Nothing
                */
                template<DataType clearMask, DataType setMask>
                static void ClearAndSet();

                /**
                 * @brief Toggle output bits
                 * 
                 * @details Inverse output register
                 * 
                 * @param [in] value Toggle mask
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void Toggle(DataType value);

                /**
                 * @brief Template @ref Toggle method
                 * 
                 * @tparam value Toggle mask
                 * 
                 * @par Returns
                 *  Nothing
                */
                template<DataType value>
                static void Toggle();

                /**
                 * @brief Returns input port value
                 * 
                 * @details
                 * Read and return IDR register
                 * 
                 * @returns IDR register value
                */
                static DataType PinRead();

                /**
                 * @brief Set port configuration
                 * 
                 * @param [in] mask Pin mask
                 * @param [in] configuration Selected pins configuration
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void SetConfiguration(Configuration configuration, DataType mask = std::numeric_limits<DataType>::max());

                /**
                 * @brief Template @ref SetConfiguration method
                 * 
                 * @tparam mask Pin mask
                 * @tparam configuration Selected pins configuration
                 * 
                 * @par Returns
                 *  Nothing
                */
                template<Configuration configuration, DataType mask = std::numeric_limits<DataType>::max()>
                static void SetConfiguration();

                /**
                 * @brief Set driver type
                 * 
                 * @param [in] mask Pin mask
                 * @param [in] driver Driver type for selected pins
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void SetDriverType(DriverType driver, DataType mask = std::numeric_limits<DataType>::max());

                /**
                 * @brief Template @ref SetDriverType method
                 * 
                 * @tparam mask Pin mask
                 * @tparam driver Driver type for selected pins
                 * 
                 * @par Returns
                 *  Nothing
                */
                template <DriverType driver, DataType mask = std::numeric_limits<DataType>::max()>
                static void SetDriverType();

                /**
                 * @brief Set pull smode
                 * 
                 * @param [in] mask Pin mask
                 * @param [in] mode Pull mode for selected pins
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void SetPullMode(PullMode mode, DataType mask = std::numeric_limits<DataType>::max());

                /**
                 * @brief Template @ref SetPullMode method
                 * 
                 * @tparam mask Pin mask
                 * @tparam mode Pull mode for selected pins
                 * 
                 * @par Returns
                 *  Nothing
                */
                template <PullMode mode, DataType mask = std::numeric_limits<DataType>::max()>
                static void SetPullMode();

                /**
                 * @brief Set port speed
                 * 
                 * @param [in] mask Pin mask
                 * @param [in] speed Selected pins speed
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void SetSpeed(Speed speed, DataType mask = std::numeric_limits<DataType>::max());

                /**
                 * @brief Template @ref SetSpeed method
                 * 
                 * @tparam mask Pin mask
                 * @tparam speed Selected pins speed
                 * 
                 * @par Returns
                 *  Nothing
                */
                template<Speed speed, DataType mask = std::numeric_limits<DataType>::max()>
                static void SetSpeed();

                /**
                 * @brief Set alternate function number
                 * 
                 * @param [in] mask Pin mask
                 * @param [in] number Alternate function number for selected pins
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void AltFuncNumber(uint8_t number, DataType mask = std::numeric_limits<DataType>::max());

                /**
                 * @brief Template @ref AltFuncNumber method
                 * 
                 * @tparam mask Pin mask
                 * @tparam number Alternate function number for selected pins
                 * 
                 * @par Returns
                 *  Nothing
                */
                template<uint8_t number, DataType mask = std::numeric_limits<DataType>::max()>
                static void AltFuncNumber();

                /**
                 * @brief Enable clock for port
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void Enable();

                /**
                 * @brief Disable clock for port
                 * 
                 * @par Returns
                 *  Nothing
                */
                static void Disable();
                
                enum { Id = ID };

            private:
                static constexpr inline unsigned UnpackConfig2bits(unsigned mask, unsigned value, unsigned configuration);
                static constexpr inline unsigned UnpackConfig4Bit(unsigned mask, unsigned value, unsigned configuration);
            };
        }

        /**
         * @brief Implements IO ports list class
         * 
         * @tparam _Ports IOPorts types
         */
        template<typename... >
        class PortList;

        template<typename... _Ports>
        class PortList<template_utils::type_list<_Ports...> >
        {
        public:
            /**
             * @brief Enables all ports
             * 
             * @par Returns
             *  Nothing
             */
            static void Enable()
            {
                (_Ports::Enable(), ...);
            }

            /**
             * @brief Disables all ports
             * 
             * @par Returns
             *  Nothing
             */
            static void Disable()
            {
                (_Ports::Disable(), ...);
            }
        };

        #if defined(GPIOA)
        namespace Private { using PortaRegs = IoStruct<GPIOA_BASE, GPIO_TypeDef>; }
        using Porta = Private::PortImplementation<Private::PortaRegs, Clock::PortaClock, 'A'>;
        #endif

        #if defined(GPIOB)
        namespace Private { using PortbRegs = IoStruct<GPIOB_BASE, GPIO_TypeDef>; }
        using Portb = Private::PortImplementation<Private::PortbRegs, Clock::PortbClock, 'B'>;
        #endif

        #if defined(GPIOC)
        namespace Private { using PortcRegs = IoStruct<GPIOC_BASE, GPIO_TypeDef>; }
        using Portc = Private::PortImplementation<Private::PortcRegs, Clock::PortcClock, 'C'>;
        #endif

        #if defined(GPIOD)
        namespace Private { using PortdRegs = IoStruct<GPIOD_BASE, GPIO_TypeDef>; }
        using Portd = Private::PortImplementation<Private::PortdRegs, Clock::PortdClock, 'D'>;
        #endif

        #if defined(GPIOE)
        namespace Private { using PorteRegs = IoStruct<GPIOE_BASE, GPIO_TypeDef>; }
        using Porte = Private::PortImplementation<Private::PorteRegs, Clock::PorteClock, 'E'>;
        #endif

        #if defined(GPIOF)
        namespace Private { using PortfRegs = IoStruct<GPIOF_BASE, GPIO_TypeDef>; }
        using Portf = Private::PortImplementation<Private::PortfRegs, Clock::PortfClock, 'F'>;
        #endif

        #if defined(GPIOG)
        namespace Private { using PortgRegs = IoStruct<GPIOG_BASE, GPIO_TypeDef>; }
        using Portg = Private::PortImplementation<Private::PortgRegs, Clock::PortgClock, 'G'>;
        #endif

        #if defined(GPIOH)
        namespace Private { using PorthRegs = IoStruct<GPIOH_BASE, GPIO_TypeDef>; }
        using Porth = Private::PortImplementation<Private::PorthRegs, Clock::PorthClock, 'H'>;
        #endif
    }
}

#include "impl/ioports.h"

#endif //ZHELE_PLATFORM_STM32_COMMON_IOPORTS_H
