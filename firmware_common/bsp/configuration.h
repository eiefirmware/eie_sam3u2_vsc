/*!*********************************************************************************************************************
@file configuration.h      
@brief Main configuration header file for project.  

This file bridges many of the generic features of the firmware to the 
specific features of the design. The definitions should be updated
to match the target hardware.  
 
Bookmarks:
!!!!! External module peripheral assignments
##### UART peripheral board-specific parameters
$$$$$ SPI peripheral board-specific parameters
%%%%% SSP peripheral board-specific parameters
^^^^^ I�C peripheral board-specific parameters


***********************************************************************************************************************/

#ifndef __CONFIG_H
#define __CONFIG_H

/**********************************************************************************************************************
Runtime switches
***********************************************************************************************************************/
//#define STARTUP_SOUND               /*!< Define to include buzzer sound on startup */

//#define EIE_DOTMATRIX_R01           /*!< Use with MPGL2-EHDW-01 revision board */


/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/

/*! 
@enum PeripheralType
@brief Short names used to identify peripherals in their configuration structs.

This provides self-documentation when indexing peripherals and when assigning
better names for devices that make use of the peripherals.
*/
typedef enum {SPI0, UART, USART0, USART1, USART2, USART3} PeripheralType;


/**********************************************************************************************************************
Includes
***********************************************************************************************************************/
/* Common header files */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "AT91SAM3U4.h"
#include "exceptions.h"
#include "interrupts.h"

/* ignore some warnings specific to the CMSIS headers */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-int"
#include "core_cm3.h"
#pragma GCC diagnostic pop

#include "main.h"
#include "typedefs.h"
#include "utilities.h"

/* EIEF1-PCB-01 specific header files */
#ifdef EIE_ASCII
#include "eief1-pcb-01.h"
#include "lcd_nhd-c0220biz.h"
#endif /* EIE_ASCII */

#ifdef EIE_DOTMATRIX
/* EIE_DOTMATRIX-specific header files */
#ifdef EIE_DOTMATRIX_R01
#include "mpgl2-ehdw-01.h"
#else
#include "mpgl2-ehdw-02.h"
#endif /* EIE_DOTMATRIX_R01 */

#include "captouch.h"
#include "lcd_NHD-C12864LZ.h"
#include "lcd_bitmaps.h"
#endif /* EIE_DOTMATRIX */

/* Common driver header files */
#include "antmessage.h"
#include "antdefines.h"
#include "ant.h"
#include "ant_api.h"
#include "buttons.h"
#include "leds.h" 
#include "messaging.h"
#include "timer.h"

#include "sam3u_i2c.h"
#include "sam3u_spi.h"
#include "sam3u_ssp.h"
#include "sam3u_uart.h"
#include "adc12.h"

/* Common application header files */
#include "blade_api.h"
#include "debug.h"
#include "music.h"
#include "user_app1.h"
#include "user_app2.h"
#include "user_app3.h"


/**********************************************************************************************************************
!!!!! External device peripheral assignments
***********************************************************************************************************************/

/* Peripheral assignments */
#define BLADE_UART                  UART
#define DEBUG_UART                  USART0
#define ANT_SPI                     USART2
#define BLADE_SPI                   SPI0
#define BLADE_I2C                   TWI0

#ifdef EIE_ASCII
#define LCD_I2C                     TWI0
#define SD_SSP                      USART1
#endif

#ifdef EIE_DOTMATRIX
#define LCD_SPI                     USART1
#endif


/* Global status flags for SPI peripherals */
#define BLADE_SPI_FLAGS             G_u32Spi0ApplicationFlags  /*!< @brief Assigns the correct global Application Flags to a self-documenting symbol */
#define SD_SSP_FLAGS                G_u32Ssp1ApplicationFlags  /*!< @brief Assigns the correct global Application Flags to a self-documenting symbol */
#define ANT_SSP_FLAGS               G_u32Ssp2ApplicationFlags  /*!< @brief Assigns the correct global Application Flags to a self-documenting symbol */


/*! @cond DOXYGEN_EXCLUDE */
/* %UART% Configuration */

