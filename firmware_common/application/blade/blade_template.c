/*!*********************************************************************************************************************
@file blade_template.c                                                                
@brief Blade tasks / applications are written here.  Blade tasks must 
conform to the Initialization and 1ms program timing rules! 
This description should be replaced by something specific to the task.  

----------------------------------------------------------------------------------------------------------------------
To start a new Blade task using this blade_template:
 1. Copy both blade_template.c and blade_template.h to the \application\blade folder
 2. Rename the files blade_taskname.c and blade_taskname.h where "taskname" should
    match the Blade hardware version
 3. Add blade_taskname.c and blade_taskname.h to the Application>Blades Include and 
    Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances 
    of "blade_template" with "blade_taskname"
 5. Use ctrl-h to find and replace all instances of "BladeTemplate" with "BladeTaskName"
 6. Use ctrl-h to find and replace all instances of "BLADE_TEMPLATE" with "BLADE_NEW_TASK_NAME"
 7. If using the new task, add a call to BladeTaskNameInitialize() in the init section of main
 8. If using the new task, ad a call to BladeTaskNameRunActiveState() in the Super Loop section of main
 9. Update blade_taskname.h per the instructions at the top of blade_taskname.h
10. Delete this text (between the dashed lines) and update the description to describe your task

Note: prior to pushing the completed Blade application to the Master Branch, the
Initialization and Main loop function calls should be removed from main.  
----------------------------------------------------------------------------------------------------------------------

BLADE TASK MAIN FUNCTION CALLS
  BladeTemplateInitialize();
  BladeTemplateActiveState();

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
- void BladeTemplateInitialize(void)
- void BladeTemplateRunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>BladeTemplate"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32BladeTemplateFlags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "BladeTemplate_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type BladeTemplate_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 BladeTemplate_u32Timeout;                           /*!< @brief Timeout counter used across states */


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
@fn void BladeTemplateInitialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void BladeTemplateInitialize(void)
{
  u8 u8ResourcesAvailable = 0;
  
  /* Blade resource requests */
  u8ResourcesAvailable += Blade
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    BladeTemplate_pfStateMachine = BladeTemplateSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    BladeTemplate_pfStateMachine = BladeTemplateSM_Error;
  }

} /* end BladeTemplateInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void BladeTemplateRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void BladeTemplateRunActiveState(void)
{
  BladeTemplate_pfStateMachine();

} /* end BladeTemplateRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void BladeTemplateSM_Idle(void)
{
    
} /* end BladeTemplateSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void BladeTemplateSM_Error(void)          
{
  
} /* end BladeTemplateSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
