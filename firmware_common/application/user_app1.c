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
void switch_led_L2R(int number){
  LedOn(number);
  LedOff(number-1);
}

void switch_led_R2L(int number){
  
  if(number == 7){
    LedOn(number);
  }
  
  else{
    LedOn(number);
    LedOff(number+1);
  }
}

void led_all_on_off(void){
  int led;

  for(led = 0; led <= 7; led++)
    LedOn(led);

  for(led = 7; led >= 0; led--)
    LedOff(led);
}

void led_all_on(void){
  int led;
  for(led=0;led<=7;led++)
    LedOn(led);
}

void led_all_off(void){
  int led;
  for(led=7;led>=0;led--){
    LedOff(led);
  }
}

void back_light_colour(int colour_num){
  switch (colour_num){
    case 0:
      LedOn(LCD_BLUE);
      LedOn(LCD_GREEN);
      LedOn(LCD_RED);
      break;
  
    case 1:
      LedOff(LCD_BLUE);
      LedOn(LCD_GREEN);
      LedOn(LCD_RED);
      break;

    case 2:
      LedOn(LCD_BLUE);
      LedOff(LCD_GREEN);
      LedOn(LCD_RED);
      break;

    case 3:
      LedOn(LCD_BLUE);
      LedOn(LCD_GREEN);
      LedOff(LCD_RED);
      break;

    case 4:
      LedOff(LCD_BLUE);
      LedOff(LCD_GREEN);
      LedOn(LCD_RED);
      break;

    case 5:
      LedOff(LCD_BLUE);
      LedOn(LCD_GREEN);
      LedOff(LCD_RED);
      break;
  
    case 6:
      LedOn(LCD_BLUE);
      LedOff(LCD_GREEN);
      LedOff(LCD_RED);
      break;
  
    case 7:
      LedOff(LCD_BLUE);
      LedOff(LCD_GREEN);
      LedOn(LCD_RED);
      break;
    
    case 8:
      LedOff(LCD_BLUE);
      LedOff(LCD_GREEN);
      LedOff(LCD_RED);
      break;

    default:
      break;
  }
}

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
  /* If good initialization, set state to Idle */
  if(1)
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
    HEARTBEAT_OFF();
    led_all_on_off();
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
/* What does this state do? */
static void UserApp1SM_Idle(void)
{
  static u16 u16Counter = U16_COUNTER_PERIOD_MS; //Time is initialized

  static bool bLightIsOn = FALSE;
  static bool L2R = TRUE, Change_led = TRUE;
  static int lcd_number = 0, led_number = 0;

  u16Counter--;
  
  if (u16Counter == 0){
    u16Counter = U16_COUNTER_PERIOD_MS;
    
    if (bLightIsOn){
      HEARTBEAT_OFF();
      bLightIsOn = FALSE;
    }
  
    else{
      HEARTBEAT_ON();
      bLightIsOn = TRUE;
    }

    if(Change_led){

      if(led_number == 7){
        switch_led_L2R(led_number);
        L2R = FALSE;
      }
    
      if(led_number == 0){
        switch_led_R2L(led_number);
        L2R = TRUE;
      }
    
      if(L2R){
        switch_led_L2R(led_number);
        led_number++;
      }

      else{
        switch_led_R2L(led_number);
        led_number--;
      } 
    }
  }

  if (WasButtonPressed(BUTTON0)){

    ButtonAcknowledge(BUTTON0);

    if(lcd_number<=8){
      back_light_colour(lcd_number);
      lcd_number++;
    }

    else{
      lcd_number=0;
      back_light_colour(lcd_number);
      lcd_number++;
    }
  }

  if(WasButtonPressed(BUTTON1)){
      
    ButtonAcknowledge(BUTTON1);

    if(Change_led){
      Change_led = FALSE;
      L2R = TRUE;
      led_number = 0;
      led_all_off();
    }

    else
    Change_led = TRUE;
  }
    
    /*
    if(Change_led){
      if(led_number == 7){
        switch_led_L2R(led_number);
        L2R = FALSE;
      }
    
      if(led_number == 0){
        switch_led_R2L(led_number);
        L2R = TRUE;
      }
    
      if(L2R){
        switch_led_L2R(led_number);
        led_number++;
      }

      else{
        switch_led_R2L(led_number);
        led_number--;
      }
    }*/
}
 /* end UserApp1SM_Idle() */


/*------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