/* Debug UART Peripheral Allocation (USART0) */
#define USART0_US_CR_INIT           DEBUG_UART_US_CR_INIT
#define USART0_US_MR_INIT           DEBUG_UART_US_MR_INIT
#define USART0_US_IER_INIT          DEBUG_UART_US_IER_INIT
#define USART0_US_IDR_INIT          DEBUG_UART_US_IDR_INIT
#define USART0_US_BRGR_INIT         DEBUG_UART_US_BRGR_INIT

#define UART0_IRQHandler            USART0_IrqHandler
#define DEBUG_UART_PERIPHERAL       AT91C_ID_US0

/* Blade UART Peripheral Allocation (UART) */
#define UART_US_CR_INIT             BLADE_UART_US_CR_INIT
#define UART_US_MR_INIT             BLADE_UART_US_MR_INIT
#define UART_US_IER_INIT            BLADE_UART_US_IER_INIT
#define UART_US_IDR_INIT            BLADE_UART_US_IDR_INIT
#define UART_US_BRGR_INIT           BLADE_UART_US_BRGR_INIT

#define UART_IRQHandler             DBGU_IrqHandler
#define BLADE_UART_PERIPHERAL       AT91C_ID_DBGU


/* %SPI% Configuration */

/* %SPI% Blade SPI Peripheral Allocation (SPI0) */
#define BLADE_BASE_PORT             AT91C_BASE_PIOA
#define BLADE_CS_PIN                PA_16_BLADE_CS
#define BLADE_SPI_FLAGS             G_u32Spi0ApplicationFlags
#define BLADE_SPI_PERIPHERAL        AT91C_ID_SPI0

#define SPI0_CR_INIT                BLADE_SPI_CR_INIT
#define SPI0_MR_INIT                BLADE_SPI_MR_INIT
#define SPI0_IER_INIT               BLADE_SPI_IER_INIT
#define SPI0_IDR_INIT               BLADE_SPI_IDR_INIT
#define SPI0_CSR0_INIT              BLADE_SPI_CSR0_INIT
#define SPI0_CSR1_INIT              BLADE_SPI_CSR1_INIT
#define SPI0_CSR2_INIT              BLADE_SPI_CSR2_INIT
#define SPI0_CSR3_INIT              BLADE_SPI_CSR3_INIT

#define SPI0_IRQHandler             SPI0_IrqHandler


/* %SSP% Configuration */

#ifdef EIE_ASCII
/* SD SPI Peripheral Allocation (USART1) */
#define SD_BASE_PORT                AT91C_BASE_PIOA
#define SD_CS_PIN                   PA_08_SD_CS_MCDA3
#define USART1_US_CR_INIT           SD_SPI_US_CR_INIT
#define USART1_US_MR_INIT           SD_SPI_US_MR_INIT
#define USART1_US_IER_INIT          SD_SPI_US_IER_INIT
#define USART1_US_IDR_INIT          SD_SPI_US_IDR_INIT
#define USART1_US_BRGR_INIT         SD_SPI_US_BRGR_INIT

#define SSP1_IRQHandler             USART1_IrqHandler
#endif /* EIE_ASCII */

#ifdef EIE_DOTMATRIX
/* %SSP% Configuration */
/* LCD SPI Peripheral Allocation (USART1) */
#define LCD_BASE_PORT               AT91C_BASE_PIOB
#define LCD_CS_PIN                  PB_12_LCD_CS
#define USART1_US_CR_INIT           LCD_US_CR_INIT
#define USART1_US_MR_INIT           LCD_US_MR_INIT
#define USART1_US_IER_INIT          LCD_US_IER_INIT
#define USART1_US_IDR_INIT          LCD_US_IDR_INIT
#define USART1_US_BRGR_INIT         LCD_US_BRGR_INIT

#define SSP1_IRQHandler             USART1_IrqHandler
#endif /* EIE_DOTMATRIX */


