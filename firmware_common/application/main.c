/*!**********************************************************************************************************************
@file main.c                                                                
@brief Main system file for the EiE firmware.  
***********************************************************************************************************************/

#include "configuration.h"

extern	void kill_x_cycles(u32);

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32SystemTime1ms = 0;     /*!< @brief Global system time incremented every ms, max 2^32 (~49 days) */
volatile u32 G_u32SystemTime1s  = 0;     /*!< @brief Global system time incremented every second, max 2^32 (~136 years) */
volatile u32 G_u32SystemFlags   = 0;     /*!< @brief Global system flags */
volatile u32 G_u32ApplicationFlags = 0;  /*!< @brief Global system application flags: set when application is successfully initialized */

/* Task short names corresponding to G_u32ApplicationFlags in main.h */
#ifdef EIE_ASCII
const u8 G_aau8AppShortNames[NUMBER_APPLICATIONS][MAX_TASK_NAME_SIZE] = 
{"LED", "BUTTON", "DEBUG", "TIMER", "LCD", "ADC", "ANT"};
#endif /* EIE_ASCII */

#ifdef EIE_DOTMATRIX
const u8 G_aau8AppShortNames[NUMBER_APPLICATIONS][MAX_TASK_NAME_SIZE] = 
{"LED", "BUTTON", "DEBUG", "TIMER", "LCD", "ADC", "ANT", "CAPTOUCH"};
#endif /* EIE_DOTMATRIX */


/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Main_" and be declared as static.
***********************************************************************************************************************/


/*!---------------------------------------------------------------------------------------------------------------------
@fn int main(void)
@brief Main program where all tasks are initialized and executed.

Requires:
- NONE

Promises:
- NONE

*/
int main(void)
{
  G_u32SystemFlags |= _SYSTEM_INITIALIZING;

  /* Low level initialization */
  WatchDogSetup(); 
  ClockSetup();
  GpioSetup();
  PWMSetupAudio();
  InterruptSetup();
  SysTickSetup();
  
  /* Driver initialization */
  MessagingInitialize();
  UartInitialize();
  DebugInitialize();

  /* Debug messages through DebugPrintf() are available from here */
  ButtonInitialize();
  TimerInitialize();  
  SpiInitialize();
  SspInitialize();
  TwiInitialize();

  Adc12Initialize();
  LcdInitialize();
  LedInitialize();
  AntInitialize();
  AntApiInitialize();
  
#ifdef EIE_ASCII
#endif /* EIE_ASCII */

#if defined(EIE_DOTMATRIX) && !defined(EIE_NO_CAPTOUCH)
  CapTouchInitialize();
#endif /* EIE_DOTMATRIX */
 
  /* Application initialization */
  BladeApiInitialize();
  UserApp1Initialize();
  UserApp2Initialize();
  UserApp3Initialize();

  /* Exit initialization */
  SystemStatusReport();
  G_u32SystemFlags &= ~_SYSTEM_INITIALIZING;
  
  /* Super loop */  
  while(1)
  {
    WATCHDOG_BONE();
    SystemTimeCheck();

    /* Drivers */
    MessagingRunActiveState();
    UartRunActiveState();
    DebugRunActiveState();

    ButtonRunActiveState();
    TimerRunActiveState(); 
    SpiRunActiveState();
    SspRunActiveState();
    TwiRunActiveState();

    Adc12RunActiveState();
    LcdRunActiveState();
    LedRunActiveState();
    AntRunActiveState();
    AntApiRunActiveState();

#ifdef EIE_ASCII
#endif /* EIE_ASCII */

#if defined(EIE_DOTMATRIX) && !defined(EIE_NO_CAPTOUCH)
    CapTouchRunActiveState();
#endif /* EIE_DOTMATRIX */

    /* Applications */
    BladeApiRunActiveState();
    UserApp1RunActiveState();
    UserApp2RunActiveState();
    UserApp3RunActiveState();
        
    /* System sleep */
    //HEARTBEAT_OFF();
    SystemSleep();
    //HEARTBEAT_ON();
    
  } /* end while(1) main super loop */
  
} /* end main() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
