/*!*********************************************************************************************************************
@file blade_imu_lsm6dsl.h                                                                
@brief Header file for Blade IMU using LSM6DSL.

**********************************************************************************************************************/

#ifndef __BLADE_LSM6DSL_H
#define __BLADE_LSM6DSL_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
/*! 
@struct lsm6dslDataType
@brief Message-specific information 
*/
typedef struct
{
  u8 u8TempL;               /*!< @brief Temperature output low byte */
  u8 u8TempH;               /*!< @brief Temperature output high byte  */
  u8 u8GyroXL;              /*!< @brief Gyro X low byte */
  u8 u8GyroXH;              /*!< @brief Gyro X high byte */
  u8 u8GyroYL;              /*!< @brief Gyro Y low byte */
  u8 u8GyroYH;              /*!< @brief Gyro Y high byte */
  u8 u8GyroZL;              /*!< @brief Gyro Z low byte */
  u8 u8GyroZH;              /*!< @brief Gyro Z high byte */
  u8 u8AccelXL;             /*!< @brief Accelerometer X low byte */
  u8 u8AccelXH;             /*!< @brief Accelerometer X high byte */
  u8 u8AccelYL;             /*!< @brief Accelerometer Y low byte */
  u8 u8AccelYH;             /*!< @brief Accelerometer Y high byte */
  u8 u8AccelZL;             /*!< @brief Accelerometer Z low byte */
  u8 u8AccelZH;             /*!< @brief Accelerometer Z high byte */
} lsm6dslDataType;


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void Bladelsm6dslInitialize(void);
void Bladelsm6dslRunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void Bladelsm6dslSM_Idle(void);    
static void Bladelsm6dslSM_Error(void);         



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
#define U32_MEASUREMENT_RATE_MS               (u32)100  /*!< @brief Rate at which IMU data is updated to G_u32Bladelsm6dslData*/

#define U8_LSM6DSL_I2C_ADDRESS                (u8)0x6b  /*!< @brief I2C address (assumes SDO tied high) */
#define U8_LSM6DSL_ID                         (u8)0x6a  /*!< @brief Expected Who I am returned ID */     

/* Required setup values */
#define U8_CTRL1_XL_INIT                      (u8)0x1c
/*
  [7-4]: b'0001' Accelerometer output data rate and power mode selection: 12.5Hz
  [3-2]: b'11'   Accelerometer full-scale selection +/- 8g
    [1]: b'0'    Accelerometer digital filter bandwidth
    [0]: b'0'    Accelerometer analog chain bandwidth selection 1.5kHz
*/

#define U8_CTRL2_G_INIT                       (u8)0x18
/*
  [7-4]: b'0001' Gyro output data rate and power mode selection: 12.5Hz
  [3-2]: b'10'   Gyro full-scale selection 1000 dps
    [1]: b'0'    Gyro full scape at 125 dps disabled
    [0]: b'0'    Must be 0
*/