/* ANT SPI Peripheral Allocation (USART2) */
#define ANT_SPI_CS_GPIO             AT91C_BASE_PIOB
#define ANT_SPI_CS_PIN              PB_22_ANT_USPI2_CS
#define USART2_US_CR_INIT           ANT_SPI_US_CR_INIT
#define USART2_US_MR_INIT           ANT_SPI_US_MR_INIT
#define USART2_US_IER_INIT          ANT_SPI_US_IER_INIT
#define USART2_US_IDR_INIT          ANT_SPI_US_IDR_INIT
#define USART2_US_BRGR_INIT         ANT_SPI_US_BRGR_INIT

#define SSP2_IRQHandler             USART2_IrqHandler


/* %I2C% Configuration */

/* EiE I2C (TWI0) */
#define TWI0_CR_INIT                EIE_TWI_CR_INIT
#define TWI0_MMR_INIT               EIE_TWI_MMR_INIT
#define TWI0_CWGR_INIT              EIE_TWI_CWGR_INIT
#define TWI0_IER_INIT               EIE_TWI_IER_INIT

#define TWI0_IRQHandler             Twi0_IrqHandler


/*! @endcond */
/***********************************************************************************************************************
##### UART peripheral board-specific parameters
***********************************************************************************************************************/
/*! @cond DOXYGEN_EXCLUDE */

/*----------------------------------------------------------------------------------------------------------------------
Debug UART Setup

Debug is used for the terminal (serial: 115.2k, 8-N-1) debugging interface.
*/
/* USART Control Register */
#define DEBUG_UART_US_CR_INIT (u32)0x00000050
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [0] RXDIS receiver not disabled
    04 [1] RXEN receiver enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register */
#define DEBUG_UART_US_MR_INIT (u32)0x000008C0
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [0] VAR_SYNC sync field 
    21 [0] DSNACK delicious! NACK is sent on ISO line immediately on parity error
    20 [0] INACK NACK generated (N/A for async)

    19 [0] OVER 16x oversampling
    18 [0] CLKO USART does not drive the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [0] MSBF/CPOL LSB first

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP 1 stop bit
    12 [0] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] SYNC/CPHA asynchronous

    07 [1] CHRL 8 bits
    06 [1] "
    05 [0] USCLKS MCK
    04 [0] "

    03 [0] USART_MODE normal
    02 [0] "
    01 [0] "
    00 [0] "
*/


/* USART Interrupt Enable Register */
#define DEBUG_UART_US_IER_INIT (u32)0x00000008
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [0] CTSIC Clear to Send Change interrupt not enabled
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt not enabled yet

    03 [1] ENDRX End of Receiver Transfer (PDC) interrupt enabled
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled
    00 [0] RXRDY Receiver Ready interrupt enabled
*/

/* USART Interrupt Disable Register */
#define DEBUG_UART_US_IDR_INIT (u32)~DEBUG_UART_US_IER_INIT

/* USART Baud Rate Generator Register 
BAUD = MCK / (8(2-OVER)(CD + FP / 8))
=> CD = (MCK / (8(2-OVER)BAUD)) - (FP / 8)
MCK = 48MHz
OVER = 0 (16-bit oversampling)

BAUD desired = 38400 bps
=> CD = 78.125 - (FP / 8)
Set FP = 1, CD = 78 = 0x4E

BAUD desired = 115200 bps
=> CD = 26.042 - (FP / 8)
Set FP = 0, CD = 26 = 0x1A

*/
#define DEBUG_UART_US_BRGR_INIT (u32)0x0000001A
/*
    31-20 [0] Reserved

    19 [0] Reserved
    18 [0] FP = 0
    17 [0] "
    16 [0] "

    15 [0] CD = 26 = 0x1A
    14 [0] "
    13 [0] "
    12 [0] "

    11 [0] "
    10 [0] "
    09 [0] "
    08 [0] "

    07 [0] "
    06 [0] "
    05 [0] "
    04 [1] "

    03 [1] "
    02 [0] "
    01 [1] "
    00 [0] "
*/


/*----------------------------------------------------------------------------------------------------------------------
Blade UART Setup

The Blade UART is used for the daughter board interface (serial: 115.2k, 8-N-1) .
*/
/* USART Control Register */
#define BLADE_UART_US_CR_INIT (u32)0x00000050
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [0] RXDIS receiver not disabled
    04 [1] RXEN receiver enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register */
