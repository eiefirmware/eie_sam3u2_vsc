/*!**********************************************************************************************************************
@file leds.c                                                                
@brief LED driver and API

This driver provides on, off, toggle, and blink functionality.
The basic on/off/toggle functionality is applied directly to the LEDs.
Blinking of LEDs rely on the EIE operating system to provide timing at
regular 1ms calls to LedSM_Idle().

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- LedNameType (devboard-specific)

 (from eief1-pcb-01):
 {WHITE, PURPLE, BLUE, CYAN, 
  GREEN, YELLOW, ORANGE, RED, 
  LCD_RED, LCD_GREEN, LCD_BLUE}

 (from mpgl2-ehdw-02):
 {RED0,   RED1,   RED2,   RED3, 
  GREEN0, GREEN1, GREEN2, GREEN3, 
  BLUE0,  BLUE1,  BLUE2,  BLUE3}


- LedRateType:
  {LED_0HZ = 0, LED_0_5HZ = 1000, LED_1HZ = 500, LED_2HZ = 250, LED_4HZ = 125, LED_8HZ = 63,
   LED_PWM_0 = 0,   LED_PWM_5 = 1,   LED_PWM_10 = 2,  LED_PWM_15 = 3,  LED_PWM_20 = 4, 
   LED_PWM_25 = 5,  LED_PWM_30 = 6,  LED_PWM_35 = 7,  LED_PWM_40 = 8,  LED_PWM_45 = 9, 
   LED_PWM_50 = 10, LED_PWM_55 = 11, LED_PWM_60 = 12, LED_PWM_65 = 13, LED_PWM_70 = 14, 
   LED_PWM_75 = 15, LED_PWM_80 = 16, LED_PWM_85 = 17, LED_PWM_90 = 18, LED_PWM_95 = 19, 
   LED_PWM_100 = 20} 

PUBLIC FUNCTIONS
- void LedOn(LedNameType eLED_)
- void LedOff(LedNameType eLED_)
- void LedToggle(LedNameType eLED_)
- void LedBlink(LedNameType eLED_, LedRateType eBlinkRate_)
- void LedPWM(LedNameType eLED_, LedRateType ePwmRate_)

PROTECTED FUNCTIONS
- void LedInitialize(void)
- void LedRunActiveState(void)

***********************************************************************************************************************/

#include "configuration.h"


/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Led"
***********************************************************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
/* New variables (all shall start with G_xxLed*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */
extern volatile u32 G_u32SystemTime1ms;                /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                 /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                  /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;             /*!< @brief From main.c */

extern const PinConfigurationType G_asBspLedConfigurations[U8_TOTAL_LEDS]; /*!< @brief from board-specific file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Led_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Led_StateMachine;                   /*!< @brief The state machine function pointer */
//static u32 Led_u32Timeout;                             /*!< @brief Timeout counter used across states */

static LedControlType Led_asControl[U8_TOTAL_LEDS];    /*!< @brief Holds individual control parameters for LEDs */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedOn(LedNameType eLED_)

@brief Turn the specified LED on.  

This function automatically takes care of the active low vs. active high LEDs.
The function works immediately (it does not require the main application
loop to be running). 

Currently it only supports one LED at a time.

Example:

LedOn(BLUE);


Requires:
- Definitions in G_asBspLedConfigurations[eLED_] and Led_asControl[eLED_] are correct

@param eLED_ is a valid LED index

Promises:
- eLED_ is turned on 
- eLED_ is set to LED_NORMAL_MODE mode

