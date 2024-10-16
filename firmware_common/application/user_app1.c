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
  /* Initialize all LEDs to OFF */
  LedOff(RED0);
  LedOff(GREEN0);
  LedOff(BLUE0);

  LedOff(RED1);
  LedOff(GREEN1);
  LedOff(BLUE1);
  
  LedOff(RED2);
  LedOff(GREEN2);
  LedOff(BLUE2);

  LedOff(RED3);
  LedOff(GREEN3);
  LedOff(BLUE3);

  LedOff(LCD_BL);


#if 0 /* The exercise code */
  /* Turn on the left side blue LED with the LedOn function */
  LedOn(BLUE0);

  /* Turn on the right side red LED with the LedToggle function (what assumption is being made?) */
  LedOn(RED3);

  /* Set the 3rd LED to blink green */
  LedBlink(GREEN2, LED_2HZ);

  /* Set the 2nd LED to the dimmest level available */
  LedPWM(BLUE1, LED_PWM_5);
#endif

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
/* Implement a 4-bit binary counter */
static void UserApp1SM_Idle(void)
{
  static u16 u16BlinkCount = 0;
  static u8  u8Counter = 0;
  static u8  u8ColorIndex = 0;

  /* The color order will be Red, Yellow, Green, Cyan, Blue, Purple, White (u8ColorIndex 0 to 6)*/
  static u8 aau8Color[][3] = {{RED0, 0xff, 0xff},     /* Red */
                              {RED0, GREEN0, 0xff},   /* Yellow */
                              {0xff, GREEN0, 0xff},   /* Green  */
                              {0xff, GREEN0, BLUE0},  /* Cyan   */
                              {0xff, 0xff,   BLUE0},  /* Blue   */
                              {RED0, 0xff,   BLUE0},  /* Purple */
                              {RED0, GREEN0, BLUE0}}; /* White  */

  u8 u8Offset = 0;

  /* Update counter to determine if it's time to increment the 4-bit counter value */
  u16BlinkCount++;
  if(u16BlinkCount == 250)
  {
    /* Reset the period counter and proceed to udpate the display with the new 4-bit value */
    u16BlinkCount = 0;
    
    /* Update the counter and roll at 16 */
    u8Counter++;
    if(u8Counter == 16)
    {
      u8Counter = 0;

      /* Change the color when the counter restarts */
      u8ColorIndex++;
      if(u8ColorIndex == 7)
      {
        u8ColorIndex = 0;
      }
    }


    /* Use knowledge of LedNameType to use a loop for initializing all LEDs to off state */
    for(u8 i = 0; i < (U8_TOTAL_LEDS - 1); i++)
    {
      LedOff( (LedNameType)i );
    }

    /* Set color for each bit (LED) in the 4-bit counter */
    if(u8Counter & 0x01)
    {
      u8Offset = 3; /* The numerical difference between LED elements of the same color (e.g. RED3 - RED0 = 3) */
      for(u8 j = 0; j < 3; j++)
      {
        if(aau8Color[u8ColorIndex][j] != 0xff)
        {
          LedOn( (aau8Color[u8ColorIndex][j]) + u8Offset);
        }
      }
    }

    if(u8Counter & 0x02)
    {
      u8Offset = 2; /* The numerical difference between LED elements of the same color */
      for(u8 j = 0; j < 3; j++)
      {
        if(aau8Color[u8ColorIndex][j] != 0xff)
        {
          LedOn( (aau8Color[u8ColorIndex][j]) + u8Offset);
        }
      }
    }

    if(u8Counter & 0x04)
    {
      u8Offset = 1; /* The numerical difference between LED elements of the same color */
      for(u8 j = 0; j < 3; j++)
      {
        if(aau8Color[u8ColorIndex][j] != 0xff)
        {
          LedOn( (aau8Color[u8ColorIndex][j]) + u8Offset);
        }
      }
    }

    if(u8Counter & 0x08)
    {
      u8Offset = 0; /* The numerical difference between LED elements of the same color */
      for(u8 j = 0; j < 3; j++)
      {
        if(aau8Color[u8ColorIndex][j] != 0xff)
        {
          LedOn( (aau8Color[u8ColorIndex][j]) + u8Offset);
        }
      }
    }

#if 0 /* First exercise */
    /* Turn all the LEDs off, then parse the current u8Counter value to turn back the correct LEDs.  
    RED3 is BIT0, GREEN2 is BIT1, BLUE1 is BIT2, RED0 and BLUE0 are BIT3 to get purple */

    LedOff(RED3);
    LedOff(GREEN2);
    LedOff(BLUE1);
    LedOff(BLUE0);
    LedOff(RED0);


    if(u8Counter & 0x01)
    {
      LedOn(RED3);
    }

    if(u8Counter & 0x02)
    {
      LedOn(GREEN2);
    }

    if(u8Counter & 0x04)
    {
      LedOn(BLUE1);
    }

    if(u8Counter & 0x08)
    {
      LedOn(RED0);
      LedOn(BLUE0);
    }
#endif /* First exercise */
  } /* end if(u16BlinkCount == 250) */
     
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