#define BLADE_UART_US_MR_INIT (u32)0x000008C0
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [0] VAR_SYNC sync field 
    21 [0] DSNACK delicious! NACK is sent on ISO line immeidately on parity error
    20 [0] INACK NACK generated (N/A for async)

    19 [0] OVER 16x oversampling
    18 [0] CLKO USART does not drive the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [0] MSBF/CPOL LSB first

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP 1 stop bit
    12 [0] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] SYNC/CPHA asynchronous

    07 [1] CHRL 8 bits
    06 [1] "
    05 [0] USCLKS MCK
    04 [0] "

    03 [0] USART_MODE normal
    02 [0] "
    01 [0] "
    00 [0] "
*/


/* USART Interrupt Enable Register */
#define BLADE_UART_US_IER_INIT (u32)0x00000008
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [0] CTSIC Clear to Send Change interrupt not enabled
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt not enabled yet

    03 [1] ENDRX End of Receiver Transfer (PDC) interrupt enabled
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled
    00 [0] RXRDY Receiver Ready interrupt not enabled
*/

/* USART Interrupt Disable Register */
#define BLADE_UART_US_IDR_INIT (u32)~BLADE_UART_US_IER_INIT

/* USART Baud Rate Generator Register
BAUD = MCK / (8(2-OVER)(CD + FP / 8))
=> CD = (MCK / (8(2-OVER)BAUD)) - (FP / 8)
MCK = 48MHz
OVER = 0 (16-bit oversampling)

BAUD desired = 115200 bps
=> CD = 26.042 - (FP / 8)
Set FP = 0, CD = 26

*/
#define BLADE_UART_US_BRGR_INIT (u32)0x0000001A
/*
    31-20 [0] Reserved

    19 [0] Reserved
    18 [0] FP = 0
    17 [0] "
    16 [0] "

    15 [0] CD = 26 = 0x1A
    14 [0] "
    13 [0] "
    12 [0] "

    11 [0] "
    10 [0] "
    09 [0] "
    08 [0] "

    07 [0] "
    06 [0] "
    05 [0] "
    04 [1] "

    03 [1] "
    02 [0] "
    01 [1] "
    00 [0] "
*/


/*! @endcond */
/***********************************************************************************************************************
$$$$$ SPI peripheral board-specific parameters
***********************************************************************************************************************/
/*! @cond DOXYGEN_EXCLUDE */

/*----------------------------------------------------------------------------------------------------------------------
Blade SPI Setup 

SPI mode to communicate with a Slave device on the Blade connector. 
*/
/* SPI Control Register */
#define BLADE_SPI_CR_INIT (u32)0x00000002
/*
    31 - 28 [0] Reserved

    27 [0] Reserved
    26 [0] "
    25 [0] "
    24 [0] LASTXFER not required

    23 - 8 [0] Reserved

    07 [0] SWRST not reset
    06 [0] Reserved
    05 [0] "
    04 [0] "

    03 [0] Reserved
    02 [0] "
    01 [1] SPIDIS SPI disabled for now
    00 [0] SPIEN SPI not enabled yet
*/

/* SPI Mode Register */
#define BLADE_SPI_MR_INIT (u32)0x00000021
/*
    31 [0] DLYBCS Delay Between Chip Selects not applicable
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] "

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [0] PCS Peripheral Chip Select select CS0
    18 [0] "
    17 [0] "
    16 [0] "

    15 - 08 [0] Reserved 

    07 [0] LLB Local Loopback disabled
    06 [0] Reserved
    05 [1] WDRBT Wait Data Before Transfer enabled
    04 [0] MODFDIS Mode Fault not disabled

    03 [0] Reserved
    02 [0] PCSDEC chip select direct connect
    01 [0] PS Fixed Peripheral Select
    00 [1] MSTR Master mode
*/