/* LSM6DSL Registers */
#define U8_FUNC_CFG_ACCESS                    (u8)0x01  /*!< @brief Embedded functions configuration register*/ 
#define U8_SENSOR_SYNC_TIME_FRAME             (u8)0x04  /*!< @brief Sensor sync configuration register */ 
#define U8_SENSOR_SYNC_RES_RATIO              (u8)0x05  /*!< @brief Sensor sync configuration register */ 
#define U8_FIFO_CTRL1                         (u8)0x06  /*!< @brief FIFO configuration register */ 
#define U8_FIFO_CTRL2                         (u8)0x07  /*!< @brief FIFO configuration register */ 
#define U8_FIFO_CTRL3                         (u8)0x08  /*!< @brief FIFO configuration register */ 
#define U8_FIFO_CTRL4                         (u8)0x09  /*!< @brief FIFO configuration register */ 
#define U8_FIFO_CTRL5                         (u8)0x0a  /*!< @brief FIFO configuration register */ 
#define U8_DRDY_PULSE_CFG_G                   (u8)0x0b  /*!< @brief DataReady config register */ 
#define U8_INT1_CTRL                          (u8)0x0d  /*!< @brief INT1 pin control */ 
#define U8_INT2_CTRL                          (u8)0x0e  /*!< @brief INT2 pin control */ 
#define U8_WHO_AM_I                           (u8)0x0f  /*!< @brief Who I am ID */ 
#define U8_CTRL1_XL                           (u8)0x10  /*!< @brief Accelerometer and gyro control registers */ 
#define U8_CTRL2_G                            (u8)0x11  /*!< @brief Accelerometer and gyro control registers */ 
#define U8_CTRL3_C                            (u8)0x12  /*!< @brief Accelerometer and gyro control registers */ 
#define U8_CTRL4_C                            (u8)0x13  /*!< @brief Accelerometer and gyro control registers */ 
#define U8_CTRL5_C                            (u8)0x14  /*!< @brief Accelerometer and gyro control registers */ 
#define U8_CTRL6_C                            (u8)0x15  /*!< @brief Accelerometer and gyro control registers */ 
#define U8_CTRL7_G                            (u8)0x16  /*!< @brief Accelerometer and gyro control registers */ 
#define U8_CTRL8_XL                           (u8)0x17  /*!< @brief Accelerometer and gyro control registers */ 
#define U8_CTRL9_XL                           (u8)0x18  /*!< @brief Accelerometer and gyro control registers */ 
#define U8_CTRL10_C                           (u8)0x19  /*!< @brief Accelerometer and gyro control registers */ 
#define U8_MASTER_CONFIG                      (u8)0x1a  /*!< @brief I2C master config register */ 
#define U8_WAKE_UP_SRC                        (u8)0x1b  /*!< @brief Interrupt registers */ 
#define U8_TAP_SRC                            (u8)0x1c  /*!< @brief Interrupt registers */ 
#define U8_D6D_SRC                            (u8)0x1d  /*!< @brief Interrupt registers */ 
#define U8_STATUS_REG                         (u8)0x1e  /*!< @brief Status data register for UI */ 
#define U8_OUT_TEMP_L                         (u8)0x20  /*!< @brief Temperature output data registers */ 
#define U8_OUT_TEMP_H                         (u8)0x21  /*!< @brief Temperature output data registers */ 
#define U8_OUTX_L_G                           (u8)0x22  /*!< @brief Gyro output data register */ 
#define U8_OUTX_H_G                           (u8)0x23  /*!< @brief Gyro output data register */ 
#define U8_OUTY_L_G                           (u8)0x24  /*!< @brief Gyro output data register */ 
#define U8_OUTY_H_G                           (u8)0x25  /*!< @brief Gyro output data register */ 
#define U8_OUTZ_L_G                           (u8)0x26  /*!< @brief Gyro output data register */ 
#define U8_OUTZ_H_G                           (u8)0x27  /*!< @brief Gyro output data register */ 
#define U8_OUTX_L_XL                          (u8)0x28  /*!< @brief Accelerometer output data register */ 
#define U8_OUTX_H_XL                          (u8)0x29  /*!< @brief Accelerometer output data register */ 
#define U8_OUTY_L_XL                          (u8)0x2a  /*!< @brief Accelerometer output data register */ 
#define U8_OUTY_H_XL                          (u8)0x2b  /*!< @brief Accelerometer output data register */ 
#define U8_OUTZ_L_XL                          (u8)0x2c  /*!< @brief Accelerometer output data register */ 
#define U8_OUTZ_H_XL                          (u8)0x2d  /*!< @brief Accelerometer output data register */ 
#define U8_FIFO_STATUS1                       (u8)0x3a  /*!< @brief FIFO status register */ 
#define U8_FIFO_STATUS2                       (u8)0x3b  /*!< @brief FIFO status register */ 
#define U8_FIFO_STATUS3                       (u8)0x3c  /*!< @brief FIFO status register */ 
#define U8_FIFO_STATUS4                       (u8)0x3d  /*!< @brief FIFO status register */ 
#define U8_FIFO_DATA_OUT_L                    (u8)0x3e  /*!< @brief FIFO data output register */ 
#define U8_FIFO_DATA_OUT_H                    (u8)0x3f  /*!< @brief FIFO data output register */ 
#define U8_TIMESTAMP0_REG                     (u8)0x40  /*!< @brief Timestamp output register */ 
#define U8_TIMESTAMP1_REG                     (u8)0x41  /*!< @brief Timestamp output register */ 
#define U8_TIMESTAMP2_REG                     (u8)0x42  /*!< @brief Timestamp output register */ 
#define U8_STEP_TIMESTAMP_L                   (u8)0x49  /*!< @brief Step counter timestamp registers */ 
#define U8_STEP_TIMESTAMP_H                   (u8)0x4a  /*!< @brief Step counter timestamp registers */ 
#define U8_STEP_COUNTER_L                     (u8)0x4b  /*!< @brief Step counter output registers */ 
#define U8_STEP_COUNTER_H                     (u8)0x4c  /*!< @brief Step counter output registers */ 
#define U8_FUNC_SRC1                          (u8)0x53  /*!< @brief Interrupt registers */ 
#define U8_FUNC_SRC2                          (u8)0x54  /*!< @brief Interrupt registers */ 
#define U8_WRIST_TILT_IA                      (u8)0x55  /*!< @brief Interrupt register */ 
#define U8_TAP_CFG                            (u8)0x58  /*!< @brief Interrupt register */ 
#define U8_TAP_THS_6D                         (u8)0x59  /*!< @brief Interrupt register */ 
#define U8_INT_DUR2                           (u8)0x5a  /*!< @brief Interrupt register */ 
#define U8_WAKE_UP_THS                        (u8)0x5b  /*!< @brief Interrupt register */ 
#define U8_WAKE_UP_DUR                        (u8)0x5c  /*!< @brief Interrupt register */ 
#define U8_FREE_FALL                          (u8)0x5d  /*!< @brief Interrupt register */ 
#define U8_MD1_CFG                            (u8)0x5e  /*!< @brief Interrupt register */ 
#define U8_MD2_CFG                            (u8)0x5f  /*!< @brief Interrupt register */ 
#define U8_MASTER_CMD_CODE                    (u8)0x60  /*!< @brief Interrupt register */ 
#define U8_SENS_SYNC_SPI_ERROR_CODE           (u8)0x61  /*!< @brief External magnetometer raw data output registers */ 
#define U8_OUT_MAG_RAW_X_L                    (u8)0x66  /*!< @brief External magnetometer raw data output registers */ 
#define U8_OUT_MAG_RAW_X_H                    (u8)0x67  /*!< @brief External magnetometer raw data output registers */ 
#define U8_OUT_MAG_RAW_Y_L                    (u8)0x68  /*!< @brief External magnetometer raw data output registers */ 
#define U8_OUT_MAG_RAW_Y_H                    (u8)0x69  /*!< @brief External magnetometer raw data output registers */ 
#define U8_OUT_MAG_RAW_Z_L                    (u8)0x6a  /*!< @brief External magnetometer raw data output registers */ 
#define U8_OUT_MAG_RAW_Z_H                    (u8)0x6b  /*!< @brief External magnetometer raw data output registers */ 
#define U8_X_OFS_USR                          (u8)0x73  /*!< @brief Accelerometer user offset correction */ 
#define U8_Y_OFS_USR                          (u8)0x74  /*!< @brief Accelerometer user offset correction */ 
#define U8_Z_OFS_USR                          (u8)0x75  /*!< @brief Accelerometer user offset correction */ 

#endif /* __BLADE_LSM6DSL_H */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
