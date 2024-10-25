/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  // Make sure all LEDs and backlight are off to start
  for(u8 i = 0; i < U8_TOTAL_LEDS; i++)
  {
    LedOff((LedNameType)i);
  }

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Button module exercises */
static void UserApp1SM_Idle(void)
{
     /*
     static bRed1Blink = FALSE;
     static LedRateType aeBLinkRate[] = {LED_1HZ, LED_2HZ, LED_4HZ, LED_8HZ};
     static u8 u8BlinkRateIndex = 0;
     */
     static yellow3On = TRUE;
     static u8 password[] = {0,0,0,0,0,0,0,0,0,0};
     const u8 correctPassword[] = {1,2,1,2,1,2,1,2,1,2};
     static correct = FALSE;
     static wrong = TRUE;
     static u16 hold = 0;
     static u8 buttonPressed = 0;
     if(yellow3On)
     {
      LedOn(GREEN3);
      LedOn(BLUE3);
     } 
     if(IsButtonHeld(BUTTON0, 2000) && IsButtonHeld(BUTTON1, 2000))
     {
      for (u8 i = 0; i< (sizeof(password)/sizeof(u8)); i++)
      {
        if(password[i] == correctPassword[i])
        {
          correct = FALSE;
          break;
        }
        else
        {
          correct = TRUE;
        }
      }
      if(correct == TRUE)
      {

      }
     }
     if(correct == FALSE && wrong == TRUE)
      {
        LedOff(GREEN3);
        LedOff(BLUE3);
        yellow3On = FALSE;
        LedBlink(RED3, LED_2HZ);
        if (hold > 2000)
        {
          LedOff(RED3);
          wrong = FALSE;
          hold = 0;
          yellow3On = TRUE;
        }
      }
      else if (correct == TRUE && wrong == FALSE)
      {
        LedOff(GREEN3);
        LedOff(BLUE3);
        yellow3On = FALSE;
        LedBlink(GREEN3, LED_2HZ);
      }
      if(WasButtonPressed(BUTTON0))
      {
        ButtonAcknowledge(BUTTON0);
        if(buttonPressed < 10)
        {
          password[buttonPressed] = 1;
          buttonPressed++;
        }
        if(buttonPressed >= 10)
        {
          buttonPressed = 0;
        }
      }
      if(WasButtonPressed(BUTTON1))
      {
        ButtonAcknowledge(BUTTON1);
        if(buttonPressed < 10)
        {
          password[buttonPressed] = 2;
          buttonPressed++;
        }
        if(buttonPressed >= 10)
        {
          buttonPressed = 0;
        }
      }
     /*
     // Turn on the LCD backlight if BUTTON0 has been held for 2 seconds
     if(IsButtonHeld(BUTTON0, 2000))
     {
      LedOn(LCD_BL);
     }
     else
     {
      LedOff(LCD_BL);
     }
     // Start the RED1 LED blinking if BUTTON1 was pressed since last check.
     // Turn the RED1 LED off if it was already blinking.
     if(WasButtonPressed(BUTTON1))
     {
      // Don't forget to ACK the button press
      ButtonAcknowledge(BUTTON1);
      
      // If the LED isn't blinking, start the blink
      if(!bRed1Blink)
      {
        bRed1Blink = TRUE;
        LedBlink(RED1, aeBLinkRate[u8BlinkRateIndex]);
        // Make sure there is no pending BUTTON0 press that would change the rate
        ButtonAcknowledge(BUTTON0);
      }
      else
      {
        bRed1Blink = FALSE;
        LedOff(RED1);
      }
    }
    if(WasButtonPressed(BUTTON0) && bRed1Blink)
    {
      //Don't forget to ACK the button press
      ButtonAcknowledge(BUTTON0);

      //Adust and roll the counter
      u8BlinkRateIndex++;
      if(u8BlinkRateIndex == 4)
      {
        u8BlinkRateIndex = 0;
      }
      LedBlink(RED1, aeBLinkRate[u8BlinkRateIndex]);
    }
     if(IsButtonPressed(BUTTON0))
     {
      LedOn(BLUE0);
     }
     else
     {
      LedOff(BLUE0);
     }
     */
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
