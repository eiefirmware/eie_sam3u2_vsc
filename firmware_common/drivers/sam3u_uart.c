/*!**********************************************************************************************************************
@file sam3u_uart.c                                                                
@brief Provides a driver to use UART peripherals on the SAM3U processor to send and receive data using interrupts. 

This driver covers both the dedicated UART peripheral and the three USART peripherals (assuming they are
running in asynchronous (UART) mode).

INITIALIZATION (should take place in application's initialization function):
1. Create a variable of UartConfigurationType in your application and initialize it to the desired UART peripheral,
the address of the receive buffer for the application, and the size in bytes of the receive buffer.

2. Call UartRequest() with pointer to the configuration variable created in step 1.  The returned pointer is the
UartPeripheralType object created that will be used by your application and should be assigned to a variable
accessible to your application.

3. If the application no longer needs the UART resource, call UartRelease().  

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- UartConfigurationType
- UartPeripheralType

PUBLIC FUNCTIONS
- UartPeripheralType* UartRequest(UartConfigurationType* psUartConfig_)
- void UartRelease(UartPeripheralType* psUartPeripheral_)
- u32 UartWriteByte(UartPeripheralType* psUartPeripheral_, u8 u8Byte_)
- u32 UartWriteData(UartPeripheralType* psUartPeripheral_, u32 u32Size_, u8* pu8Data_)

PROTECTED FUNCTIONS
- void UartInitialize(void);
- void UartRunActiveState(void);
- static void UartManualMode(void);


**********************************************************************************************************************/

#include "configuration.h"


/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Uart"
***********************************************************************************************************************/
/* New variables */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;          /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;           /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;            /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;       /*!< @brief From main.c */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Uart_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Uart_pfnStateMachine;         /*!< @brief The UART application state machine function pointer */

static u32 Uart_u32Flags;                        /*!< @brief Application flags for UART */
static u8  Uart_u8ActiveUarts;                   /*!< @brief Counting semaphore for # of active UARTs */

static UartPeripheralType Uart_sPeripheral;      /*!< @brief UART peripheral object */
static UartPeripheralType Uart_sPeripheral0;     /*!< @brief USART0 peripheral object (used as UART) */
static UartPeripheralType Uart_sPeripheral1;     /*!< @brief USART1 peripheral object (used as UART) */
static UartPeripheralType Uart_sPeripheral2;     /*!< @brief USART2 peripheral object (used as UART) */

static UartPeripheralType* Uart_psCurrentUart;   /*!< @brief Current UART peripheral being processed */
static UartPeripheralType* Uart_psCurrentISR;    /*!< @brief Current UART peripheral being processed in ISR */

static u32 Uart_u32IntCount  = 0;                /*!< @brief Debug counter for UART interrupts */
static u32 Uart_u32Int0Count = 0;                /*!< @brief Debug counter for USART0 interrupts */
static u32 Uart_u32Int1Count = 0;                /*!< @brief Debug counter for USART1 interrupts */
static u32 Uart_u32Int2Count = 0;                /*!< @brief Debug counter for USART2 interrupts */


/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*!---------------------------------------------------------------------------------------------------------------------
@fn UartPeripheralType* UartRequest(UartConfigurationType* psUartConfig_)

@brief Requests access to a UART resource.  

If the resource is available, the transmit and receive parameters are set up
and the peripheral is made ready to use in the application.  

Requires:
- Uart_sPeripheralx perihperal objects have been initialized
- USART Peripheralx registers are not write-protected (WPEN)
- UART peripheral register initialization values in configuration.h must be set correctly
- UART/USART peripheral registers configured here are at the same address offset regardless of the peripheral. 

@param psUartConfig_ has the UART peripheral number, address of the RxBuffer, and the RxBuffer size and the calling
       application is ready to start using the peripheral.

Promises:
- Returns NULL if a resource cannot be assigned; OR
- Returns a pointer to the requested UART peripheral object if the resource is available
- Peripheral is configured and enabled 
- Peripheral interrupts are enabled.