*/
void LedOn(LedNameType eLED_)
{
  u32 *pu32OnAddress;

  /* Configure set and clear addresses */
  if(G_asBspLedConfigurations[eLED_].eActiveState == ACTIVE_HIGH)
  {
    /* Active high LEDs use SODR to turn on */
    pu32OnAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_SODR) + G_asBspLedConfigurations[(u8)eLED_].ePort);
  }
  else
  {
    /* Active low LEDs use CODR to turn on */
    pu32OnAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_CODR) + G_asBspLedConfigurations[(u8)eLED_].ePort);
  }
  
  /* Turn on the LED */
  *pu32OnAddress = G_asBspLedConfigurations[(u8)eLED_].u32BitPosition;
  
  /* Always set the LED back to LED_NORMAL_MODE mode */
	Led_asControl[(u8)eLED_].eMode = LED_NORMAL_MODE;

} /* end LedOn() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedOff(LedNameType eLED_)

@brief Turn the specified LED off.

This function automatically takes care of the active low vs. active high LEDs.
It works immediately (it does not require the main application
loop to be running). 

Currently it only supports one LED at a time.

Example:

LedOff(BLUE);


Requires:
- Definitions in G_asBspLedConfigurations[eLED_] and Led_asControl[eLED_] are correct

@param eLED_ is a valid LED index

Promises:
- eLED_ is turned off 
- eLED_ is set to LED_NORMAL_MODE mode

*/
void LedOff(LedNameType eLED_)
{
  u32 *pu32OffAddress;
  
  pu32OffAddress = 0;

  /* Configure set and clear addresses */
  if(G_asBspLedConfigurations[(u8)eLED_].eActiveState == ACTIVE_HIGH)
  {
    /* Active high LEDs use CODR to turn off */
    pu32OffAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_CODR) + G_asBspLedConfigurations[(u8)eLED_].ePort);
  }
  else
  {
    /* Active low LEDs use SODR to turn off */
    pu32OffAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_SODR) + G_asBspLedConfigurations[(u8)eLED_].ePort);
  }
  
  /* Clear the bit corresponding to eLED_ */
	*pu32OffAddress = G_asBspLedConfigurations[(u8)eLED_].u32BitPosition;

  /* Always set the LED back to LED_NORMAL_MODE mode */
	Led_asControl[(u8)eLED_].eMode = LED_NORMAL_MODE;
  
} /* end LedOff() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedToggle(LedNameType eLED_)

@brief Toggles the specified LED from on to off or vise-versa.

This function automatically takes care of the active low vs. active high LEDs.
It works immediately (it does not require the main application
loop to be running). 

Currently it only supports one LED at a time.

Example:

LedToggle(BLUE);


Requires:
- Write access to PIOx_ODSR is enabled

@param eLED_ is a valid LED index

Promises:
- eLED_ is toggled 
- eLED_ is set to LED_NORMAL_MODE

*/
void LedToggle(LedNameType eLED_)
{
  u32* pu32Address = (u32*)(&(AT91C_BASE_PIOA->PIO_ODSR) + G_asBspLedConfigurations[eLED_].ePort);

  *pu32Address ^= G_asBspLedConfigurations[(u8)eLED_].u32BitPosition;
  
  /* Set the LED to LED_NORMAL_MODE mode */
	Led_asControl[(u8)eLED_].eMode = LED_NORMAL_MODE;

} /* end LedToggle() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedBlink(LedNameType eLED_, LedRateType eBlinkRate_)

@brief Sets eLED_ to BLINK mode with the rate given.

BLINK mode requires the main loop to be running at 1ms period. If the main
loop timing is regularly off, the blinking timing may be affected although
unlikely to a noticeable degree.  

Example to blink the PURPLE LED at 1Hz:

LedBlink(PURPLE, LED_1HZ);


Requires:
@param eLED_ is a valid LED index
@param eBlinkRate_ is an allowed blinking rate from LedRateType

Promises:
- eLED_ is set to LED_BLINK_MODE at the blink rate specified

*/
void LedBlink(LedNameType eLED_, LedRateType eBlinkRate_)
{
	Led_asControl[(u8)eLED_].eMode = LED_BLINK_MODE;
	Led_asControl[(u8)eLED_].eRate = eBlinkRate_;
	Led_asControl[(u8)eLED_].u16Count = eBlinkRate_;

} /* end LedBlink() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedPWM(LedNameType eLED_, LedRateType ePwmRate_)

@brief Sets an LED to PWM mode with the rate given.

The PWM output is bit-bashed based on the 1ms system timing.  Therefore,
PWM mode requires the main loop to be running properly. If the main 
loop timing is longer than 1ms, noticeable glitches will be observed
in the PWM signal to the LED.  Even if all applications are working properly,
there still may be some jitter due to applications taking processor time.

Use LedOff(eLED_) to stop PWM mode and return to NORMAL mode.

Example to turn on the BLUE LED with 5% duty cycle:

LedPWM(BLUE, LED_PWM_5);


Requires:
@param eLED_ is a valid LED index
@param ePwmRate_ is an allowed duty cycle - use a PWM value from LedRateType

Promises:
- eLED_ is set to PWM mode at the duty cycle rate specified

*/
void LedPWM(LedNameType eLED_, LedRateType ePwmRate_)
{
  Led_asControl[(u8)eLED_].eMode = LED_PWM_MODE;
  Led_asControl[(u8)eLED_].eRate = ePwmRate_;
  Led_asControl[(u8)eLED_].u16Count = (u16)ePwmRate_;
  Led_asControl[(u8)eLED_].eCurrentDuty = LED_PWM_DUTY_HIGH;

} /* end LedPWM() */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef EIE_ASCII
/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedInitialize(void)

@brief Initialization of LED system parameters and visual LED check.


Requires:
- 

