/*!**********************************************************************************************************************
@file captouch.c

@brief Driver for Capacitive Touch sensors. 

This code supports the sliders on the EIE dot matrix board and any future buttons/sliders that 
may be added.  The majority of Captouch functionality is provided by the Atmel QT library.
The sensor read function violates system timing, but as it is not open-source nothing
can be done about that.  The Captouch driver is turned off by default to mitigate this.
If an application requires Captouch sensing, then the whole application must account
for the expected 1ms loop timing disruption.
  
------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- void CapTouchOn(void)
- void CapTouchOff(void)
- u8 CaptouchCurrentHSlidePosition(void)
- u8 CaptouchCurrentVSlidePosition(void)

PROTECTED FUNCTIONS
- ErrorStatusType CapTouchInitialize(void)
- void CapTouchRunActiveState(void)
- ErrorStatusType CapTouchVerify(void)


***********************************************************************************************************************/

#include "configuration.h"
#include "libqtouch.h"
#ifndef EIE_NO_CAPTOUCH

#define GET_SENSOR_STATE(SENSOR_NUMBER) (qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] & (1 << (SENSOR_NUMBER % 8)))
#define GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) qt_measure_data.qt_touch_status.rotor_slider_values[ROTOR_SLIDER_NUMBER]


/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>CapTouch"
***********************************************************************************************************************/
/* New variables */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                 /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                  /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;             /*!< @brief From main.c */

/* Measurement data */
extern qt_touch_lib_measure_data_t qt_measure_data;    /*!< @brief From touch_api.h */

extern u32 G_u32DebugFlags;                            /*!< @brief From debug.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "CapTouch_<type>" and be declared as static.
***********************************************************************************************************************/
/* New variables */
static fnCode_type CapTouch_pfnStateMachine;           /*!< @brief The state machine function pointer */

static u32 CapTouch_u32Timer;                          /*!< @brief Measurement timer */        
static u8 CapTouch_u8CurrentHSliderValue;              /*!< @brief Current horizontal slider value */
static u8 CapTouch_u8CurrentVSliderValue;              /*!< @brief Current vertical slider value */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!---------------------------------------------------------------------------------------------------------------------
@fn void CapTouchOn(void)

@brief Sets the state machine to "Measuring" so that the captouch functionality is active.  This is fairly processor intensive.

Requires:
- NONE

Promises:
- Horizontal CapTouch sensor is active

*/
void CapTouchOn(void)
{
  CapTouch_pfnStateMachine = CapTouchSM_Measure;
  
} /* end CapTouchOn() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn void CapTouchOff(void)

@brief Sets the state machine to "Idle" so that the captouch functionality is disabled.

Requires:
- NONE

Promises:
- Horizontal CapTouch sensor is not active

*/
void CapTouchOff(void)
{
  CapTouch_pfnStateMachine = CapTouchSM_Idle;
  CapTouch_u8CurrentHSliderValue = 0;
  CapTouch_u8CurrentVSliderValue = 0;
  
} /* end CapTouchOff() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn u8 CaptouchCurrentHSlidePosition(void)

@brief Returns the current slider count (0-254)

Requires:
- NONE

Promises:
  - Returns the current slider count (0-254)

*/
u8 CaptouchCurrentHSlidePosition(void)
{
  return CapTouch_u8CurrentHSliderValue;
  
} /* end CaptouchCurrentHSlidePosition() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn u8 CaptouchCurrentVSlidePosition(void)

@brief Returns the current slider count (0-254)

Requires:
- NONE

Promises:
- Returns the current slider count (0-254)

*/
u8 CaptouchCurrentVSlidePosition(void)
{
  return CapTouch_u8CurrentVSliderValue;
  
} /* end CaptouchCurrentVSlidePosition() */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn ErrorStatusType CapTouchInitialize(void)

@brief Initializes the Captouch library. 

The library version is checked, the buttons being used are initialized, and the configuration parameters are set.

The qt_xxx functions are part of the QTouch API.

Requires:
- NONE

Promises:
- Returns ERROR if the library doesn't match what's expected, else SUCCESS