*/
UartPeripheralType* UartRequest(UartConfigurationType* psUartConfig_)
{
  UartPeripheralType* psRequestedUart;
  u32 u32TargetCR;
  u32 u32TargetMR;
  u32 u32TargetIER;
  u32 u32TargetIDR;
  u32 u32TargetBRGR;
  
  /* Set-up is peripheral-specific */
  switch(psUartConfig_->UartPeripheral)
  {
    case UART:
    {
      psRequestedUart = &Uart_sPeripheral; 

      u32TargetCR   = UART_US_CR_INIT;
      u32TargetMR   = UART_US_MR_INIT; 
      u32TargetIER  = UART_US_IER_INIT; 
      u32TargetIDR  = UART_US_IDR_INIT;
      u32TargetBRGR = UART_US_BRGR_INIT;
      break;
    } 

    case USART0:
    {
      psRequestedUart = &Uart_sPeripheral0; 

      u32TargetCR   = USART0_US_CR_INIT;
      u32TargetMR   = USART0_US_MR_INIT; 
      u32TargetIER  = USART0_US_IER_INIT; 
      u32TargetIDR  = USART0_US_IDR_INIT;
      u32TargetBRGR = USART0_US_BRGR_INIT;
      break;
    } 

#if 0
    case USART1:
    {
      psRequestedUart = &Uart_sPeripheral1; 

      u32TargetCR   = USART1_US_CR_INIT;
      u32TargetMR   = USART1_US_MR_INIT; 
      u32TargetIER  = USART1_US_IER_INIT; 
      u32TargetIDR  = USART1_US_IDR_INIT;
      u32TargetBRGR = USART1_US_BRGR_INIT;
      break;
    } 
    
    case USART2:
    {
      psRequestedUart = &Uart_sPeripheral2; 
      
      u32TargetCR   = USART2_US_CR_INIT;
      u32TargetMR   = USART2_US_MR_INIT; 
      u32TargetIER  = USART2_US_IER_INIT; 
      u32TargetIDR  = USART2_US_IDR_INIT;
      u32TargetBRGR = USART2_US_BRGR_INIT;
      break;
    } 
#endif
    default:
    {
      return(NULL);
      break;
    } 
  } /* end switch */

  /* If the requested peripheral is already assigned, return NULL now */
  if(psRequestedUart->u32PrivateFlags & _UART_PERIPHERAL_ASSIGNED)
  {
    return(NULL);
  }
  
  /* Activate and configure the peripheral */
  AT91C_BASE_PMC->PMC_PCER |= (1 << psRequestedUart->u8PeripheralId);

  psRequestedUart->pu8RxBuffer     = psUartConfig_->pu8RxBufferAddress;
  psRequestedUart->u16RxBufferSize = psUartConfig_->u16RxBufferSize;
  psRequestedUart->pu8RxNextByte   = psUartConfig_->pu8RxNextByte;
  psRequestedUart->fnRxCallback    = psUartConfig_->fnRxCallback;
  psRequestedUart->u32PrivateFlags |= _UART_PERIPHERAL_ASSIGNED;
  
  psRequestedUart->pBaseAddress->US_CR   = u32TargetCR;
  psRequestedUart->pBaseAddress->US_MR   = u32TargetMR;
  psRequestedUart->pBaseAddress->US_IER  = u32TargetIER;
  psRequestedUart->pBaseAddress->US_IDR  = u32TargetIDR;
  psRequestedUart->pBaseAddress->US_BRGR = u32TargetBRGR;

  /* Preset the receive PDC pointers and counters; the receive buffer must be starting from [0] and be at least 2 bytes long)*/
  psRequestedUart->pBaseAddress->US_RPR  = (unsigned int)psUartConfig_->pu8RxBufferAddress;
  psRequestedUart->pBaseAddress->US_RNPR = (unsigned int)((psUartConfig_->pu8RxBufferAddress) + 1);
  psRequestedUart->pBaseAddress->US_RCR  = 1;
  psRequestedUart->pBaseAddress->US_RNCR = 1;
  
  /* Enable the receiver and transmitter requests */
  psRequestedUart->pBaseAddress->US_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;

  /* Enable UART interrupts */
  NVIC_ClearPendingIRQ( (IRQn_Type)psRequestedUart->u8PeripheralId );
  NVIC_EnableIRQ( (IRQn_Type)psRequestedUart->u8PeripheralId );
  
  return(psRequestedUart);
  
} /* end UartRequest() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn void UartRelease(UartPeripheralType* psUartPeripheral_)

@brief Releases a UART resource.  

Requires:
@param psUartPeripheral_ has the UART peripheral number to be released

Promises:
- Disables the associated interrupts
- Resets peripheral object's pointers
- Any unsent messages are dumped and set to ABANDONED status
- Main SM reset to Idle

*/
void UartRelease(UartPeripheralType* psUartPeripheral_)
{
  /* Check to see if the peripheral is already released */
  if(psUartPeripheral_->pu8RxBuffer == NULL)
  {
    return;
  }
  
  /* First disable the interrupts */
  NVIC_DisableIRQ( (IRQn_Type)(psUartPeripheral_->u8PeripheralId) );
  NVIC_ClearPendingIRQ( (IRQn_Type)(psUartPeripheral_->u8PeripheralId) );
 
  /* Now it's safe to release all of the resources in the target peripheral */
  psUartPeripheral_->pu8RxBuffer   = NULL;
  psUartPeripheral_->pu8RxNextByte = NULL;
  psUartPeripheral_->fnRxCallback  = NULL;
  psUartPeripheral_->u32PrivateFlags = 0;

  /* Empty the transmit buffer if there were leftover messages */
  while(psUartPeripheral_->psTransmitBuffer != NULL)
  {
    UpdateMessageStatus(psUartPeripheral_->psTransmitBuffer->u32Token, ABANDONED);
    DeQueueMessage(&psUartPeripheral_->psTransmitBuffer);
  }
  
  /* Ensure the SM is in the Idle state */
  Uart_pfnStateMachine = UartSM_Idle;
 
} /* end UartRelease() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn u32 UartWriteByte(UartPeripheralType* psUartPeripheral_, u8 u8Byte_)

@brief Queues a single byte for transfer on the target UART peripheral.  

Requires:
@param psUartPeripheral_ has been requested.
@param u8Byte_ is the byte to send

Promises:
- Creates a 1-byte message at psUartPeripheral_->pTransmitBuffer that will be sent by the UART application
  when it is available.
- Returns the message token assigned to the message

*/
u32 UartWriteByte(UartPeripheralType* psUartPeripheral_, u8 u8Byte_)
{
  u32 u32Token;
  u8 u8Data = u8Byte_;
  
  /* Attempt to queue message and get a response token */
  u32Token = QueueMessage(&psUartPeripheral_->psTransmitBuffer, 1, &u8Data);
  
  if( u32Token != 0 )
  {
    /* If the system is initializing, we want to manually cycle the UART task through one iteration
    to send the message */
    if(G_u32SystemFlags & _SYSTEM_INITIALIZING)
    {
      UartManualMode();
    }
  }
  
  return(u32Token);
  
} /* end UartWriteByte() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn u32 UartWriteData(UartPeripheralType* psUartPeripheral_, u32 u32Size_, u8* pu8Data_)

@brief Queues an array of bytes for transfer on the target UART peripheral.  

Requires:
@param psUartPeripheral_ has been requested and holds a valid pointer to a transmit buffer; even if a transmission is
       in progress, the node in the buffer that is currently being sent will not be destroyed during this function.
@param u32Size_ is the number of bytes in the data array; should not be 0
@param pu8Data_ points to the first byte of the data array

Promises:
- adds the data message at psUartPeripheral_->pTransmitBuffer that will be sent by the UART application
  when it is available.
- Returns the message token assigned to the message; 0 is returned if the message cannot be queued in which case
  G_u32MessagingFlags can be checked for the reason

*/
u32 UartWriteData(UartPeripheralType* psUartPeripheral_, u32 u32Size_, u8* pu8Data_)
{
  u32 u32Token;
  
  /* Check for a valid size */
  if(u32Size_ == 0)
  {
    return 0;
  }

  /* Attempt to queue message and get a response token */
  u32Token = QueueMessage(&psUartPeripheral_->psTransmitBuffer, u32Size_, pu8Data_);
  if(u32Token)
  {
    /* If the system is initializing, manually cycle the UART task through one iteration to send the message */
    if(G_u32SystemFlags & _SYSTEM_INITIALIZING)
    {
      UartManualMode();
    }
  }
  
  return(u32Token);
  
} /* end UartWriteData() */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UartInitialize(void)

