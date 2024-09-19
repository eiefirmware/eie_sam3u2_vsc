/*!*********************************************************************************************************************
@file blade_imu_lsm6dsl.c                                                                
@brief Driver for ST LSM6DSL IMU.

Provides configuration service and up to 1kHz of 3-axis acceleration, gyro, and compass data.

----------------------------------------------------------------------------------------------------------------------

BLADE TASK MAIN FUNCTION CALLS
  Bladelsm6dslInitialize();
  Bladelsm6dslActiveState();

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- G_u32Bladelsm6dslData holds the current IMU data (updated per U16_MEASUREMENT_RATE_MS)

CONSTANTS
- NONE

TYPES
- lsm6dslDataType

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void Bladelsm6dslInitialize(void)
- void Bladelsm6dslRunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"


/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Bladelsm6dsl"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32Bladelsm6dslFlags;                      /*!< @brief Global state flags */
lsm6dslDataType G_u32Bladelsm6dslData;                    /*!< @brief Globally available IMU data */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Bladelsm6dsl_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Bladelsm6dsl_pfStateMachine;           /*!< @brief The state machine function pointer */
static u32 Bladelsm6dsl_u32Timeout;                       /*!< @brief Timeout counter used across states */


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
@fn void Bladelsm6dslInitialize(void)

@brief
Initializes the State Machine and its variables. Should only be called once in main init section.
Queries the IMU to check ID byte, then proceeds to send configuration data.

Note: Defaults for CTRL3_C, CTRL4_C, CTRL5_C, CTRL6_C, CTRL7_G, CTRL8_XL,
      CTRL9_X, CTRL10_C, and MASTER_CONFIG are ok and thus not configured.

The IMU is set for basic operation with values updated at 12.5Hz.  No interrupts or special
functionality is configured -- see the configuration registers for options.  The FIFO is
also not enabled. The idea is that the device will be polled at an interval maybe 10x per second
to update and display the data received.  This rate *should* be ok for the LCD refresh and/or
debug output.

Requires:
- Blade pins (at least for I²C comms) are configured)

Promises:
- Presence of IMU is determined; if present, default configuration is set up.