*/
ErrorStatusType CapTouchInitialize(void)
{
  DebugPrintf("Initializing CapTouch...\n\r");
  
	/* Ensure library is correct */
	if (CapTouchVerify() != SUCCESS)
	{
    DebugPrintf("CapTouch library did not verify\n\r");
		return (ERROR);
	}

	/* Reset touch sensing */
	qt_reset_sensing();

  /* Configure the GPIO used for the Horizontal Slider */
  qt_enable_slider( CHANNEL_0, CHANNEL_2, NO_AKS_GROUP, 50u, HYST_50, RES_8_BIT, 0u  );

  /* Configure the GPIO used for the Vertical Slider */
  qt_enable_slider( CHANNEL_19, CHANNEL_21, NO_AKS_GROUP, 50u, HYST_50, RES_8_BIT, 0u  );

  /* Initialise and calibrate touch sensing (all sensors must be enabled first)*/
  qt_init_sensing();
  qt_calibrate_sensing();

  /* Set the parameters like recalibration threshold, Max_On_Duration etc .
  These are user configurable */
  CapTouchSetParameters( );

  /* Address to pass address of user functions */
  /* This function is called after the library has made capacitive measurements,
  but before it has processed them. The user can use this hook to apply filter
  functions to the measured signal values.(Possibly to fix sensor layout faults) */
  qt_filter_callback = 0;

  CapTouch_u32Timer = 0;
  CapTouch_u8CurrentHSliderValue = 0;
  CapTouch_u8CurrentVSliderValue = 0;
  
  CapTouch_pfnStateMachine = CapTouchSM_Idle;
  G_u32ApplicationFlags |= _APPLICATION_FLAGS_CAPTOUCH;
  return (SUCCESS);
  
} /* end CapTouchInitialize() */


/*!--------------------------------------------------------------------------------------------------------------------
@fn void CapTouchRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void CapTouchRunActiveState(void)
{
  CapTouch_pfnStateMachine();

} /* end CapTouchRunActiveState */


/*!----------------------------------------------------------------------------------------------------------------------
@fn bool CapTouchUpdateSensorReadings(u16 u16CurrentTimeMs_)

@brief This is the main read routine to get the cap touch sensor
readings. It should be called periodically every 25ms.

Requires:
@param u16CurrentTimeMs_ is the current value of the tick timer (ideally in increments of 25ms)

Promises:
- returns TRUE for successful read
- returns FALSE if read times out 

*/
BOOL CapTouchUpdateSensorReadings(u16 u16CurrentTimeMs_)
{
  u16 u16StatusFlag = 0;
  u16 u16BurstFlag  = 0;

  CapTouch_u32Timer = G_u32SystemTime1ms;
  
	do
	{
		/* Trigger sensor measurment */
		u16StatusFlag = qt_measure_sensors( u16CurrentTimeMs_ );

		u16BurstFlag = u16StatusFlag & QTLIB_BURST_AGAIN;

	} while ( u16BurstFlag && !IsTimeUp(&CapTouch_u32Timer, QTOUCH_MEASUREMENT_TIMEOUT) );

  if(IsTimeUp(&CapTouch_u32Timer, QTOUCH_MEASUREMENT_TIMEOUT))
  {
    DebugPrintf("\n\rCaptouch sensor read timeout\n\r");
    return(FALSE);
  }
  else
  {
    return(TRUE);
  }

} /* end CapTouchUpdateSensorReadings() */

            
/*!----------------------------------------------------------------------------------------------------------------------
@fn u8 u8CapTouchGetSliderValue(SliderNumberType eSliderNumber_)

@brief Call this function after a successful read from CapTouchUpdateSensorReadings
to obtain the value of a slider reading.

Requires:
@param eSliderNumber_ is the slider to read

Promises:
- returns 8-bit value for sensor reading

*/
u8 u8CapTouchGetSliderValue(SliderNumberType eSliderNumber_)
{
  return (GET_ROTOR_SLIDER_POSITION(eSliderNumber_));

} /* end u8CapTouchGetSliderValue() */


/*----------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*----------------------------------------------------------------------------------------------------------------------*/