@brief Initializes the UART application and its variables.  

Requires:
- NONE 

Promises:
- UART peripheral objects are ready 
- UART application set to Idle

*/
void UartInitialize(void)
{
  /* Initialize all the UART peripheral structures */
  Uart_sPeripheral.pBaseAddress      = (AT91S_USART*)AT91C_BASE_DBGU;
  Uart_sPeripheral.psTransmitBuffer  = NULL;
  Uart_sPeripheral.pu8RxBuffer       = NULL;
  Uart_sPeripheral.u16RxBufferSize   = 0;
  Uart_sPeripheral.pu8RxNextByte     = NULL;
  Uart_sPeripheral.u32PrivateFlags   = 0;
  Uart_sPeripheral.u8PeripheralId    = AT91C_ID_DBGU;

  Uart_sPeripheral0.pBaseAddress     = AT91C_BASE_US0;
  Uart_sPeripheral0.psTransmitBuffer = NULL;
  Uart_sPeripheral0.pu8RxBuffer      = NULL;
  Uart_sPeripheral0.u16RxBufferSize  = 0;
  Uart_sPeripheral0.pu8RxNextByte    = NULL;
  Uart_sPeripheral0.u32PrivateFlags  = 0;
  Uart_sPeripheral0.u8PeripheralId   = AT91C_ID_US0;

  Uart_sPeripheral1.pBaseAddress     = AT91C_BASE_US1;
  Uart_sPeripheral1.psTransmitBuffer = NULL;
  Uart_sPeripheral1.pu8RxBuffer      = NULL;
  Uart_sPeripheral1.u16RxBufferSize  = 0;
  Uart_sPeripheral1.pu8RxNextByte    = NULL;
  Uart_sPeripheral1.u32PrivateFlags  = 0;
  Uart_sPeripheral1.u8PeripheralId   = AT91C_ID_US1;

  Uart_sPeripheral2.pBaseAddress     = AT91C_BASE_US2;
  Uart_sPeripheral2.psTransmitBuffer = NULL;
  Uart_sPeripheral2.pu8RxBuffer      = NULL;
  Uart_sPeripheral2.u16RxBufferSize  = 0;
  Uart_sPeripheral2.pu8RxNextByte    = NULL;
  Uart_sPeripheral2.u32PrivateFlags  = 0;
  Uart_sPeripheral2.u8PeripheralId   = AT91C_ID_US2;
  
  /* Select the first UART peripheral and initialize other globals */
  Uart_psCurrentUart = &Uart_sPeripheral;
  Uart_u32Flags = 0;
  Uart_u8ActiveUarts = 0;
  Uart_pfnStateMachine = UartSM_Idle;
  
} /* end UartInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void UartRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UartRunActiveState(void)
{
  Uart_pfnStateMachine();

} /* end UartRunActiveState */


