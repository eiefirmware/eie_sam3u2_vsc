#include <string.h>

#include "configuration.h"

extern char _bss_start[];
extern char _bss_end[];
extern char _data_start[];
extern char _data_end[];
extern char _data_load[];
extern char _stack_init[];

int main(void);
__attribute__((noreturn)) void _start(void);

__attribute__((section(".intvec"))) const void *const vector_table[64] = {
    _stack_init, _start,
    // Keeping the names the same as the IAR startup file to reduce porting
    // effort.
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0, 0, 0, 0,             // Reserved
    SVC_Handler,
    DebugMon_Handler,
    0,                      // Reserved
    PendSV_Handler,
    SysTick_Handler,

    // Configurable interrupts
    SUPC_IrqHandler,    // 0  SUPPLY CONTROLLER
    RSTC_IrqHandler,    // 1  RESET CONTROLLER
    RTC_IrqHandler,     // 2  REAL TIME CLOCK
    RTT_IrqHandler,     // 3  REAL TIME TIMER
    WDT_IrqHandler,     // 4  WATCHDOG TIMER
    PMC_IrqHandler,     // 5  PMC
    EFC0_IrqHandler,    // 6  EFC0
    EFC1_IrqHandler,    // 7  EFC1
    DBGU_IrqHandler,    // 8  DBGU
    HSMC4_IrqHandler,   // 9  HSMC4
    PIOA_IrqHandler,    // 10 Parallel IO Controller A
    PIOB_IrqHandler,    // 11 Parallel IO Controller B
    PIOC_IrqHandler,    // 12 Parallel IO Controller C
    USART0_IrqHandler,  // 13 USART 0
    USART1_IrqHandler,  // 14 USART 1
    USART2_IrqHandler,  // 15 USART 2
    USART3_IrqHandler,  // 16 USART 3
    MCI0_IrqHandler,    // 17 Multimedia Card Interface
    TWI0_IrqHandler,    // 18 TWI 0
    TWI1_IrqHandler,    // 19 TWI 1
    SPI0_IrqHandler,    // 20 Serial Peripheral Interface
    SSC0_IrqHandler,    // 21 Serial Synchronous Controller 0
    TC0_IrqHandler,     // 22 Timer Counter 0
    TC1_IrqHandler,     // 23 Timer Counter 1
    TC2_IrqHandler,     // 24 Timer Counter 2
    PWM_IrqHandler,     // 25 Pulse Width Modulation Controller
    ADCC0_IrqHandler,   // 26 ADC controller0
    ADCC1_IrqHandler,   // 27 ADC controller1
    HDMA_IrqHandler,    // 28 HDMA
    UDPD_IrqHandler,   // 29 USB Device High Speed UDP_HS
};

void c_startup(void) {
  // Make sure our vector table is the one actually used.
  AT91C_BASE_NVIC->NVIC_VTOFFR = ((uintptr_t)(&vector_table));

  // Init .data
  memcpy(_data_start, _data_load, (size_t)(_data_end - _data_start));
  // Init .bss
  memset(_bss_start, 0, (size_t)(_bss_end - _bss_start));

  main();

  // NOTE: Normally exit() would be called here to do cleanup. However with
  // newlib-nano that pulls in a bunch of stdio code we don't really want.

  // Main shouldn't return. But if it does, lets just reset.
  NVIC_SystemReset();
}
