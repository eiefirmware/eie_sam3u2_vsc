/*!*********************************************************************************************************************
@file blade_api.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

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
- void BladeApiInitialize(void)
- void BladeApiRunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>BladeApi"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32BladeApiFlags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "BladeApi_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type BladeApi_pfStateMachine;                     /*!< @brief The state machine function pointer */

static BladePinIOType BladeApi_auePinAllocated[U8_BLADE_PINS];  /*!< @brief Which Blade pins have been requested */

static const u32 BladeApi_au32BladePins[U8_BLADE_PINS] = 
{
  BLADE_PIN_AN0, BLADE_PIN_AN1,                    
  BLADE_PIN_UPIMO, BLADE_PIN_UPOMI,                
  BLADE_PIN_MISO, BLADE_PIN_MOSI, BLADE_PIN_SCK, BLADE_PIN_CS,                     
  BLADE_PIN_SDA, BLADE_PIN_SCL
};

static const u32 BladeApi_au32BladePinPorts[U8_BLADE_PINS] = 
{
  BLADE_PIN_AN0_PORT, BLADE_PIN_AN1_PORT,                    
  BLADE_PIN_UPIMO_PORT, BLADE_PIN_UPOMI_PORT,                
  BLADE_PIN_MISO_PORT, BLADE_PIN_MOSI_PORT, BLADE_PIN_SCK_PORT, BLADE_PIN_CS_PORT,                     
  BLADE_PIN_SDA_PORT, BLADE_PIN_SCL_PORT
};

static const u32 BladeApi_au32BladePinPeripherals[U8_BLADE_PINS] = 
{
  BLADE_PIN_AN0_PERIPH, BLADE_PIN_AN1_PERIPH,                    
  BLADE_PIN_UPIMO_PERIPH, BLADE_PIN_UPOMI_PERIPH,                
  BLADE_PIN_MISO_PERIPH, BLADE_PIN_MOSI_PERIPH, BLADE_PIN_SCK_PERIPH, BLADE_PIN_CS_PERIPH,                     
  BLADE_PIN_SDA_PERIPH, BLADE_PIN_SCL_PERIPH
};


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn ErrorStatusType BladeRequestPin(BladePinType ePin_, BladePinIOType ePinFunction_)

@brief Requests a pin to be used by a Blade task.

Only main pin functionality is controlled here (GPIO input or output,
or peripheral function). Hardware filters, pull resistors, or other special 
features can be enabled by the application if required.

Since Blade connections are standard, it is assumed the user understands
the functions available for each pin.

BLADE_PIN0: GPIO 0 or Analog 0            
BLADE_PIN1: GPIO 1 or Analog 1                    
BLADE_PIN2: GPIO 2 or UART PIMO                
BLADE_PIN3: GPIO 3 or UART POMI                
BLADE_PIN4: GPIO 4 or SPI MISO                  
BLADE_PIN5: GPIO 5 or SPI MOSI                  
BLADE_PIN6: GPIO 6 or SPI SCK                    
BLADE_PIN7: GPIO 7 or SPI CS                      
BLADE_PIN8: GPIO 8 or I2C SDA              
BLADE_PIN9: GPIO 9 or I2C SCL                      

Requests for I2C pin peripheral function are not denied if the pin is already requested
as the peripheral function.

Requires:
@param ePin_ is one of the 10 Blade pins to request and configure
@param ePinFunction_ specifies the desired configuration

Promises:
- Returns SUCCESS if pin is available; BladeApi_au8PinAllocated[ePin_] is
  set to the type of allocation and the pin is configured according to ePinFunction_
- Returns ERROR if pin has already been allocated