/* SPI Interrupt Enable Register */
#define BLADE_SPI_IER_INIT (u32)0x00000000
/*
    31 - 12 [0] Reserved 

    11 [0] Reserved
    10 [0] UNDES not enabled
    09 [0] TXEMPTY not enabled
    08 [0] NSSR not enabled

    07 [0] Reserved
    06 [0] "
    05 [0] "
    04 [0] "

    03 [0] OVRES not enabled
    02 [0] MODF not enabled
    01 [0] TDRE not enabled yet
    00 [0] RDRF not enabled yet
*/

/* SPI Interrupt Disable Register  */
#define BLADE_SPI_IDR_INIT (u32)~BLADE_SPI_IER_INIT

/* SPI Baud Rate Generator Register
BAUD = MCK / SCBR 
=> SCBR = MCK / BAUD
BAUD desired = 1 Mbps
=> SCBR = 48

Delay before SPCK: target 10us
DLYBS = SPCK x MCK = 10us x 48MHz = 480

Delay between transfers: target 3us (3 clock ticks)
Delay = (32 x DLYBCT) / MCK
DLYBCT = (3us x 48MHz) / 32
DLYBCT = 4.5 (round up to 5)
*/
#define BLADE_SPI_CSR0_INIT (u32)0x05303001
/*
    31 [0] DLYBCT Delay between transfers = 5 (0x05)
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [1] "
    25 [0] "
    24 [1] "

    23 [0] DLYBS Delay before SCK 1us => 48 (0x30)
    22 [0] "
    21 [1] "
    20 [1] "

    19 [0] "
    18 [0] "
    17 [0] "
    16 [0] "

    15 [0] SCBR Serial clock baud rate => 48 (0x30)
    14 [0] "
    13 [1] "
    12 [1] "

    11 [0] "
    10 [0] "
    09 [0] "
    08 [0] "

    07 [0] BITS 8 bits per transfer
    06 [0] "
    05 [0] "
    04 [0] "

    03 [0] CSAAT CS rises after data tranfer
    02 [0] CSNAAT CS not active after transfer 
    01 [0] NCPHA Clock phase leading edge
    00 [1] CPOL Clock polarity high when inactive
*/

/* Only a single Slave is used, so other CSRs are the same as CSR0 */
#define BLADE_SPI_CSR1_INIT (u32)0x05303001
#define BLADE_SPI_CSR2_INIT (u32)0x05303001
#define BLADE_SPI_CSR3_INIT (u32)0x05303001

/*! @endcond */
/***********************************************************************************************************************
%%%%% SSP peripheral board-specific parameters
***********************************************************************************************************************/
/*! @cond DOXYGEN_EXCLUDE */

/*----------------------------------------------------------------------------------------------------------------------
EIE_ASCII ASCII board SD USART Setup in SSP mode

SPI mode to communicate with an SPI SD card. 
*/
/* USART Control Register */
#define SD_SPI_US_CR_INIT (u32)0x00000050
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [0] RXDIS receiver not disabled
    04 [1] RXEN receiver enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register */
#define SD_SPI_US_MR_INIT (u32)0x004518CE
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [1] VAR_SYNC sync field is updated on char to US_THR
    21 [0] DSNACK delicious! NACK is sent on ISO line immeidately on parity error
    20 [0] INACK transmission starts as oons as byte is written to US_THR

    19 [0] OVER 16x oversampling
    18 [1] CLKO USART drives the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [1] CPOL clock is high when inactive

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP N/A
    12 [1] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] CPHA data captured on leading edge of SPCK (first high to low transition does not count)

    07 [1] CHRL 8 bits
    06 [1] "
    05 [0] USCLKS MCK
    04 [0] "

    03 [1] USART_MODE SPI Master
    02 [1] "
    01 [1] "
    00 [0] "
*/


/* USART Interrupt Enable Register */
#define SD_SPI_US_IER_INIT (u32)0x00000000
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [0] CTSIC Clear to Send Change interrupt not enabled
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt not enabled for now

    03 [0] ENDRX End of Receiver Transfer (PDC) interrupt not enabled for now
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled
    00 [0] RXRDY Receiver Ready interrupt not enabled
*/

/* USART Interrupt Disable Register  */
#define SD_SPI_US_IDR_INIT (u32)~SD_SPI_US_IER_INIT

