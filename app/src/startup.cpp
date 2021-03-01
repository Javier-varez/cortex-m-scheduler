/* 
 * This file is part of the Popcorn
 * Copyright (c) 2020 Javier Alvarez
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <algorithm>
#include <cstdint>

std::uint32_t* const CORE_DEBUG_DHCSR =
    reinterpret_cast<std::uint32_t*>(0xE000EDF0UL);  // -V566

extern "C"
void SystemInit(void);

extern std::uint32_t _estack;

extern "C"
void Reset_Handler() {
  // Initialize data section
  extern std::uint8_t _sdata;
  extern std::uint8_t _edata;
  extern std::uint8_t _sidata;
  std::size_t size = static_cast<size_t>(&_edata - &_sdata);
  std::copy(&_sidata, &_sidata + size, &_sdata);

  // Initialize bss section
  extern std::uint8_t _sbss;
  extern std::uint8_t _ebss;
  std::fill(&_sbss, &_ebss, UINT8_C(0x00));

  // Initialize static objects by calling their constructors
  typedef void (*function_t)();
  extern function_t __init_array_start;
  extern function_t __init_array_end;
  std::for_each(&__init_array_start, &__init_array_end,
    [](const function_t pfn) {
    pfn();
  });

  SystemInit();

  // Jump to main
  asm volatile (
      "msr msp, %[stack_top]\n"
      "bl main"
      : : [stack_top] "r" (&_estack));
}

#define DEFINE_DEFAULT_ISR(name) \
  extern "C" \
  __attribute__((interrupt)) \
  __attribute__((weak)) \
  __attribute__((noreturn)) \
  void name() { \
    volatile int32_t exception_number; \
    asm("mrs %[ipsr_reg], ipsr   \n" \
         : [ipsr_reg] "=r" (exception_number)); \
    exception_number -= 16; \
    if (*CORE_DEBUG_DHCSR & (1 << 0)) { \
      asm("bkpt #0"); \
    } \
    while (true) {} \
  }

extern "C" void SVC_Handler();
extern "C" void PendSV_Handler();
extern "C" void SysTick_Handler();

DEFINE_DEFAULT_ISR(defaultISR)
DEFINE_DEFAULT_ISR(HardFault_Handler)

extern std::uintptr_t g_pfnVectors[];
std::uintptr_t g_pfnVectors[]
__attribute__((section(".isr_vector"))) {
  // Stack Ptr initialization
  reinterpret_cast<std::uintptr_t>(&_estack),
  // Entry point
  reinterpret_cast<std::uintptr_t>(Reset_Handler),
  // Exceptions
  reinterpret_cast<std::uintptr_t>(defaultISR),         // NMI_Handler
  reinterpret_cast<std::uintptr_t>(HardFault_Handler),  // HardFault_Handler
  reinterpret_cast<std::uintptr_t>(defaultISR),         // MemManage_Handler
  reinterpret_cast<std::uintptr_t>(defaultISR),         // BusFault_Handler
  reinterpret_cast<std::uintptr_t>(defaultISR),         // UsageFault_Handler
  reinterpret_cast<std::uintptr_t>(nullptr),            // 0
  reinterpret_cast<std::uintptr_t>(nullptr),            // 0
  reinterpret_cast<std::uintptr_t>(nullptr),            // 0
  reinterpret_cast<std::uintptr_t>(nullptr),            // 0
  reinterpret_cast<std::uintptr_t>(SVC_Handler),        // SVC_Handler
  reinterpret_cast<std::uintptr_t>(defaultISR),         // DebugMon_Handler
  reinterpret_cast<std::uintptr_t>(nullptr),            // 0
  reinterpret_cast<std::uintptr_t>(PendSV_Handler),     // PendSV_Handler
  reinterpret_cast<std::uintptr_t>(SysTick_Handler),    // SysTick_Handler
  // External Interrupts
  reinterpret_cast<std::uintptr_t>(defaultISR),  // WWDG_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // PVD_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // TAMP_STAMP_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // RTC_WKUP_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // FLASH_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // RCC_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // EXTI0_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // EXTI1_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // EXTI2_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // EXTI3_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // EXTI4_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // DMA1_Channel1_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // DMA1_Channel2_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // DMA1_Channel3_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // DMA1_Channel4_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // DMA1_Channel5_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // DMA1_Channel6_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // DMA1_Channel7_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // ADC_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // USB_HP_CAN1_TX_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // USB_LP_CAN1_RX0_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // CAN1_RX1_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // CAN1_SCE_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // EXTI9_5_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // TIM1_BRK_TIM9_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // TIM1_UP_TIM10_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // TIM1_TRG_COM_TIM11_IRQHand
  reinterpret_cast<std::uintptr_t>(defaultISR),  // TIM1_CC_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // TIM2_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // TIM3_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // TIM4_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // I2C1_EV_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // I2C1_ER_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // I2C2_EV_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // I2C2_ER_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // SPI1_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // SPI2_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // USART1_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // USART2_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // USART3_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // EXTI15_10_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // RTC_Alarm_IRQHandler
  reinterpret_cast<std::uintptr_t>(defaultISR),  // OTG_FS_WKUP_IRQHandler
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};
