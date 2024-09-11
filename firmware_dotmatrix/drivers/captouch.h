/*!**********************************************************************************************************************
@file captouch.h                                                               

@brief Header file for captouch.c

***********************************************************************************************************************/

#ifndef __CAPTOUCH_H
#define __CAPTOUCH_H


/***********************************************************************************************************************
Type Definitions
***********************************************************************************************************************/

/*! 
@enum SliderNumberType
@brief Controlled list of available Slider channels used in the member functions. 
*/
typedef enum {SLIDER0, SLIDER1} SliderNumberType;


/***********************************************************************************************************************
* Function Declarations
***********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*-------------------------------------------------------------------------------------------------------------------*/
void CapTouchOn(void);
void CapTouchOff(void);
u8 CaptouchCurrentHSlidePosition(void);
u8 CaptouchCurrentVSlidePosition(void);


/*-------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*-------------------------------------------------------------------------------------------------------------------*/
ErrorStatusType CapTouchInitialize(void);
void CapTouchRunActiveState(void);
BOOL CapTouchUpdateSensorReadings(u16 u16CurrentTimeMs_);
u8 u8CapTouchGetSliderValue(SliderNumberType eSliderNumber_);


/*-------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*-------------------------------------------------------------------------------------------------------------------*/
static ErrorStatusType CapTouchVerify(void);
static void CapTouchSetParameters(void);
static void CapTouchGetDebugValues(u8 u8Channel_, u16* pu16Measure_, u16* pu16Reference_);


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
void CapTouchSM_Idle(void);             
void CapTouchSM_Measure(void);

void CapTouchSM_Error(void);         


/***********************************************************************************************************************
Constants / Definitions
***********************************************************************************************************************/
#define QTOUCH_MEASUREMENT_TIME_MS  (u32)25      /*!< @brief Interval in ms between captouch sweeps */
#define QTOUCH_MEASUREMENT_TIMEOUT  (u32)100     /*!< @brief Interval in ms between captouch sweeps */
#define QTOUCH_DEBUG_PRINT_PERIOD   (u32)200     /*!< @brief Interval in ms between printing captouch current values */



#endif /* __CAPTOUCH_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