/* USART Baud Rate Generator Register
BAUD = MCK / CD 
=> CD = MCK / BAUD
BAUD desired = 1 Mbps
=> CD = 48
*/
#define SD_SPI_US_BRGR_INIT (u32)0x00000030  
/*
    31-20 [0] Reserved

    19 [0] Reserved
    18 [0] FP baud disabled
    17 [0] "
    16 [0] "

    15 [0] CD = 48 = 0x30
    14 [0] "
    13 [0] "
    12 [0] "

    11 [0] "
    10 [0] "
    09 [0] "
    08 [0] "

    07 [0] "
    06 [0] "
    05 [1] "
    04 [1] "

    03 [0] "
    02 [0] "
    01 [0] "
    00 [0] "
*/


/*----------------------------------------------------------------------------------------------------------------------
EIE_DOTMATRIX Dot Matrix LCD USART Setup in SSP mode

SPI mode to communicate with an SPI LCD screen. 
*/
/* USART Control Register - Page 734 */
#define LCD_US_CR_INIT (u32)0x00000060
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [1] RXDIS receiver disabled
    04 [0] RXEN receiver not enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register - page 737 */
#define LCD_US_MR_INIT (u32)0x004518CE
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [1] VAR_SYNC sync field is updated on char to US_THR
    21 [0] DSNACK delicious! NACK is sent on ISO line immeidately on parity error
    20 [0] INACK transmission starts as oons as byte is written to US_THR

    19 [0] OVER 16x oversampling
    18 [1] CLKO USART drives the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [1] CPOL clock is high when inactive

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP N/A
    12 [1] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] CPHA data captured on leading edge of SPCK (first high to low transition does not count)

    07 [1] CHRL 8 bits
    06 [1] "
    05 [0] USCLKS MCK
    04 [0] "

    03 [1] USART_MODE SPI Master
    02 [1] "
    01 [1] "
    00 [0] "
*/


/* USART Interrupt Enable Register - Page 741 */
#define LCD_US_IER_INIT (u32)0x00000000
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [0] CTSIC Clear to Send Change interrupt not enabled
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt not enabled for now

    03 [0] ENDRX End of Receiver Transfer (PDC) interrupt not enabled
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled
    00 [0] RXRDY Receiver Ready interrupt not enabled
*/

/* USART Interrupt Disable Register - Page 743 */
#define LCD_US_IDR_INIT (u32)~LCD_US_IER_INIT

/* USART Baud Rate Generator Register - Page 752
BAUD = MCK / CD 
=> CD = MCK / BAUD
BAUD desired = 1 Mbps
=> CD = 48
*/
#define LCD_US_BRGR_INIT (u32)0x00000030  /* VERIFY SPI CLOCK! */
/*
    31-20 [0] Reserved

    19 [0] Reserved
    18 [0] FP baud disabled
    17 [0] "
    16 [0] "

    15 [0] CD = 48 = 0x30
    14 [0] "
    13 [0] "
    12 [0] "

    11 [0] "
    10 [0] "
    09 [0] "
    08 [0] "

    07 [0] "
    06 [0] "
    05 [1] "
    04 [1] "

    03 [0] "
    02 [0] "
    01 [0] "
    00 [0] "
*/


/*----------------------------------------------------------------------------------------------------------------------
ANT USART Setup in SSP
SPI slave mode to communicate with an ANT device. 
*/
/* USART Control Register */
#define ANT_SPI_US_CR_INIT (u32)0x00000050
/*
    31 - 20 [0] Reserved

    19 [0] RTSDIS/RCS no release/force RTS to 1
    18 [0] RTSEN/FCS no drive/force RTS to 0
    17 [0] DTRDIS no drive DTR to 1
    16 [0] DTREN no drive DTR to 0

    15 [0] RETTO no restart timeout
    14 [0] RSTNACK N/A
    13 [0] RSTIT N/A
    12 [0] SENDA N/A

    11 [0] STTTO no start time-out
    10 [0] STPBRK no stop break
    09 [0] STTBRK no transmit break
    08 [0] RSTSTA status bits not reset

    07 [0] TXDIS transmitter not disabled
    06 [1] TXEN transmitter enabled
    05 [0] RXDIS receiver not disabled
    04 [1] RXEN receiver enabled

    03 [0] RSTTX not reset
    02 [0] RSTRX not reset
    01 [0] Reserved
    00 [0] "
*/