*/
ErrorStatusType BladeRequestPin(BladePinType ePin_, BladePinIOType ePinFunction_)
{
  /* Check special case of I2C PERIPHERAL pins */
  if( (BladeApi_au32BladePins[ePin_] == BLADE_PIN_SDA) ||
      (BladeApi_au32BladePins[ePin_] == BLADE_PIN_SCL) )
  {
    if(BladeApi_auePinAllocated[ePin_] == PERIPHERAL)
    {
      /* I2C pins already configured for PERIPHERAL do not need to be reconfigured; 
      user is alerted to the shared config and assigment is considered successful. */
      DebugPrintf("Blade I2C pin assigned as shared\n\r");
      return SUCCESS;
    }
  }
  
  /* Check if the requested pin is available */
  if(BladeApi_auePinAllocated[ePin_] != PIN_NOT_ASSIGNED)
  {
    DebugPrintf("Request denied: Blade pin ");
    DebugPrintNumber((u32)ePin_);
    DebugPrintf(" already allocated\n\r");
    
    return ERROR;
  }
  
  /* Pin is available, so claim it and configure */
  BladeApi_auePinAllocated[ePin_] = ePinFunction_;
  (AT91C_BASE_PIOA + BladeApi_au32BladePinPorts[ePin_])->PIO_PDR = BladeApi_au32BladePins[ePin_];
  
  /* Set IO or peripheral */
  if(ePinFunction_ == PERIPHERAL)
  {
    /* PIO does not control pin */
    (AT91C_BASE_PIOA + BladeApi_au32BladePinPorts[ePin_])->PIO_PDR = BladeApi_au32BladePins[ePin_];
    
    /* Select the peripheral function location */
    if(BladeApi_au32BladePinPeripherals[ePin_] == PERIPHERAL_A)
    {
      (AT91C_BASE_PIOA + BladeApi_au32BladePinPorts[ePin_])->PIO_ABSR &= ~BladeApi_au32BladePins[ePin_];
    }
    else
    {
      (AT91C_BASE_PIOA + BladeApi_au32BladePinPorts[ePin_])->PIO_ABSR |= BladeApi_au32BladePins[ePin_];
    }
  }
  /* Otherwise the pin is digital IO */
  else
  {
    /* PIO controller has pin */
    (AT91C_BASE_PIOA + BladeApi_au32BladePinPorts[ePin_])->PIO_PER = BladeApi_au32BladePins[ePin_];
    
    /* Set input or output */
    if(ePinFunction_ == DIGITAL_IN)
    {
      (AT91C_BASE_PIOA + BladeApi_au32BladePinPorts[ePin_])->PIO_ODR = BladeApi_au32BladePins[ePin_];
    }
    else
    {
      (AT91C_BASE_PIOA + BladeApi_au32BladePinPorts[ePin_])->PIO_OER = BladeApi_au32BladePins[ePin_];
    }
  }

  DebugPrintNumber((u32)ePin_);
  DebugPrintf(" pin Blade assigned\n\r");
  return SUCCESS;
  
} /* end BladeRequestPin() */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void BladeApiInitialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- All pin states in BladeApi_au8PinAllocated are set to FALSE
- State machine to Idle

*/
void BladeApiInitialize(void)
{
  /* Deallocate all blade pins */
  for(u8 i = 0; i < U8_BLADE_PINS; i++)
  {
    BladeApi_auePinAllocated[i] = PIN_NOT_ASSIGNED;
  }
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    DebugPrintf("Blade task initialized\n\r");
    G_u32ApplicationFlags |= _APPLICATION_FLAGS_BLADE;
    BladeApi_pfStateMachine = BladeApiSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    BladeApi_pfStateMachine = BladeApiSM_Error;
  }

} /* end BladeApiInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void BladeApiRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void BladeApiRunActiveState(void)
{
  BladeApi_pfStateMachine();

} /* end BladeApiRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Currently, the Blade task does not do anything beyond provide configuration
resources.  Options of an Idle state could include pinging configured blades
or monitoring conformance to system task time.  */
static void BladeApiSM_Idle(void)
{
    
} /* end BladeApiSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void BladeApiSM_Error(void)          
{
  
} /* end BladeApiSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