Promises:
- Led_asControl is initialized (all LEDs in LED_NORMAL_MODE)

*/
void LedInitialize(void)
{
  u32 u32Timer;
  u8  u8Index;

  u32 u32Buzzer1Frequency = 4000;
  u32 u32Buzzer2Frequency = 500;
  u32 u32StepSize = (u32Buzzer1Frequency - u32Buzzer2Frequency) / 20;

  /* Initialize the LED control array */
  for(u8 i = 0; i < U8_TOTAL_LEDS; i++)
  {
    LedPWM( (LedNameType)i, LED_PWM_100);
  }
  
  /* Fade the LEDS out */
  for(u8Index = 20; u8Index > 0; u8Index--)
  {
#ifdef STARTUP_SOUND
    /* Configure Buzzers to provide some audio during start up */
    PWMAudioSetFrequency(BUZZER1, u32Buzzer1Frequency);
    PWMAudioOn(BUZZER1);
    PWMAudioSetFrequency(BUZZER2, u32Buzzer2Frequency);
    PWMAudioOn(BUZZER2);
#endif /* STARTUP_SOUND */
    
    /* Spend a little bit of time in each level of intensity */
    for(u16 j = 20; j > 0; j--)
    {
      u32Timer = G_u32SystemTime1ms;
      while( !IsTimeUp(&u32Timer, 1) );
      LedSM_Idle();
    }
    /* Pause for a bit on the first iteration to show the LEDs on for little while */
    if(u8Index == 20)
    {
      while( !IsTimeUp(&u32Timer, 200) );
    }
    
    /* Set the LED intensity for the next iteration */
    for(u8 j = 0; j < U8_TOTAL_LEDS; j++)
    {
      Led_asControl[j].eRate = (LedRateType)(u8Index - 1);
    }
    
    /* Set the buzzer frequency for the next iteration */
    u32Buzzer1Frequency -= u32StepSize;
    u32Buzzer2Frequency += u32StepSize;
  }

  /* Final update to set last state, hold for a short period */
  LedSM_Idle();
  while( !IsTimeUp(&u32Timer, 200) );
  
#ifdef STARTUP_SOUND
  /* Turn off the buzzers */
  PWMAudioOff(BUZZER1);
  PWMAudioOff(BUZZER2);
#endif /* STARTUP_SOUND */
 

  /* Initialize the LED control array */
  for(u8 i = 0; i < U8_TOTAL_LEDS; i++)
  {
    Led_asControl[i].eMode = LED_NORMAL_MODE;
    Led_asControl[i].eRate = LED_0HZ;
    Led_asControl[i].u16Count = 0;
    Led_asControl[i].eCurrentDuty = LED_PWM_DUTY_LOW;
  }

  /* Backlight on and white */
  LedOn(LCD_RED);
  LedOn(LCD_GREEN);
  LedOn(LCD_BLUE);

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    /* Final setup and report that LED system is ready */
    G_u32ApplicationFlags |= _APPLICATION_FLAGS_LED;
    DebugPrintf("LED functions ready\n\r");
    Led_StateMachine = LedSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    Led_StateMachine = LedSM_Error;
  }
  
} /* end LedInitialize() */
#endif /* EIE_ASCII */


#ifdef EIE_DOTMATRIX
/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedInitialize(void)

@brief Initialization of LED system parameters and visual LED check.


Requires:
- 

Promises:
- Led_asControl is initialized (all LEDs in LED_NORMAL_MODE)