/* USART Mode Register */
#define ANT_SPI_US_MR_INIT (u32)0x004118FF
/*
    31 [0] ONEBIT start frame delimiter is COMMAND or DATA SYNC
    30 [0] MODSYNC Manchester start bit N/A
    29 [0] MAN Machester encoding disabled
    28 [0] FILTER no filter on Rx line

    27 [0] Reserved
    26 [0] MAX_ITERATION (ISO7816 mode only)
    25 [0] "
    24 [0] "

    23 [0] INVDATA data is not inverted
    22 [1] VAR_SYNC sync field is updated on char to US_THR
    21 [0] DSNACK (delicious!) NACK is sent on ISO line immeidately on parity error
    20 [0] INACK transmission starts as soon as byte is written to US_THR

    19 [0] OVER 16x oversampling
    18 [0] CLKO USART does not drive the SCK pin
    17 [0] MODE9 CHRL defines char length
    16 [1] CPOL clock is high when inactive

    15 [0] CHMODE normal mode
    14 [0] "
    13 [0] NBSTOP N/A
    12 [1] "

    11 [1] PAR no parity
    10 [0] "
    09 [0] "
    08 [0] CPHA data captured on leading edge of SPCK (first high to low transition does not count)

    07 [1] CHRL 8 bits
    06 [1] "
    05 [1] USCLKS SCK
    04 [1] "

    03 [1] USART_MODE SPI Slave
    02 [1] "
    01 [1] "
    00 [1] "
*/


/* USART Interrupt Enable Register */
#define ANT_SPI_US_IER_INIT (u32)0x00080000
/*
    31 [0] Reserved
    30 [0] "
    29 [0] "
    28 [0] "

    27 [0] "
    26 [0] "
    25 [0] "
    24 [0] MANE Manchester Error interrupt not enabled

    23 [0] Reserved
    22 [0] "
    21 [0] "
    20 [0] "

    19 [1] CTSIC Clear to Send Change interrupt enabled 
    18 [0] DCDIC Data Carrier Detect Change interrupt not enabled
    17 [0] DSRIC Data Set Ready Change interrupt not enabled
    16 [0] RIIC Ring Inidicator Change interrupt not enabled

    15 [0] Reserved
    14 [0] "
    13 [0] NACK Non Ack interrupt not enabled
    12 [0] RXBUFF Reception Buffer Full (PDC) interrupt not enabled 

    11 [0] TXBUFE Transmission Buffer Empty (PDC) interrupt not enabled
    10 [0] ITER/UNRE Max number of Repetitions Reached interrupt not enabled
    09 [0] TXEMPTY Transmitter Empty interrupt not enabled (yet)
    08 [0] TIMEOUT Receiver Time-out interrupt not enabled

    07 [0] PARE Parity Error interrupt not enabled
    06 [0] FRAME Framing Error interrupt not enabled
    05 [0] OVRE Overrun Error interrupt not enabled
    04 [0] ENDTX End of Transmitter Transfer (PDC) interrupt not enabled 

    03 [0] ENDRX End of Receiver Transfer (PDC) interrupt not enabled
    02 [0] RXBRK Break Received interrupt not enabled
    01 [0] TXRDY Transmitter Ready interrupt not enabled YET
    00 [0] RXRDY Receiver Ready interrupt enabled 
*/

/* USART Interrupt Disable Register */
#define ANT_SPI_US_IDR_INIT (u32)~ANT_SPI_US_IER_INIT

/* USART Baud Rate Generator Register 
!!!!! Not applicable for Slave (note that incoming clock cannot 
exceed MCLK/6 = 8MHz.  To date, ANT devices communicate at 500kHz
or 2MHz, so no issues.
*/
#define ANT_SPI_US_BRGR_INIT (u32)0x00000000  