/*!----------------------------------------------------------------------------------------------------------------------
@fn static void UartManualMode(void)

@brief Runs a transmit cycle of the UART application to clock out a message.  This function is used only during
initialization.

Requires:
- G_u32SystemFlags _SYSTEM_INITIALIZING is set
- UART application has been initialized.

Promises:
- Runs the UART task until no UART messages are queued for transmission.

*/
static void UartManualMode(void)
{
  u32 u32Timer;
  
  Uart_u32Flags |=_UART_MANUAL_MODE;
  Uart_psCurrentUart = &Uart_sPeripheral;
  
  while(Uart_u32Flags &_UART_MANUAL_MODE)
  {
    WATCHDOG_BONE();
    UartRunActiveState();
    MessagingRunActiveState();

    u32Timer = G_u32SystemTime1ms;
    while( !IsTimeUp(&u32Timer, 1) );
  }
      
} /* end UartManualMode() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn ISR void UART_IRQHandler(void)

@brief Handles the enabled UART interrupts for the basic UART. 

Requires:
- NONE

Promises:
- Gets the current interrupt context and proceeds to the Generic handler

*/
void UART_IRQHandler(void)
{
  /* Set the current ISR pointers to UART targets */
  Uart_psCurrentISR = &Uart_sPeripheral;                       
  Uart_u32IntCount++;

  /* Go to common UART interrupt using Uart_psCurrentISR since the SSP cannot interrupt itself */
  UartGenericHandler();

} /* end Uart_IRQHandler() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn ISR void UART0_IRQHandler(void)

@brief Handles the enabled UART interrupts for UART0. 

Requires:
- NONE

Promises:
- Gets the current interrupt context and proceeds to the Generic handler

*/
void UART0_IRQHandler(void)
{
  /* Set the current ISR pointers to UART0 targets */
  Uart_psCurrentISR = &Uart_sPeripheral0;                         
  Uart_u32Int0Count++;

  /* Go to common interrupt */
  UartGenericHandler();

} /* end UART0_IRQHandler() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn ISR void UART1_IRQHandler(void)

@brief Handles the enabled UART interrupts for UART1. 

Requires:
- NONE

Promises:
- Gets the current interrupt context and proceeds to the Generic handler

*/
void UART1_IRQHandler(void)
{
  /* Set the current ISR pointers to UART1 targets */
  Uart_psCurrentISR = &Uart_sPeripheral1;                          
  Uart_u32Int1Count++;

  /* Go to common interrupt */
  UartGenericHandler();

} /* end UART1_IRQHandler() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn ISR void UART2_IRQHandler(void)

@brief Handles the enabled interrupts for UART2. 

Requires:
- NONE

Promises:
- Gets the current interrupt context and proceeds to the Generic handler

*/
void UART2_IRQHandler(void)
{
  /* Set the current ISR pointers to UART2 targets */
  Uart_psCurrentISR = &Uart_sPeripheral2;                          
  Uart_u32Int2Count++;

  /* Go to common interrupt */
  UartGenericHandler();

} /* end UART2_IRQHandler() */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------------------------------------------------
@fn static void UartGenericHandler(void)