*/
void LedInitialize(void)
{
  u32 u32Timer;
  u8  u8Index;

  u32 u32Buzzer1Frequency = 4000;
  u32 u32StepSize = u32Buzzer1Frequency / 20;

  /* Initialize the LED control array */
  for(u8 i = 0; i < U8_TOTAL_LEDS; i++)
  {
    LedPWM( (LedNameType)i, LED_PWM_100);
  }
  
  /* Fade the LEDS out */
  for(u8Index = 20; u8Index > 0; u8Index--)
  {
#ifdef STARTUP_SOUND
    /* Configure Buzzers to provide some audio during start up */
    PWMAudioSetFrequency(BUZZER1, u32Buzzer1Frequency);
    PWMAudioOn(BUZZER1);
#endif /* STARTUP_SOUND */
    
    /* Spend a little bit of time in each level of intensity */
    for(u16 j = 20; j > 0; j--)
    {
      u32Timer = G_u32SystemTime1ms;
      while( !IsTimeUp(&u32Timer, 1) );
      LedSM_Idle();
    }
    /* Pause for a bit on the first iteration to show the LEDs on for little while */
    if(u8Index == 20)
    {
      while( !IsTimeUp(&u32Timer, 200) );
    }
    
    /* Set the LED intensity for the next iteration */
    for(u8 j = 0; j < U8_TOTAL_LEDS; j++)
    {
      Led_asControl[j].eRate = (LedRateType)(u8Index - 1);
    }
    
    /* Set the buzzer frequency for the next iteration */
    u32Buzzer1Frequency -= u32StepSize;
  }

  /* Final update to set last state, hold for a short period */
  LedSM_Idle();
  while( !IsTimeUp(&u32Timer, 200) );
  
#ifdef STARTUP_SOUND
  /* Turn off the buzzers */
  PWMAudioOff(BUZZER1);
#endif /* STARTUP_SOUND */
 

  /* Initialize the LED control array */
  for(u8 i = 0; i < U8_TOTAL_LEDS; i++)
  {
    Led_asControl[i].eMode = LED_NORMAL_MODE;
    Led_asControl[i].eRate = LED_0HZ;
    Led_asControl[i].u16Count = 0;
    Led_asControl[i].eCurrentDuty = LED_PWM_DUTY_LOW;
  }

  /* Backlight on */
  LedOn(LCD_BL);

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    /* Final setup and report that LED system is ready */
    G_u32ApplicationFlags |= _APPLICATION_FLAGS_LED;
    DebugPrintf("LED functions ready\n\r");
    Led_StateMachine = LedSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    Led_StateMachine = LedSM_Error;
  }
  
} /* end LedInitialize() */
#endif /* EIE_DOTMATRIX */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void LedRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void LedRunActiveState(void)
{
  Led_StateMachine();

} /* end LedRunActiveState */



/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void LedSM_Idle(void)

@brief Run through all the LEDs to check for blinking updates.
*/
static void LedSM_Idle(void)
{
  u32* pu32Address;
  
	/* Loop through each LED to check for blinkers */
  for(u8 i = 0; i < U8_TOTAL_LEDS; i++)
  {
    /* Check if LED is in LED_BLINK_MODE */
    if(Led_asControl[(LedNameType)i].eMode == LED_BLINK_MODE)
    {
      /* Decrement counter and check for 0 */
      if( --Led_asControl[(LedNameType)i].u16Count == 0)
      {
        /* Toggle and reload the LED */
        pu32Address = (u32*)(&(AT91C_BASE_PIOA->PIO_ODSR) + G_asBspLedConfigurations[i].ePort);
        *pu32Address ^= G_asBspLedConfigurations[i].u32BitPosition;
        Led_asControl[(LedNameType)i].u16Count = Led_asControl[(LedNameType)i].eRate;
      }
    } /* end LED_BLINK_MODE */
    
    /* Check if LED is in LED_PWM_MODE */
    if(Led_asControl[(LedNameType)i].eMode == LED_PWM_MODE)
    {
      /* Handle special case of 0% duty cycle */
      if( Led_asControl[i].eRate == LED_PWM_0 )
      {
        LedOff( (LedNameType)i );
      }
      
      /* Handle special case of 100% duty cycle */
      else if( Led_asControl[i].eRate == LED_PWM_100 )
      {
        LedOn( (LedNameType)i );
      }
  
      /* Otherwise, regular PWM: decrement counter; toggle and reload if counter reaches 0 */
      else
      {
        if(--Led_asControl[(LedNameType)i].u16Count == 0)
        {
          if(Led_asControl[(LedNameType)i].eCurrentDuty == LED_PWM_DUTY_HIGH)
          {
            /* Turn the LED off and update the counters for the next cycle */
            LedOff( (LedNameType)i );
            Led_asControl[(LedNameType)i].u16Count = LED_PWM_100 - Led_asControl[(LedNameType)i].eRate;
            Led_asControl[(LedNameType)i].eCurrentDuty = LED_PWM_DUTY_LOW;
          }
          else
          {
            /* Turn the LED on and update the counters for the next cycle */
            LedOn( (LedNameType)i );
            Led_asControl[i].u16Count = Led_asControl[i].eRate;
            Led_asControl[i].eCurrentDuty = LED_PWM_DUTY_HIGH;
          }
        }
      }

      /* Set the LED back to PWM mode since LedOff and LedOn set it to normal mode */
     	Led_asControl[(LedNameType)i].eMode = LED_PWM_MODE;
      
    } /* end LED_PWM_MODE */
    
  } /* end for(u8 i = 0; i < U8_TOTAL_LEDS; i++) */
   
} /* end LedSM_Idle() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn static void LedSM_Error(void)

@brief Handle an error here.  For now, the task is just held in this state. 
*/
static void LedSM_Error(void)          
{
  
} /* end LedSM_Error() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/