/*! @endcond */
/***********************************************************************************************************************
^^^^^ I�C (TWI) peripheral board-specific parameters
***********************************************************************************************************************/
/*! @cond DOXYGEN_EXCLUDE */

/*----------------------------------------------------------------------------------------------------------------------
I�C Master mode for EiE development board (TWI0)
ASCII: LCD and Blade
Dot Matrix: Blade and R01 EIE_DOTMATRIX accelerometer
*/

/* Control Register */
#define EIE_TWI_CR_INIT (u32)0x00000024
/*
    31-8 [0] Reserved

    07 [0] SWRST - Software reset
    06 [0] QUICK - SMBUS Quick Command
    05 [1] SVDIS - Slave mode disabled
    04 [0] SVEN - Slave mode enable

    03 [0] MSDIS - Master mode disable
    02 [1] MSEN - Master mode enable
    01 [0] STOP - Stop a transfer
    00 [0] START - Start a transfer
*/

/* Master Mode Register */
#define EIE_TWI_MMR_INIT (u32)0x00000000
/*
    31-24 [0] Reserved
    
    23 [0] Reserved
    22 [0] DADR - device Slave address - start with zero
    21 [0] "
    20 [0] "

    19 [0] "
    18 [0] "
    17 [0] "
    16 [0] "

    15 [0] Reserved
    14 [0] "
    13 [0] "
    12 [0] MREAD - Master Read Direction - 0 -> Write, 1 -> Read

    11 [0] Reserved
    10 [0] "
    09 [0] IADRSZ - Internal device address - 0 = no internal device address
    08 [0] "

    07-00 [0] Reserved
*/

/* Clock Wave Generator Register */
/* 
    Calculation:
        T_low = ((CLDIV * (2^CKDIV))+4) * T_MCK
        T_high = ((CHDIV * (2^CKDIV))+4) * T_MCK

        T_MCK - period of master clock = 1/(48 MHz)
        T_low/T_high - period of the low and high signals
        
        CKDIV = 2, CHDIV and CLDIV = 59
        T_low/T_high = 2.5 microseconds

        Data frequency - 
        f = ((T_low + T_high)^-1)
        f = 200000 Hz 0r 200 kHz

    Additional Rates:
        50 kHz - 0x00027777
       100 kHz - 0x00023B3B
       200 kHz - 0x00021D1D
       400 kHz - 0x00030707  *Maximum rate*
*/
#define EIE_TWI_CWGR_INIT (u32)0x00021D1D
/*
    31-20 [0] Reserved
    
    19 [0] Reserved
    18 [0] CKDIV
    17 [1] "
    16 [0] "

    15 [0] CHDIV
    14 [0] "
    13 [0] "
    12 [1] "

    11 [1] "
    10 [1] "
    09 [0] "
    08 [1] "

    07 [0] CLDIV - Clock Low Divider
    06 [0] "
    05 [0] "
    04 [1] "

    03 [1] "
    02 [1] "
    01 [0] "
    00 [1] "
*/

/*Interrupt Enable Register*/
#define EIE_TWI_IER_INIT (u32)0x00000100
/*
    31-16 [0] Reserved

    15 [0] TXBUFE - Transmit Buffer Empty
    14 [0] RXBUFF - Receive Buffer Full
    13 [0] ENDTX - End of Transmit Buffer not enabled yet
    12 [0] ENDRX - End of Receive Buffer not enabled yet

    11 [0] EOSACC - End of Slave Address
    10 [0] SCL_WS - Clock Wait State
    09 [0] ARBLST - Arbitration Lost
    08 [1] NACK - Not Acknowledge

    07 [0] Reserved
    06 [0] OVRE - Overrun Error
    05 [0] GACC - General Call Access
    04 [0] SVACC - Slave Access

    03 [0] Reserved
    02 [0] TXRDY - Transmit Holding Register Ready
    01 [0] RXRDY - Receive Holding Register Ready
    00 [0] TXCOMP - Transmission Completed
*/




#endif /* __CONFIG_H */

/*! @endcond */
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/