*/
void Bladelsm6dslInitialize(void)
{
  u8 u8RxMessage = 0;
  u8 au8TxMessage[] = {U8_CTRL1_XL, U8_CTRL1_XL_INIT, U8_CTRL2_G_INIT};
  ErrorStatusType eErrorStatus = SUCCESS;

  /* Initialize the IMU data global */
  G_u32Bladelsm6dslData.u8TempL   = 0;
  G_u32Bladelsm6dslData.u8TempH   = 0;
  G_u32Bladelsm6dslData.u8GyroXL  = 0;
  G_u32Bladelsm6dslData.u8GyroXH  = 0;
  G_u32Bladelsm6dslData.u8GyroYL  = 0;
  G_u32Bladelsm6dslData.u8GyroYH  = 0;
  G_u32Bladelsm6dslData.u8GyroZL  = 0;
  G_u32Bladelsm6dslData.u8GyroZH  = 0;
  G_u32Bladelsm6dslData.u8AccelXL = 0;
  G_u32Bladelsm6dslData.u8AccelXH = 0;
  G_u32Bladelsm6dslData.u8AccelYL = 0;
  G_u32Bladelsm6dslData.u8AccelYH = 0;
  G_u32Bladelsm6dslData.u8AccelZL = 0;
  G_u32Bladelsm6dslData.u8AccelZH = 0;
  
  /* Blade resource requests: I2C SCL, SDA, IO2 and IO3 interrupt lines */
  eErrorStatus += BladeRequestPin(BLADE_PIN2, DIGITAL_IN);
  eErrorStatus += BladeRequestPin(BLADE_PIN3, DIGITAL_IN);
  eErrorStatus += BladeRequestPin(BLADE_PIN8, PERIPHERAL);
  eErrorStatus += BladeRequestPin(BLADE_PIN9, PERIPHERAL);
  if(eErrorStatus)
  {
    DebugPrintf("LSM6DSL Blade pin resources not available\n\r"); 
  }
  else
  {
    DebugPrintf("LSM6DSL Blade pin resources allocated\n\r"); 
  }
  
  /* Ping the accelerometer to check it's responding by reading its ID byte */
  TwiWriteReadData(U8_LSM6DSL_I2C_ADDRESS, U8_WHO_AM_I, &u8RxMessage, 1);
  if(u8RxMessage != U8_LSM6DSL_ID)
  {
    eErrorStatus = ERROR;
  }
  else
  {
    /* Report ID returned */
    DebugPrintf("LSM6DSL returned ID: ");
    DebugPrintNumber(u8RxMessage);
    DebugLineFeed();
    
    /* Send basic configuration - the two registers are sequential so can be done in a single write */
    TwiWriteData(U8_LSM6DSL_I2C_ADDRESS, 3, au8TxMessage, TWI_STOP);
  }
 
  /* If good initialization, set state to Idle */
  if( eErrorStatus == SUCCESS )
  {
    Bladelsm6dsl_u32Timeout = G_u32SystemTime1ms;
    Bladelsm6dsl_pfStateMachine = Bladelsm6dslSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    Bladelsm6dsl_pfStateMachine = Bladelsm6dslSM_Error;
  }

} /* end Bladelsm6dslInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void Bladelsm6dslRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void Bladelsm6dslRunActiveState(void)
{
  Bladelsm6dsl_pfStateMachine();

} /* end Bladelsm6dslRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Read the IMU every U32_MEASUREMENT_RATE_MS and send out the values on the debug port.
Currently this makes no attempt to format or process the values.
Be careful with data processing -- if you refresh the IMU at too fast an interval, the TWI message system
will be overwhelmed.  Similarily, if you send the results out the debug port (or to the LCD) too quickly,
the messaging system will get overwhelmed.  */
static void Bladelsm6dslSM_Idle(void)
{
  static u8 au8Outputmessage[] = "00000 00000 00000 00000 00000 00000 00000\n\r";
  u32 u32Number;
  u8 au8NumberString[11];
  u8* pu8NumberIndex;
  u8* pu8NumberParser;
  u8* pu8StringParser;
  u8  u8Digits;
  
  /* Read the latest IMU data if it's time */
  if( IsTimeUp(&Bladelsm6dsl_u32Timeout, U32_MEASUREMENT_RATE_MS) )
  {
    Bladelsm6dsl_u32Timeout = G_u32SystemTime1ms;
    TwiWriteReadData(U8_LSM6DSL_I2C_ADDRESS, U8_OUT_TEMP_L, &G_u32Bladelsm6dslData.u8TempL, 14);
    
    /* Write data to debug this will be 1 write behind newest data since the command will not yet be queued */
    pu8NumberIndex = &G_u32Bladelsm6dslData.u8TempL;
    pu8StringParser = au8Outputmessage;
    
    for(u8 i = 0; i < 7; i++)
    {
      u32Number = 0;
      u32Number |= (u32)*pu8NumberIndex;
      pu8NumberIndex++;
      u32Number |= ((u32)(*pu8NumberIndex << 8)) & 0x0000FF00;
      pu8NumberIndex++;
      u8Digits = NumberToAscii(u32Number, au8NumberString);
      pu8NumberParser = au8NumberString;
      
      /* Copy ASCII number into result string including leading 0s */
      u8Digits = 5 - u8Digits;
      for(u8 j= 0; j < u8Digits; j++)
      {
        *pu8StringParser = '0';
        pu8StringParser++;
      }
      u8Digits = 5 - u8Digits;
      for(u8 j = 0; j < u8Digits; j++)
      {
        *pu8StringParser = *pu8NumberParser;
        pu8StringParser++;
        pu8NumberParser++;
      }
      
      /* Skip the space */
      pu8StringParser++;
    } /* end for(u8 i = 0; i < 7; i++) */
    
    DebugPrintf(au8Outputmessage);
  } /* end if( IsTimeUp(&Bladelsm6dsl_u32Timeout, U32_MEASUREMENT_RATE_MS) ) */
  
} /* end Bladelsm6dslSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void Bladelsm6dslSM_Error(void)          
{
  
} /* end Bladelsm6dslSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