@brief Common handler for all expected UART interrupts regardless of base peripheral

Receive: A requested UART peripheral is always enabled and ready to receive data.  Receive interrupts will occur when a
new byte has been read by the peripheral. All incoming data is dumped into the circular receive data buffer configured.
No processing is done on the data - it is up to the processing application to parse incoming data to find useful information
and to manage dummy bytes.  All data reception is done with DMA, but only 1 byte at a time.  Receiving is done by using
the two reception pointers to ensure no data is missed.

Transmit: All data bytes in the transmit buffer are sent using DMA and interrupts. Once the full message has been sent,
the message status is updated.

*/
static void UartGenericHandler(void)
{
  /* ENDRX Interrupt when a byte has been received (RNCR is moved to RCR; RNPR is copied to RPR) */
  if( (Uart_psCurrentISR->pBaseAddress->US_IMR & AT91C_US_ENDRX) && 
      (Uart_psCurrentISR->pBaseAddress->US_CSR & AT91C_US_ENDRX) )
  {
    /* Update the "next" DMA pointer to the next valid Rx location with wrap-around check */
    Uart_psCurrentISR->pBaseAddress->US_RNPR++;
    if(Uart_psCurrentISR->pBaseAddress->US_RNPR == (u32)(Uart_psCurrentISR->pu8RxBuffer + ( (u32)(Uart_psCurrentISR->u16RxBufferSize) & 0x0000FFFF ) ) )
    {
      Uart_psCurrentISR->pBaseAddress->US_RNPR = (u32)Uart_psCurrentISR->pu8RxBuffer;  
    }

    /* Invoke the callback */
    Uart_psCurrentISR->fnRxCallback();
    
    /* Write RNCR to 1 to clear the ENDRX flag */
    Uart_psCurrentISR->pBaseAddress->US_RNCR = 1;
    
  } /* end of ENDRX interrupt processing */

  
  /* ENDTX Interrupt when all requested transmit bytes have been sent (if enabled) */
  if( (Uart_psCurrentISR->pBaseAddress->US_IMR & AT91C_US_ENDTX) && 
      (Uart_psCurrentISR->pBaseAddress->US_CSR & AT91C_US_ENDTX) )
  {
    /* Update this message's token status and then DeQueue it */
    UpdateMessageStatus(Uart_psCurrentISR->psTransmitBuffer->u32Token, COMPLETE);
    DeQueueMessage( &Uart_psCurrentISR->psTransmitBuffer );
    Uart_psCurrentISR->u32PrivateFlags &= ~_UART_PERIPHERAL_TX;
        
    /* Disable the transmitter and interrupt sources that were enabled in UART Idle to 
    start the transmission sequence */
    Uart_psCurrentISR->pBaseAddress->US_PTCR = AT91C_PDC_TXTDIS;
    Uart_psCurrentISR->pBaseAddress->US_IDR  = AT91C_US_ENDTX;
    
    /* Decrement # of UARTs that are currently sending (incremented in UART Idle when the
    transmission started) */
    if(Uart_u8ActiveUarts != 0)
    {
      Uart_u8ActiveUarts--;
    }
    else
    {
      /* If Uart_u8ActiveUarts is already 0, then we are not properly synchronized */
      DebugPrintf("\n\rUART counter out of sync\n\r");
      Uart_u32Flags |= _UART_NO_ACTIVE_UARTS;
    }
    
  } /* end of ENDTX interrupt processing */
  
} /* end UartGenericHandler() */