/*!----------------------------------------------------------------------------------------------------------------------
@fn static ErrorStatusType CapTouchVerify(void)

@brief This function verifies the captouch library that's being used and makes sure
the capabilities (and version) match what's expected and required.

Requires:
- NONE

Promises:
- Returns ERROR if the library doesn't match what's expected, else SUCCESS

*/
static ErrorStatusType CapTouchVerify(void)
{
  u32 u32Error = 0;

  qt_lib_siginfo_t plib_sig;

  /* Read library signature info */
  qt_measure_sensors( 0 );
  qt_get_library_sig(&plib_sig);

  /* QTouch or Matrix */
  if((plib_sig.lib_sig_lword & 0x01) == 0)
  {
    DebugPrintf("Library QTouch ");
  }
  else
  {
    DebugPrintf("Library QMatrix ");
    u32Error++;
  }

  /* Max channels should be 32 */
  if ( ((plib_sig.lib_sig_lword >> 3) & 0x7F) != 32)
  {
    u32Error++;
  }

  /* Should supoprt sliders/roters */
  if( ((plib_sig.lib_sig_lword >> 10) & 0x01) == 0)
  {
    DebugPrintf("supports only keys \r\n");
    u32Error++;
  }
  else
  {
    DebugPrintf("supports keys and rotors/sliders. ");
  }

  /* Should supoprt 8 sliders */
  if ( ((plib_sig.lib_sig_lword >> 11) & 0x1F) != 8 )
  {
    u32Error++;
  }

  /* Current version: $0440 */
  DebugPrintf("Version ");
  DebugPrintNumber(plib_sig.library_version);
  DebugLineFeed();
  if ( (plib_sig.library_version) != 0x440)
  {
    u32Error++;
  }

  /* if error count = 0, all is good */
  if (u32Error == 0)
  {
    return (SUCCESS);
  }

  return (ERROR);
  
} /* end CapTouchVerify() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn static void CapTouchSetParameters(void)

@brief Fills the default threshold values in the configuration
data structure. These values can be tweaked to optimize performance.

Requires:
- DEF_QT_xxx values are defined in touch_api.h

Promises:
- qt_config_data struct initialized for board operation

*/
static void CapTouchSetParameters(void)
{
  /*  This can be modified by the user to different values   */
  qt_config_data.qt_di              = DEF_QT_DI;
  qt_config_data.qt_neg_drift_rate  = DEF_QT_NEG_DRIFT_RATE;
  qt_config_data.qt_pos_drift_rate  = DEF_QT_POS_DRIFT_RATE;
  qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
  qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
  qt_config_data.qt_recal_threshold = DEF_QT_RECAL_THRESHOLD;
  qt_config_data.qt_pos_recal_delay = DEF_QT_POS_RECAL_DELAY;
    
} /* end CapTouchSetParameters() */

#if 0
/*!----------------------------------------------------------------------------------------------------------------------
@fn static void CapTouchGetDebugValues(u8 u8Channel_, u16* pu16Measure_, u16* pu16Reference_)

@brief Call this function after a successful read to obtain the raw channel measurement
and reference value. This will indicate what the 'normal' values should be when
the sensor is idle/not active (no finger present) along with the current reading.

Requires:
@param u8Channel_ The channel to verify !!!!! What channels are valid?
@param pu16Measure_ Pointer to return measurement result
@param pu16Reference_ Pointer to return reference result

Promises:
- *pu16Measure_ returns the value of the current measurement
- *pu16Reference_ returns the normal, untouched value

*/
static void CapTouchGetDebugValues(u8 u8Channel_, u16* pu16Measure_, u16* pu16Reference_)
{
  *pu16Measure_ = qt_measure_data.channel_signals[u8Channel_];
  *pu16Reference_ = qt_measure_data.channel_references[u8Channel_];

} /* end CapTouchGetDebugValues() */
#endif

/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn void CapTouchSM_Idle(void)

@brief Do nothing as the sensors are off.  The state will get changed when sensors
are activated.
*/
void CapTouchSM_Idle(void)
{
  
} /* end CapTouchSM_Idle() */


/*!-------------------------------------------------------------------------------------------------------------------
@fn void CapTouchSM_Measure(void)

@brief Run sensor measurements at the defined rate.
*/
void CapTouchSM_Measure(void)
{
  static u32 u32DebugPrintTimer = 0;
  
  if( IsTimeUp(&CapTouch_u32Timer, QTOUCH_MEASUREMENT_TIME_MS) )
  {
    CapTouch_u32Timer = G_u32SystemTime1ms;
    CapTouchUpdateSensorReadings((u16)G_u32SystemTime1ms);

    /* Read the horizontal slider */
    CapTouch_u8CurrentHSliderValue = u8CapTouchGetSliderValue(SLIDER0);
    
    /* Read the vertical slider */
    CapTouch_u8CurrentVSliderValue = u8CapTouchGetSliderValue(SLIDER1);
  }
  
  /* Print the current values if Debug function is enabled */
  if(G_u32DebugFlags & _DEBUG_CAPTOUCH_VALUES_ENABLE)
  {
    if( IsTimeUp(&u32DebugPrintTimer, QTOUCH_DEBUG_PRINT_PERIOD) )
    {
      u32DebugPrintTimer = G_u32SystemTime1ms;

      DebugPrintf("Captouch (H:V) ");
      DebugPrintNumber(CapTouch_u8CurrentHSliderValue);
      DebugPrintf(":");
      DebugPrintNumber(CapTouch_u8CurrentVSliderValue);
      DebugLineFeed();
    }
  }
               
} /* end CapTouchSM_Measure() */
            
#endif /* EIE_NO_CAPTOUCH */
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/