/***********************************************************************************************************************
State Machine Function Definitions

The UART state machine monitors messaging activity on the available UART peripherals.  It manages outgoing messages and will
transmit any bytes that has been queued.  

Transmitting:
When TxBufferUnsentChar doesn't match Uart_pu8U0TxBufferNextChar, then we know that there is data to send.
Data transfer is initiated by writing the first byte and setting the _Uart_U0_SENDING flag to keep the USART state machine 
busy sending all of the current data on the USART.  The interrupt service routine will be responsible for clearing the
bit which will allow the SM to return to Idle.  

Receiving on USART 0:
Since the UART can only talk to one device, we will hard-code some of the functionality.  Reception of bytes will
simply dump into the UartRxBuffer and the task interested in those bytes can read the data.  In this case, we
know that this is only the Debug / User interface task.  Though other tasks could also access the buffer, we
assume they won't.

***********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UartSM_Idle(void)

@brief Wait for a transmit message to be queued.  Received data is handled in interrupts. 
*/
static void UartSM_Idle(void)
{
  /* Check all UART peripherals for message activity or skip the current peripheral if it is already busy sending.
  All receive functions take place outside of the state machine.
  Devices sending a message will have Uart_psCurrentSsp->psTransmitBuffer->pu8Message pointing to the message to send. */
  if( (Uart_psCurrentUart->psTransmitBuffer != NULL) && 
     !(Uart_psCurrentUart->u32PrivateFlags & _UART_PERIPHERAL_TX ) )
  {
    /* Transmitting: update the message's status and flag that the peripheral is now busy */
    UpdateMessageStatus(Uart_psCurrentUart->psTransmitBuffer->u32Token, SENDING);
    Uart_psCurrentUart->u32PrivateFlags |= _UART_PERIPHERAL_TX;    
      
    /* Load the PDC counter and pointer registers */
    Uart_psCurrentUart->pBaseAddress->US_TPR = (unsigned int)Uart_psCurrentUart->psTransmitBuffer->pu8Message;
    Uart_psCurrentUart->pBaseAddress->US_TCR = Uart_psCurrentUart->psTransmitBuffer->u32Size;

    /* When TCR is loaded, the ENDTX flag is cleared so it is safe to enable the interrupt */
    Uart_psCurrentUart->pBaseAddress->US_IER = AT91C_US_ENDTX;
    
    /* Update active UART count and enable the transmitter to start the transfer */
    Uart_u8ActiveUarts++;
    if(Uart_u8ActiveUarts > U8_MAX_NUM_UARTS)
    {
      /* Alert that the number of actual UARTs has been exceeded */
      Uart_u32Flags |= _UART_TOO_MANY_UARTS;
    }
    Uart_psCurrentUart->pBaseAddress->US_PTCR = AT91C_PDC_TXTEN;
  }
  
  /* Adjust to check the next peripheral next time through */
  switch (Uart_psCurrentUart->u8PeripheralId)
  {
    case AT91C_ID_DBGU:
    {
      Uart_psCurrentUart = &Uart_sPeripheral0;
      break;
    }
    
    case AT91C_ID_US0:
    {
      Uart_psCurrentUart = &Uart_sPeripheral1;
      break;
    }
    
    case AT91C_ID_US1:
    {  Uart_psCurrentUart = &Uart_sPeripheral2;
      break;
    }
    
    case AT91C_ID_US2:
    {
      Uart_psCurrentUart = &Uart_sPeripheral;
      
      /* Only clear _UART_MANUAL_MODE if all UARTs are done sending to ensure messages are sent during initialization */
      if( (G_u32SystemFlags & _SYSTEM_INITIALIZING) && !Uart_u8ActiveUarts)
      {
        Uart_u32Flags &= ~_UART_MANUAL_MODE;
      }
      break;
    }
    
    default:
    {
      Uart_psCurrentUart = &Uart_sPeripheral;
      break;
    }
  } /* end switch */
  
} /* end UartSM_Idle() */


#if 0 /* Not currently used */
/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UartSM_Error(void)

@brief Handle an error
*/
static void UartSM_Error(void)          
{
  
} /* end UartSM_Error() */
#endif         
          
          
          
        
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/

