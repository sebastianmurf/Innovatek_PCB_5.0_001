/******************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH (FSEU)                */
/*                                                                           */
/* The following software deliverable is intended for and must only be       */
/* used for reference and in an evaluation laboratory environment.           */
/* It is provided on an as-is basis without charge and is subject to         */
/* alterations.                                                              */
/* It is the user’s obligation to fully test the software in its             */
/* environment and to ensure proper functionality, qualification and         */
/* compliance with component specifications.                                 */
/*                                                                           */
/* In the event the software deliverable includes the use of open            */
/* source components, the provisions of the governing open source            */
/* license agreement shall apply with respect to such software               */
/* deliverable.                                                              */
/* FSEU does not warrant that the deliverables do not infringe any           */
/* third party intellectual property right (IPR). In the event that          */
/* the deliverables infringe a third party IPR it is the sole                */
/* responsibility of the customer to obtain necessary licenses to            */
/* continue the usage of the deliverable.                                    */
/*                                                                           */
/* To the maximum extent permitted by applicable law FSEU disclaims all      */
/* warranties, whether express or implied, in particular, but not            */
/* limited to, warranties of merchantability and fitness for a               */
/* particular purpose for which the deliverable is not designated.           */
/*                                                                           */
/* To the maximum extent permitted by applicable law, FSEU’s liability       */
/* is restricted to intentional misconduct and gross negligence.             */
/* FSEU is not liable for consequential damages.                             */
/*                                                                           */
/* (V1.5)                                                                    */
/******************************************************************************/
/** \file mfs.c
 **
 ** A detailed description is available at 
 ** @link MfsGroup MFS Module description @endlink
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-05-11  1.1  MWi  I2C noise filter added, I2C multiple byte read
 **                           bug fix, I2C timeout handling improved
 **   - 2012-07-09  1.2  MWi  No LIN support for Device Type 3, 6, 7 added
 **   - 2012-08-10  1.3  MWi  Bug fix in Mfs_Read()
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "mfs.h"
#if (defined(L3_PERIPHERAL_MFS_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup MfsGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/// Macro to return the number of enabled MFS instances
#define MFS_INSTANCE_COUNT (uint32_t)(sizeof(m_astcMfsInstanceDataLut) / sizeof(m_astcMfsInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled MFS instances and their internal data
stc_mfs_instance_data_t m_astcMfsInstanceDataLut[] =
{
    #if (L3_PERIPHERAL_ENABLE_MFS0 == L3_ON)
    { &MFS0,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS1 == L3_ON)
    { &MFS1,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS2 == L3_ON)
    { &MFS2,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS3 == L3_ON)
    { &MFS3,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS4 == L3_ON)
    { &MFS4,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS5 == L3_ON)
    { &MFS5,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS6 == L3_ON)
    { &MFS6,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS7 == L3_ON)
    { &MFS7,  // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
};

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/

/******************************************************************************/
/* Local variable definitions ('static')                                      */
/******************************************************************************/

/** Holds the pointer to a single USART channel used by AsynInit. Multiple
    channels are not supported.
 */
static stc_mfsn_t* pstcMfsComChannel = NULL ;

/// Static buffer, which is used by Mfs_AsyncInit() only!
static char cRxBuffer[MFS_BUFFER_LENGTH] ;
static char cTxBuffer[MFS_BUFFER_LENGTH] ;

/******************************************************************************/
/* Function implementation - global ('extern') and local ('static')           */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Return the internal data for a certain MFS instance.
 **
 ** \param pstcMfs Pointer to MFS instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled (or not known)
 **
 ******************************************************************************/
static stc_mfs_intern_data_t* MfsGetInternDataPtr(const stc_mfsn_t* pstcMfs) 
{
    uint32_t u32Instance;
   
    for (u32Instance = 0; u32Instance < MFS_INSTANCE_COUNT; u32Instance++)
    {
        if (pstcMfs == m_astcMfsInstanceDataLut[u32Instance].pstcInstance)
        {
            return &m_astcMfsInstanceDataLut[u32Instance].stcInternData;
        }
    }

    return NULL;
}


/**
 ******************************************************************************
 ** \brief Device dependent initialization of interrupts according CMSIS with
 **        level defined in l3.h
 **
 ** \param pstcMfs Pointer to MFS instance
 **
 ** \return Ok     Successful initialization
 **
 ******************************************************************************/
static en_result_t Mfs_InitIrq( stc_mfsn_t* pstcMfs ) 
{
  #if (L3_PERIPHERAL_ENABLE_MFS0 == L3_ON)
  if (pstcMfs == &MFS0)
  {
    // MFS0 interrupt settings
    NVIC_ClearPendingIRQ(MFS0TX_IRQn);
    NVIC_EnableIRQ(MFS0TX_IRQn);
    NVIC_SetPriority(MFS0TX_IRQn, L3_IRQ_LEVEL_MFS0_TX);
   
    NVIC_ClearPendingIRQ(MFS0RX_IRQn);
    NVIC_EnableIRQ(MFS0RX_IRQn);
    NVIC_SetPriority(MFS0RX_IRQn, L3_IRQ_LEVEL_MFS0_RX);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS1 == L3_ON) 
  if (pstcMfs == &MFS1)
  {
    // MFS1 interrupt settings
    NVIC_ClearPendingIRQ(MFS1TX_IRQn);
    NVIC_EnableIRQ(MFS1TX_IRQn);
    NVIC_SetPriority(MFS1TX_IRQn, L3_IRQ_LEVEL_MFS1_TX);
   
    NVIC_ClearPendingIRQ(MFS1RX_IRQn);
    NVIC_EnableIRQ(MFS1RX_IRQn);
    NVIC_SetPriority(MFS1RX_IRQn, L3_IRQ_LEVEL_MFS1_RX);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS2 == L3_ON) 
  if (pstcMfs == &MFS2)
  {
    // MFS2 interrupt settings
    NVIC_ClearPendingIRQ(MFS2TX_IRQn);
    NVIC_EnableIRQ(MFS2TX_IRQn);
    NVIC_SetPriority(MFS2TX_IRQn, L3_IRQ_LEVEL_MFS2_TX);
   
    NVIC_ClearPendingIRQ(MFS2RX_IRQn);
    NVIC_EnableIRQ(MFS2RX_IRQn);
    NVIC_SetPriority(MFS2RX_IRQn, L3_IRQ_LEVEL_MFS2_RX);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS3 == L3_ON) 
  if (pstcMfs == &MFS3)
  {
    // MFS3 interrupt settings
    NVIC_ClearPendingIRQ(MFS3TX_IRQn);
    NVIC_EnableIRQ(MFS3TX_IRQn);
    NVIC_SetPriority(MFS3TX_IRQn, L3_IRQ_LEVEL_MFS3_TX);
   
    NVIC_ClearPendingIRQ(MFS3RX_IRQn);
    NVIC_EnableIRQ(MFS3RX_IRQn);
    NVIC_SetPriority(MFS3RX_IRQn, L3_IRQ_LEVEL_MFS3_RX);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS4 == L3_ON) 
  if (pstcMfs == &MFS4)
  {
    // MFS4 interrupt settings
    NVIC_ClearPendingIRQ(MFS4TX_IRQn);
    NVIC_EnableIRQ(MFS4TX_IRQn);
    NVIC_SetPriority(MFS4TX_IRQn, L3_IRQ_LEVEL_MFS4_TX);
   
    NVIC_ClearPendingIRQ(MFS4RX_IRQn);
    NVIC_EnableIRQ(MFS4RX_IRQn);
    NVIC_SetPriority(MFS4RX_IRQn, L3_IRQ_LEVEL_MFS4_RX);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS5 == L3_ON) 
  if (pstcMfs == &MFS5)
  {
    // MFS5 interrupt settings
    NVIC_ClearPendingIRQ(MFS5TX_IRQn);
    NVIC_EnableIRQ(MFS5TX_IRQn);
    NVIC_SetPriority(MFS5TX_IRQn, L3_IRQ_LEVEL_MFS5_TX);
   
    NVIC_ClearPendingIRQ(MFS5RX_IRQn);
    NVIC_EnableIRQ(MFS5RX_IRQn);
    NVIC_SetPriority(MFS5RX_IRQn, L3_IRQ_LEVEL_MFS5_RX);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS6 == L3_ON) 
  if (pstcMfs == &MFS6)
  {
    // MFS6 interrupt settings
    NVIC_ClearPendingIRQ(MFS6TX_IRQn);
    NVIC_EnableIRQ(MFS6TX_IRQn);
    NVIC_SetPriority(MFS6TX_IRQn, L3_IRQ_LEVEL_MFS6_TX);
   
    NVIC_ClearPendingIRQ(MFS6RX_IRQn);
    NVIC_EnableIRQ(MFS6RX_IRQn);
    NVIC_SetPriority(MFS6RX_IRQn, L3_IRQ_LEVEL_MFS6_RX);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS7 == L3_ON)   
  if (pstcMfs == &MFS7)
  {
    // MFS7 interrupt settings
    NVIC_ClearPendingIRQ(MFS7TX_IRQn);
    NVIC_EnableIRQ(MFS7TX_IRQn);
    NVIC_SetPriority(MFS7TX_IRQn, L3_IRQ_LEVEL_MFS7_TX);
   
    NVIC_ClearPendingIRQ(MFS7RX_IRQn);
    NVIC_EnableIRQ(MFS7RX_IRQn);
    NVIC_SetPriority(MFS7RX_IRQn, L3_IRQ_LEVEL_MFS7_RX);
  }
  #endif

  return Ok;
}

/**
 ******************************************************************************
 ** \brief Device dependent de-initialization of interrupts according CMSIS
 **
 ** \param pstcMfs Pointer to MFS instance
 **
 ** \return Ok          Successful de-initialization
 **
 ******************************************************************************/
static en_result_t Mfs_DeInitIrq( stc_mfsn_t* pstcMfs ) 
{
  #if (L3_PERIPHERAL_ENABLE_MFS0 == L3_ON)
  if (pstcMfs == &MFS0)
  {
    // MFS0 interrupt settings
    NVIC_ClearPendingIRQ(MFS0TX_IRQn);
    NVIC_DisableIRQ(MFS0TX_IRQn);
    NVIC_SetPriority(MFS0TX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
   
    NVIC_ClearPendingIRQ(MFS0RX_IRQn);
    NVIC_DisableIRQ(MFS0RX_IRQn);
    NVIC_SetPriority(MFS0RX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS1 == L3_ON) 
  if (pstcMfs == &MFS1)
  {
    // MFS1 interrupt settings
    NVIC_ClearPendingIRQ(MFS1TX_IRQn);
    NVIC_DisableIRQ(MFS1TX_IRQn);
    NVIC_SetPriority(MFS1TX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
   
    NVIC_ClearPendingIRQ(MFS1RX_IRQn);
    NVIC_DisableIRQ(MFS1RX_IRQn);
    NVIC_SetPriority(MFS1RX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS2 == L3_ON) 
  if (pstcMfs == &MFS2)
  {
    // MFS2 interrupt settings
    NVIC_ClearPendingIRQ(MFS2TX_IRQn);
    NVIC_DisableIRQ(MFS2TX_IRQn);
    NVIC_SetPriority(MFS2TX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
   
    NVIC_ClearPendingIRQ(MFS2RX_IRQn);
    NVIC_DisableIRQ(MFS2RX_IRQn);
    NVIC_SetPriority(MFS2RX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS3 == L3_ON) 
  if (pstcMfs == &MFS3)
  {
    // MFS3 interrupt settings
    NVIC_ClearPendingIRQ(MFS3TX_IRQn);
    NVIC_DisableIRQ(MFS3TX_IRQn);
    NVIC_SetPriority(MFS3TX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
   
    NVIC_ClearPendingIRQ(MFS3RX_IRQn);
    NVIC_DisableIRQ(MFS3RX_IRQn);
    NVIC_SetPriority(MFS3RX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS4 == L3_ON) 
  if (pstcMfs == &MFS4)
  {
    // MFS4 interrupt settings
    NVIC_ClearPendingIRQ(MFS4TX_IRQn);
    NVIC_DisableIRQ(MFS4TX_IRQn);
    NVIC_SetPriority(MFS4TX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
   
    NVIC_ClearPendingIRQ(MFS4RX_IRQn);
    NVIC_DisableIRQ(MFS4RX_IRQn);
    NVIC_SetPriority(MFS4RX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS5 == L3_ON) 
  if (pstcMfs == &MFS5)
  {
    // MFS5 interrupt settings
    NVIC_ClearPendingIRQ(MFS5TX_IRQn);
    NVIC_DisableIRQ(MFS5TX_IRQn);
    NVIC_SetPriority(MFS5TX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
   
    NVIC_ClearPendingIRQ(MFS5RX_IRQn);
    NVIC_DisableIRQ(MFS5RX_IRQn);
    NVIC_SetPriority(MFS5RX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS6 == L3_ON) 
  if (pstcMfs == &MFS6)
  {
    // MFS6 interrupt settings
    NVIC_ClearPendingIRQ(MFS6TX_IRQn);
    NVIC_DisableIRQ(MFS6TX_IRQn);
    NVIC_SetPriority(MFS6TX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
   
    NVIC_ClearPendingIRQ(MFS6RX_IRQn);
    NVIC_DisableIRQ(MFS6RX_IRQn);
    NVIC_SetPriority(MFS6RX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  
  #if (L3_PERIPHERAL_ENABLE_MFS7 == L3_ON)   
  if (pstcMfs == &MFS7)
  {
    // MFS7 interrupt settings
    NVIC_ClearPendingIRQ(MFS7TX_IRQn);
    NVIC_DisableIRQ(MFS7TX_IRQn);
    NVIC_SetPriority(MFS7TX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
   
    NVIC_ClearPendingIRQ(MFS7RX_IRQn);
    NVIC_DisableIRQ(MFS7RX_IRQn);
    NVIC_SetPriority(MFS7RX_IRQn, L3_DEFAULT_INTERRUPT_LEVEL);
  }
  #endif
  
  return Ok;
}

/**
 ******************************************************************************
 ** \brief MFS transmit interrupt service routine.
 **
 ** This Fct is called on each Transmit Interrupt set by the MFS. This highly
 ** depends on the Configuration of the INT Sources. Interrupts might occur on
 ** TX Fifo empty, last bit has been shifted out, ...
 ** The data from the internal Ring Buffer is put into the Fifo until there's
 ** no more space left or all data has been transferred to the Fifo.
 ** If the referenced MFS does not have a FIFO single data is put.
 **
 ** In LIN mode this handler also checks for LIN Break detection. If it is
 ** detected, possible reception errors are cleared, the reception register
 ** (RDR) is flushed and (if not NULL) a dedicated callback function is called.
 ** Afterwards the Fct is left by return w/o any further transmission action.
 ** 
 ******************************************************************************/

void MfsIrqHandlerTx( volatile stc_mfsn_t* pstcMfs, 
                             stc_mfs_intern_data_t* pstcMfsInternData
                            )
{
    stc_mfs_buffer_t* pstcBuffer = &pstcMfsInternData->stcTxBuffer ;
    uint_fast16_t fu16Loop ;
    volatile uint8_t u8DummyData;

    // LIN mode
    if ((pstcMfs->SMR & MFS_SMR_MODEBITS_MASK) == MFS_SMR_LIN_MODE)
    {
      // LIN break detected?
      if (pstcMfs->u8SSR.PE_LBD == 1)
      {
        u8DummyData = pstcMfs->RDR;
        pstcMfs->u8SSR.PE_LBD = 0;  // Clear LIN break detection
        pstcMfs->u8SSR.REC    = 1;  // Clear possible reception errors
        
        // Break Callback if specified
        if (pstcMfsInternData->pfnLinBreakDetectionCbFunction != NULL)
        {
          pstcMfsInternData->pfnLinBreakDetectionCbFunction();
        }
            
        return;
      }
    }
    
    if ( pstcMfsInternData->enFifoAvailable == MfsHasFifo)
    {
      // Get # of Data to put into HW FIFO, whichever space is less
      // Temporary var only used to define the order of volatile accesses to
      // Get rid of the compiler warning
      uint_fast16_t u16Cmp = pstcBuffer->u16FillCount ;
      fu16Loop = MIN( u16Cmp,
                      MFS_FIFO_MAX_VAL - pstcMfs->FBYTE1
                    ) ;
    }
    else
    {
     // fu16Loop = (pstcBuffer->u16FillCount > 0);
      fu16Loop = 1;
    }

    while ( fu16Loop != 0 )
    {
      // Put data from Buffer into FIFO
      pstcMfs->TDR = pstcBuffer->pcBuffer[ pstcBuffer->u16OutIndex ] ;
  
      // update tail
      pstcBuffer->u16OutIndex = \
                    (pstcBuffer->u16OutIndex +1) % pstcBuffer->u16BufferSize ;
      pstcBuffer->u16FillCount-- ;
  
      fu16Loop-- ;	

    } // while ( fu16Loop != 0 )

    if ( pstcBuffer->u16FillCount == 0 )
    {
      // If no more bytes to sent: Disable transmission interrupt(s)
      if ( pstcMfsInternData->enFifoAvailable == MfsHasFifo)
      {
        // Disable TX Fifo Interrupt
        pstcMfs->u8FCR1.FTIE = 0 ;
      }
      else
      {
        // Disable TX Interrupt
        pstcMfs->u8SCR.TIE = 0; 
      }
      
      if ( pstcMfsInternData->pfnTransmitCbFunction != NULL )
      {
        // Check for Callback active on TX Buffer empty
        if ( pstcMfsInternData->enTxCallbackMode == MfsOnTxBufferEmpty )
        {
          // Call transmit callback function.
          pstcMfsInternData->pfnTransmitCbFunction() ; 
        } // if ( pstcMfsInternData->enTxCallbackMode == MfsOnTxBufferEmpty )
          
        // Check for Callback active on TX finished
        if ( pstcMfsInternData->enTxCallbackMode == MfsOnTxFinished )
        {           
           if (pstcMfsInternData->pfnTransmitCbFunction != NULL)
           {
              pstcMfsInternData->pfnTransmitCbFunction(); 
           }
        } // if ( pstcMfsInternData->enTxCallbackMode == MfsOnTxFinished )
      } //  if ( pstcMfsInternData->pfnTransmitCbFunction != NULL )       
    } // if (pstcTxBuffer->u16FillCount == 0)
} // MfsIrqHandlerTx

/**
 ******************************************************************************
 ** \brief Write data to MFS module synchronously or asynchronously
 **
 ** The data provided by Mfs_Write#pcData is copied into the internal TX buffer and the
 ** transmission (via TX interrupt) is started, if not ongoing already. 
 ** Depending on the Mfs_Write#bWait parameter, the function return behavior is different.
 **
 ** For an asynchronous (non-blocking) call (Mfs_Write#bWait = FALSE), the free size of 
 ** the internal buffer must be sufficient to take all data (Mfs_Write#pcData) of length 
 ** Mfs_Write#u16DataLength, otherwise the function will return ErrorBufferFull. 
 ** After all data is copied into the internal buffer, the function will return 
 ** immediately. The transmission may be pending when the function returns.
 **
 ** For a synchronous (blocking) call (Mfs_Write#bWait = TRUE), the function will wait 
 ** until all data is transferred to the MFS hardware FIFO. The transmission 
 ** may be pending when the function returns.
 ** If the referenced MFS does not have a FIFO single data is written.
 **
 ** \param [in]  pstcMfs     Pointer to MFS instance register area
 ** \param [in]  pcData        Transmit data buffer holdinmg data to transmit
 ** \param [in]  u16WriteCount Number of characters to write, must be at least 1
 ** \param [in]  bWait         If TRUE, synchronously wait until all data is transferred 
 **                            into the hardware Fifo buffer
 **                            If FALSE, put data into internal TX buffer and return 
 **                            immediately
 **
 ** \retval Ok                        Write data successfully done or started.
 ** \retval ErrorInvalidParameter     If one of following conditions are met:
 **         - pstcMfs == NULL
 **         - pcData == NULL
 **         - u16WriteCount == 0
 **         - pstcMfsInternData == NULL (invalid or disabled MFS unit (L3_PERIPHERAL_ENABLE_MFS))  
 ** \retval ErrorBufferFull           Insufficient free size of TX buffer to 
 **                                   take all data (in case of 
 **                                   Mfs_Write#bWait = FALSE only)
 ******************************************************************************/
en_result_t Mfs_Write( stc_mfsn_t* pstcMfs, 
                         const char_t* pcData, 
                         uint16_t u16WriteCount, 
                         boolean_t bWait
                       )
{
    stc_mfs_intern_data_t* pstcMfsInternData ;
    stc_mfs_buffer_t*      pstcBuffer ;
    uint_fast16_t          fu16DataSent ;
    uint_fast16_t          fu16Idx ;
    boolean_t              bTxInProgress ;

    // Check for NULL Pointers
    if ( pstcMfs == NULL ||
         pcData  == NULL
       )
    {
        return ErrorInvalidParameter ;
    }

    // Check if nothing to do
    if ( u16WriteCount == 0 )
    {
        return Ok ;
    }

    // Get ptr to internal data struct ...
    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    // ... and check
    if ( pstcMfsInternData == NULL )
    {
        return ErrorInvalidParameter ;
    }

    // Get ptr to internal transmit Buffer
    pstcBuffer = &pstcMfsInternData->stcTxBuffer ;

    // Check if ring buffer can take all bytes (blocking only)
    if ( bWait == FALSE &&
        (u16WriteCount > (pstcBuffer->u16BufferSize - pstcBuffer->u16FillCount))
       )
    {
        // not enough space left if non-blocking mode is requested
        return ErrorBufferFull ;
    }

    // Loop until all data has been sent (blocking only)
    // If non-blocking mode is requested, it is guaranteed here that the
    // provided data will fit into the tx buffer
    while ( u16WriteCount )
    {
        // Check for transmission ongoing
        bTxInProgress = pstcMfs->u8SSR.TDRE;
        
        // In case, a transmission is already pending
        if ( bTxInProgress == TRUE )
        {
          if (pstcMfsInternData->enFifoAvailable == MfsHasFifo)
          {  
            // Disable transmission Fifo interrupt during copy
            pstcMfs->u8FCR1.FTIE = 0;
          }
          else // if (pstcMfsInternData->enFifoAvailable == MfsHasFifo)
          {
            // Disable transmission interrupt during copy
            pstcMfs->u8SCR.TIE = 0;
          }
        }

        // Copy data to provided destinaton buffer and save # bytes been read
        // determine free size in TX buffer
        fu16DataSent = MIN((pstcBuffer->u16BufferSize) - (pstcBuffer->u16FillCount),
                            u16WriteCount
                          ) ;

        // store bytes in TX buffer
        for ( fu16Idx = 0; fu16Idx < fu16DataSent; fu16Idx++ )
        {
            pstcBuffer->pcBuffer[pstcBuffer->u16InIndex] = *pcData ;
            pcData++ ;

            // Update in index
            pstcBuffer->u16InIndex++ ;
            if ( pstcBuffer->u16InIndex == pstcBuffer->u16BufferSize )
            {
                pstcBuffer->u16InIndex = 0 ; // wrapped around
            }
        }

        pstcBuffer->u16FillCount += fu16DataSent ;
        u16WriteCount -= fu16DataSent ;

        if (pstcMfsInternData->enFifoAvailable == MfsHasFifo)
        {     
          // Now enable transmission Fifo interrupt to trigger send operation
          pstcMfs->u8FCR1.FTIE = 1;
        }
        else // if (pstcMfsInternData->enFifoAvailable == MfsHasFifo)
        {
          // Now enable transmission interrupt to trigger send operation
          pstcMfs->u8SCR.TIE = 1;
        }

        // Wait until all data has been tranferred to the MFS HW FIFO
        // This is fully INT driven
        while ( pstcBuffer->u16FillCount != 0 )
        {
            L3_WAIT_LOOP_HOOK() ;
        }
    } // while (u16WriteCount)

    return Ok ;
} // Mfs_Write

/**
 ******************************************************************************
 ** \brief MFS receive interrupt service routine.
 **
 ** This Fct is called on each Receive Interrupt set by the MFS. It stores
 ** the data from the FIFO into the internal Ring Buffer an triggers a callback
 ** function if the specific Fill Level of the internal Ring Buffer is reached.
 ** If the referenced MFS does not have a FIFO single data is read.
 **
 ** In LIN mode this ISR also handles the LIN Break detection.
 **
 ******************************************************************************/

void MfsIrqHandlerRx( volatile stc_mfsn_t* pstcMfs, 
                               stc_mfs_intern_data_t* pstcMfsInternData
                             )
{
  volatile uint8_t  u8DummyData ;  
  stc_mfs_buffer_t* pstcRxBuffer ;

  // Pointer to the receive buffer  
  pstcRxBuffer = &pstcMfsInternData->stcRxBuffer ;

  if ( pstcMfsInternData->enFifoAvailable == MfsHasFifo)
  {
    // Now read all available bytes from hardware FIFO (if available) to buffer
    while (pstcMfs->FBYTE2 != 0)
    {
      // If there is empty space in RX buffer
      if ( pstcRxBuffer->u16FillCount < pstcRxBuffer->u16BufferSize )
      {
        // Read data from RX FIFO, store in RX buffer
        pstcRxBuffer->pcBuffer[pstcRxBuffer->u16InIndex] = (char_t)pstcMfs->RDR;

        // Increment in index
        pstcRxBuffer->u16InIndex++ ;
        if ( pstcRxBuffer->u16InIndex == pstcRxBuffer->u16BufferSize )
        {
          pstcRxBuffer->u16InIndex = 0 ;
        }
        // Count bytes in RX-FIFO
        pstcRxBuffer->u16FillCount++ ;
      }
      else
      {
        // We still need to do a dummy read to clear pending interrupts
        u8DummyData = pstcMfs->RDR;
        pstcRxBuffer->bOverflow = TRUE ;
        break ;
      }
    }
  }
  else
  {
    pstcRxBuffer->pcBuffer[pstcRxBuffer->u16InIndex] = (char_t)pstcMfs->RDR;
    
    // Increment in index
    pstcRxBuffer->u16InIndex++ ;
    if ( pstcRxBuffer->u16InIndex == pstcRxBuffer->u16BufferSize )
    {
      pstcRxBuffer->u16InIndex = 0 ;
    }
    // Count bytes in RX
    pstcRxBuffer->u16FillCount++ ;
  }
  
  // Check if receive callback function is valid.
  if ( pstcMfsInternData->pfnReceiveCbFunction != NULL )
  {
    if ( pstcRxBuffer->u16FillCount >= pstcMfsInternData->u16RxCallbackBufferFillLevel )
    {
      // Call receive callback function. 
      pstcMfsInternData->pfnReceiveCbFunction() ; 
    }            
  }
} // MfsIrqHandlerRx

/**
 ******************************************************************************
 ** \brief Read received data from MFS module synchronously or asynchronously
 **
 ** The received data is copied from internal RX buffer (filled by RX interrupt)
 ** into the provided data buffer Mfs_Read#pcData. The size is defined by 
 ** Mfs_Read#pu16ReadCount. Depending on the Mfs_Read#bWait parameter, the function behavior is
 ** different.
 **
 ** For an asynchronous (non-blocking) call (Mfs_Read#bWait = FALSE), the function will
 ** return immediately after all currently available characters (in SW ring 
 ** buffer and HW FIFO) are copied into the provided buffer (Mfs_Read#pcData) or the
 ** maximum count (Mfs_Read#pu16ReadCount) is reached. The value returned by 
 ** Mfs_Read#puReadCount gives the count of characters that was read actually.
 ** If the referenced MFS does not have a FIFO single data is read.
 **
 ** For a synchronous (blocking) call (Mfs_Read#bWait == TRUE), the function will return
 ** after the requested count of characters (Mfs_Read#pu16ReadCount) is received 
 ** completely. This should be used with caution as the full application can get
 ** stuck if no further data is received.
 **
 ** Anyway, Receive interrupt is used in any case.
 **
 ** \param [in]       pstcMfs       Pointer to MFS instance register area
 ** \param [in,out]   pcData        Buffer to store received characters
 ** \param [in]       pu16DataCount Ptr. to variable for number of bytes been read
 ** \param [in]       u16ReadCount  Maximum number of characters to read 
 **                                  (ensure sufficient size of Mfs_Read#pcData!)
 ** \param [in]       bWait         If TRUE, synchronously wait until Mfs_Read#u16ReadCount 
 **                                  bytes have been received.
 **                                 If FALSE, read all available and return immediately.
 **
 ** \retval Ok                      Read data successfully done or started.
 ** \retval ErrorInvalidParameter   If one of following conditions are met:
 **             - pstcMfs == NULL
 **             - pcData == NULL
 **             - pstcMfsInternData == NULL (invalid or disabled MFS unit (L3_PERIPHERAL_ENABLE_MFS)) 
 ** \retval ErrorOperationInProgress  If the following condition is met:
 **             - An asynchronous transmission is still ongoing while another asynchronous operation should be started.  
 ******************************************************************************/
en_result_t Mfs_Read( stc_mfsn_t* pstcMfs, 
                        char_t*   pcData, 
                        uint16_t* pu16DataCount, 
                        uint16_t  u16ReadCount, 
                        boolean_t bWait
                      )
{
    stc_mfs_intern_data_t*   pstcMfsInternData ;
    stc_mfs_buffer_t*        pstcBuffer ;
    uint_fast16_t            fu16Idx ;
    uint_fast16_t            fu16Length ;
    uint16_t                 u16BytesToReadLeft ;

    // Check for NULL Pointers
    if ( pstcMfs == NULL ||
         pcData  == NULL
       )
    {
      if ( pu16DataCount != NULL )
      {
          *pu16DataCount = 0 ;
      }  
      return ErrorInvalidParameter ;
    }

    // Check for nothing to do
    if ( u16ReadCount == 0 && pu16DataCount != NULL )
    {
        *pu16DataCount = 0 ;
        return Ok ;
    }
    
    // Save Read Count for later use
    u16BytesToReadLeft = u16ReadCount ;
    *pu16DataCount = 0 ;   // Preset to default

    // Get ptr to internal data struct ...
    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    // ... and check
    if (pstcMfsInternData == NULL)
    {
      return ErrorInvalidParameter ;
    }

    // Get ptr to internal receive Buffer
    pstcBuffer = &pstcMfsInternData->stcRxBuffer ;

    // Adjust FIFO, if u16ReadCount less than FIFO depth
    if ( pstcMfsInternData->enFifoAvailable == MfsHasFifo)
    {
      if (u16ReadCount < MFS_FIFO_MAX_VAL)
      {
        pstcMfs->FBYTE2 = u16ReadCount;
      }
      else
      {
        pstcMfs->FBYTE2 = MFS_FIFO_MAX_VAL; 
      }
    }
    
    // Enable reception interrupt
    pstcMfs->u8SCR.RIE = 1;

    // Read all available bytes from ring buffer, blocking.
    while ( u16BytesToReadLeft > 0 )
    {
        if ( bWait == TRUE )
        {
            while ( pstcBuffer->u16FillCount == 0 )
            {
                // Wait until at least one byte is available
                L3_WAIT_LOOP_HOOK() ;
            }
        }
        else if ( pstcBuffer->u16FillCount == 0 )
        {
            return Ok ;
        }

        // Disable reception interrupt
        pstcMfs->u8SCR.RIE = 0;
        
        // Copy data to destinaton buffer and save no. of bytes been read
        //
        // get number of bytes to read     
        fu16Length = MIN( pstcBuffer->u16FillCount, u16ReadCount ) ;

        // if there are any bytes left to read
        if ( fu16Length != 0 )
        {
            // read bytes out of RX buffer
            for ( fu16Idx = *pu16DataCount; fu16Idx < (fu16Length + *pu16DataCount); fu16Idx++ )
            {
                pcData[fu16Idx] = pstcBuffer->pcBuffer[pstcBuffer->u16OutIndex];
                // Update out index
                pstcBuffer->u16OutIndex++ ;
                if ( pstcBuffer->u16OutIndex == pstcBuffer->u16BufferSize )
                {
                    pstcBuffer->u16OutIndex = 0 ;
                }
            }
            pstcBuffer->u16FillCount -= fu16Length ; // Update fill counter
        } // if ( fu16Length != 0 )

        *pu16DataCount     += fu16Length ; // Provide no. of read to the caller
        u16BytesToReadLeft -= fu16Length ; // Some data processed

        // Adjust FIFO, if bytes to read left less than FIFO depth
        if ( pstcMfsInternData->enFifoAvailable == MfsHasFifo)
        {
          if (u16BytesToReadLeft < MFS_FIFO_MAX_VAL)
          {
            pstcMfs->FBYTE2 = u16BytesToReadLeft;
          }
        }
        // Reenable reception interrupt
        pstcMfs->u8SCR.RIE = 1;
    }
    
    return Ok ;
} // Mfs_Read

/**
 ******************************************************************************
 ** \brief Read single character from MFS module synchronously or 
 **         asynchronously
 **
 ** The received data is returned top the caller, no error checking is done.
 **
 ** For an asynchronous (non-blocking) call (Mfs_ReadChar#bWait = FALSE), 
 ** the function will return immediately after any available character (in SW
 ** ring buffer and HW FIFO [if available]) is found.
 **
 ** For a synchronous (blocking) call (Mfs_ReadChar#bWait == TRUE), the 
 ** function will return after receipt of a single character.
 **
 ** Receive interrupt is used in any case.
 **
 ** \note No error checking is done while reading from the MFS channel
 **
 ** \param [in]       pstcMfs       Pointer to MFS instance register area
 ** \param [in]       bWait         If TRUE, synchronously wait until Mfs_Read#u16ReadCount 
 **                                 bytes have been received (no interrupts are needed).
 **                                 If FALSE, read all available and return immediately.
 **
 ** \retval char_t    cChar         Character read
 ******************************************************************************/
char_t Mfs_ReadChar( stc_mfsn_t* pstcMfs, boolean_t bWait )
{
    char_t cChar ;
    uint16_t u16DataRead ;
    
    Mfs_Read( pstcMfs, &cChar, &u16DataRead, 1, bWait ) ;
    if ( u16DataRead == 0 )
    {
        // No character got, return zero to indicate that
        return '\0' ;
    }
    return cChar ;
} // Mfs_ReadChar

/**
 ******************************************************************************
 ** \brief Return number of pending TX data (Sum of HW Fifo and Ring Buffer)
 ******************************************************************************/ 
static uint16_t Mfs_GetTxFillCount( volatile stc_mfsn_t* pstcMfs ) 
{
    stc_mfs_intern_data_t* pstcMfsInternData ; // Ptr. to internal data
    stc_mfs_buffer_t* pstcBuffer ; // Ptr. to internal software Buffer

    // check fro non-null pointers
    if ( pstcMfs == NULL )
    {
        return (uint16_t) -1 ; // just return a big value in case of error
    }

    // Get ptr to internal data struct ...
    pstcMfsInternData = MfsGetInternDataPtr( (stc_mfsn_t*)pstcMfs ) ;
    // ... and check
    if (pstcMfsInternData == NULL)
    {
        return (uint16_t) -1 ; // just return a big value in case of error
    }

    // Get ptr to internal receive Buffer
    pstcBuffer = &pstcMfsInternData->stcTxBuffer ;

    if ( pstcMfsInternData->enFifoAvailable == MfsHasFifo)
    {
       return pstcBuffer->u16FillCount + pstcMfs->FBYTE1;
    }
    return pstcBuffer->u16FillCount;
} // Mfs_GetTxFillCount


/**
 ******************************************************************************
 ** \brief Return number of pending RX data (Sum of HW Fifo and Ring Buffer)
 ******************************************************************************/ 
static uint16_t Mfs_GetRxFillCount( volatile stc_mfsn_t* pstcMfs ) 
{
    stc_mfs_intern_data_t* pstcMfsInternData ; // Ptr. to internal data
    stc_mfs_buffer_t* pstcBuffer ; // Ptr. to internal software Buffer

    // check fro non-null pointers
    if (pstcMfs == NULL)
    {
        return (uint16_t) -1 ; // just return a big value in case of error
    }

    // Get ptr to internal data struct ...
    pstcMfsInternData = MfsGetInternDataPtr( (stc_mfsn_t*)pstcMfs ) ;
    // ... and check
    if (pstcMfsInternData == NULL)
    {
        return (uint16_t) -1 ; // just return a big value in case of error
    }

    // Get ptr to internal receive Buffer
    pstcBuffer = &pstcMfsInternData->stcRxBuffer ;

    if ( pstcMfsInternData->enFifoAvailable == MfsHasFifo)
    {
       return pstcBuffer->u16FillCount + pstcMfs->FBYTE2;
    }
    
    return pstcBuffer->u16FillCount;
} // Mfs_GetRxFillCount

/**
 ******************************************************************************
 ** \brief Check for empty Buffers
 ** This Function checks all Buffers (software and hardware) for an pending
 ** Operation.
 **
 ** \param [in]  pstcMfs     Pointer to MFS instance register area
 **
 ** \retval Ok                    All Buffers are empty
 ** \retval ErrorOperationInProgress Buffers not empty
 ** \retval ErrorInvalidParameter If one of following conditions are met:
 **         - pstcMfs == NULL
 ******************************************************************************/
en_result_t Mfs_BuffersEmpty( stc_mfsn_t* pstcMfs ) 
{
    // Check for NULL ptr
    if (pstcMfs == NULL)
    {
      return ErrorInvalidParameter;
    }

    if ( Mfs_GetRxFillCount( pstcMfs ) ||
         Mfs_GetTxFillCount( pstcMfs )
       )
    {
        return ErrorOperationInProgress ;
    }
    return Ok ;
} // Mfs_BuffersEmpty 



/**
 ******************************************************************************
 ** \brief Simultaneously transmit and receive data as synchronous master
 **
 ** This function will transmit and receive the same amount of data, based on 
 ** the serial (shift) clock signal (MFS SCK pin) in synchronous master or 
 ** slave mode. The operation mode of the MFS instance must be setup for 
 ** synchronous master or slave mode.
 **
 ** While the data in Mfs_SynchronousTransfer#pu8TxData is transmitted the
 ** input data is received and stored in Mfs_SynchronousTransfer#pu8RxData.
 ** The function operates in synchronous (blocking) mode 
 ** only i.e. it will wait until the amount of data defined by
 ** Mfs_SynchronousTransfer#u8TransferSize 
 ** is transmitted/received. No MFS internal buffers or transfer counters
 ** are used.
 ** The TX/RX callback functions will not be called.
 ** Note, that in synchronous slave mode, the transfer is controlled by 
 ** the external master device, providing the serial shift clock. This may 
 ** cause this function to block very long time.
 **
 ** Because this function uses blocking method, no interrupts are needed and
 ** therfore are not used. Also no FIFO operation is performed.
 **
 ** \note Asynchronous (non-blocking) TX/RX operations are provided by using 
 **       the functions Mfs_Write() and Mfs_Read() for MFS synchronous master 
 **       and slave modes, too. Note, that these functions do not support
 **       full-duplex operation!
 **
 ** \param [in] pstcMfs             Pointer to MFS instance register area
 ** \param [in] pu8TxData           Pointer to transmit data buffer.
 ** \param [in] pu8RxData           Pointer to receive data buffer, can be NULL.
 ** \param [in] u8TransferSize      Must be at least 1
 **
 ** \retval Ok                        Transfer done successfully.
 ** \retval ErrorInvalidParameter     If any of following conditions are met:
 **         - pstcMfs == NULL
 **         - pu8TxData == NULL
 **         - u8TransferSize == 0 
 **         - pstcMfsInternData == NULL (invalid or disabled MFS unit (L3_PERIPHERAL_ENABLE_MFS)) 
 ** \retval ErrorOperationInProgress  A transmission is still pending or the RX buffer is not empty.
 ** \retval ErrorInvalidMode          MFS module was not initalised in mfs synchronous mode.
 **
******************************************************************************/
en_result_t Mfs_SynchronousTransfer( stc_mfsn_t* pstcMfs, 
                                       uint8_t* pu8TxData, 
                                       uint8_t* pu8RxData, 
                                       uint8_t u8TransferSize
                                     )
{
    stc_mfs_intern_data_t* pstcMfsInternData ;
    uint8_t u8DataToSendCounter ;
    uint8_t u8DataReceivedCounter ;

    // Check pointers
    if ( (pstcMfs   == NULL) ||
         (pu8TxData == NULL)
       )
    {
        return ErrorInvalidParameter;
    }
    
    // Check for 0 < transmission length
    if (u8TransferSize == 0)
    {
        return ErrorInvalidParameter ;
    }    
    
    // Get ptr to internal data struct ...
    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    // ... and check
    if (pstcMfsInternData == NULL)
    {
      return ErrorInvalidParameter ;
    }

    // check for valid mode (only sync mode allowed)
    if ((pstcMfs->u8SMR.MD0 != 0 ) && (pstcMfs->u8SMR.MD1 != 1 ))
    {
        return ErrorInvalidMode ;
    }

    u8DataToSendCounter   = 0 ;
    u8DataReceivedCounter = 0 ;
    
    while (u8DataReceivedCounter != u8TransferSize)
    {
      if (u8DataToSendCounter < u8TransferSize)
      {
        // Send data
        while(0 == pstcMfs->u8SSR.TDRE) ;  // Wait for TDR empty
        {
          L3_WAIT_LOOP_HOOK(); 
        }
        pstcMfs->TDR = pu8TxData[ u8DataToSendCounter ] ;
        u8DataToSendCounter++;
      }

      // To avoid inter-byte spikes on SOT, send out 2nd byte immediately,
      //   if TDR is empty!
      if ((u8DataToSendCounter == 1) && (u8TransferSize > 1))
      {
        while(0 == pstcMfs->u8SSR.TDRE) ;  // Wait for TDR empty
        {
          L3_WAIT_LOOP_HOOK(); 
        }
        pstcMfs->TDR = pu8TxData[ u8DataToSendCounter ] ;
        u8DataToSendCounter++;
      }
      
      // wait for reception finsihed
      while(0 == pstcMfs->u8SSR.RDRF) ;
      {
        L3_WAIT_LOOP_HOOK(); 
      }
      pu8RxData[ u8DataReceivedCounter ] = pstcMfs->RDR ;
      u8DataReceivedCounter++;
    }
    
    return Ok ;
} // Mfs_SynchronousTransfer

/**
 ******************************************************************************
 ** \brief Stop MFS receive module transfers and clear buffer.
 **
 ** The hardware receive fifos and the software buffer is cleared.
 ** Any data which is in the receive buffer at this time will be lost!  
 ** All settings done by #Mfs_Init() will be preserved.
 ** DMA operation for read will be disabled and read access 
 ** to receive buffer by CPU will be enabled.
 **
 ** \param [in]   pstcMfs         Pointer to register area of a MFS module.
 **
 ** \retval Ok                      Purge successfully done. 
 ** \retval ErrorInvalidParameter   If one of following conditions are met:
 **         - pstcMfs == NULL
 **         - pstcMfsInternData == NULL 
 **                (invalid or disabled MFS unit (L3_PERIPHERAL_ENABLE_MFS)) 
 ******************************************************************************/
en_result_t Mfs_PurgeRx( stc_mfsn_t* pstcMfs )
{
    stc_mfs_intern_data_t* pstcMfsInternData ;

    // Check for NULL Pointer
    if (pstcMfs == NULL)
    {
        return ErrorInvalidParameter;
    }

    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    
    if (pstcMfsInternData == NULL)
    {
        return ErrorInvalidParameter;
    }

    // Reset receive ring buffers index parameter.
    pstcMfsInternData->stcRxBuffer.u16FillCount = 0 ;    
    
    // Reset FIFO
    pstcMfs->u8FCR0.FCL2 = 1;
  
    return Ok;
} // Mfs_PurgeRx


/**
 ******************************************************************************
 ** \brief Stop MFS module tx transfer and clear buffers.
 **
 ** The hardware transmit fifo and the software buffer is cleared.
 ** Any data which is not transferred at this time will be lost!  
 ** All settings done by #Mfs_Init() will be preserved.
 ** DMA operation for write will be disabled and write access 
 ** to transmit buffer by CPU will be enabled.
 **
 ** \param [in]   pstcMfs         Pointer to register area of a MFS module.
 **
 ** \retval Ok                      Purge successfully done. 
 ** \retval ErrorInvalidParameter   If one of following conditions are met:
 **         - pstcMfs == NULL
 **         - pstcMfsInternData == NULL 
 **                (invalid or disabled MFS unit (L3_PERIPHERAL_ENABLE_MFS)) 
 ******************************************************************************/
en_result_t Mfs_PurgeTx( stc_mfsn_t* pstcMfs )
{
    stc_mfs_intern_data_t* pstcMfsInternData ;

    // Check for NULL Pointer
    if (pstcMfs == NULL)
    {
        return ErrorInvalidParameter;
    }

    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    
    if (pstcMfsInternData == NULL)
    {
        return ErrorInvalidParameter;
    }
    
    // Reset transmit ring buffers index parameter.
    pstcMfsInternData->stcTxBuffer.u16FillCount = 0 ;
    
    // Reset FIFO
    pstcMfs->u8FCR0.FCL1 = 1;
    
    return Ok;
} // Mfs_PurgeTx



/**
 ******************************************************************************
 ** \brief Stop all MFS module transfers and clear all buffers.
 **
 ** The hardware receive and transmit fifos and the software buffers are cleared.
 ** Any data which is not transferred at this time will be lost!  
 ** All settings done by #Mfs_Init() will be preserved.
 ** DMA operation for write and read will be disabled and read/write access 
 ** to receive and transmit buffers by CPU will be enabled.
 **
 ** \param [in]   pstcMfs         Pointer to register area of a MFS module.
 **
 ** \retval Ok                      Purge successfully done. 
 ** \retval ErrorInvalidParameter   If one of following conditions are met:
 **         - pstcMfs == NULL
 **         - pstcMfsInternData == NULL 
 **                (invalid or disabled MFS unit (L3_PERIPHERAL_ENABLE_MFS)) 
 **
 ******************************************************************************/
en_result_t Mfs_Purge( stc_mfsn_t* pstcMfs )
{
    stc_mfs_intern_data_t* pstcMfsInternData ;

    // Check for NULL Pointer
    if (pstcMfs == NULL)
    {
        return ErrorInvalidParameter;
    }

    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    
    if (pstcMfsInternData == NULL)
    {
        return ErrorInvalidParameter;
    }

    Mfs_PurgeRx( pstcMfs ) ;
    Mfs_PurgeTx( pstcMfs ) ;
    
    // Clear MFS by setting the two Software Reset bits
    pstcMfs->u8SCR.UPCL = 1;
      
    // Resets receive and transmit hardware Fifo
    pstcMfs->u8FCR0.FCL1 = 1;
    pstcMfs->u8FCR0.FCL2 = 1;
    
    return Ok;
} // Mfs_Purge

/**
 ******************************************************************************
 ** \brief Write a '\\0'-terminated string to MFS module synchronously or 
 **          asynchronously
 **
 ** The characeters provided by Mfs_WriteString#pcData is copied into the 
 ** internal TX buffer and the transmission (via TX interrupt) is started, 
 ** if not ongoing already. 
 ** Depending on the Mfs_WriteString#bWait parameter, the function return 
 ** behavior is different.
 **
 ** For an asynchronous (non-blocking) call (Mfs_WriteString#bWait = FALSE), 
 ** the free size of  the internal buffer must be sufficient to take all data
 ** (Mfs_Write#pcData) of length 
 ** Mfs_Write#u16DataLength, otherwise the function will return ErrorBufferFull. 
 ** After all data is copied into the internal buffer, the function will return 
 ** immediately. The transmission may be pending when the function returns.
 **
 ** For a synchronous (blocking) call (Mfs_WriteString#bWait = TRUE), the 
 ** function will wait  until all data is transferred to the MFS hardware 
 ** FIFO. The transmission may be pending when the function returns.
 ** If the referenced MFS does not have a FIFO, single data is transfered.
 **
 ** \note No error checking is done while reading from the MFS channel
 **
 ** \param [in]  pstcMfs     Pointer to MFS instance register area
 ** \param [in]  pszString        String to transmit
 ** \param [in]  bWait         If TRUE, synchronously wait until all data is 
 **                             transferred into the hardware Fifo buffer
 **                             If FALSE, put data into internal TX buffer and 
 **                             return immediately
 **
 ** \retval Ok                        Write data successfully done or started.
 ** \retval ErrorInvalidParameter     If one of following conditions are met:
 **         - pstcMfs == NULL
 **         - pcData == NULL
 **         - pstcMfsInternData == NULL 
 **                (invalid or disabled MFS unit (L3_PERIPHERAL_ENABLE_MFS))  
 ******************************************************************************/
en_result_t Mfs_WriteString(  stc_mfsn_t* pstcMfs,
                               char_t*       pszString,
                               boolean_t     bWait
                             )
{
    stc_mfs_intern_data_t* pstcMfsInternData ;

    uint_fast8_t fu8Length = 0 ;
    en_result_t  enRet    = Ok ;
    char_t       cLastChr = '\0' ;

    // Check for NULL Pointer
    if ( pstcMfs == NULL ||
         pszString == NULL
      )
    {
        return ErrorInvalidParameter ;
    }

    // Get pointer to internal data structure ...
    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    // .. and check for null pointer
    if ( pstcMfsInternData == NULL )
    {
        return ErrorInvalidParameter ;
    }

    // Determine string length
    while ( pszString[fu8Length] != '\0' )
    {
        cLastChr = pszString[fu8Length] ;
        fu8Length++ ;
    }

    enRet = Mfs_Write( pstcMfs, pszString, fu8Length, bWait ) ;
    if ( enRet != Ok )
    {
        return enRet ;
    }

    if ( pstcMfsInternData->bSubstLFwithCRLF == TRUE &&
         cLastChr == '\n'
       )
    {
        // Auto add CR to LF at line end, if configured
        enRet = Mfs_Write( pstcMfs, "\r", 1, bWait ) ;
    }

    return enRet ;
} // Mfs_WriteString


/**
 ******************************************************************************
 ** \brief Read a EOL-terminated string from the MFS module (a)synchronously
 **
 ** The character buffer provided by Mfs_ReadString#pcData is used to copy
 ** the received data.
 **
 ** The free size of the buffer must be sufficient to take all data
 ** (Mfs_Read#pcData) of length Mfs_Read#u16MaxChars +1 (Null-Terminator).
 ** After Mfs_Read#u16MaxChars data is received, the function will return
 ** immediately without terminating the buffer.
 ** If the provided terminator is detected the function will null-terminate
 ** the buffer and return
 **
 ** \note No error checking is done while reading from the MFS channel.
 ** \note Given character array will be null terminated by the function itself.
 ** \note The reception FIFO is disabled because of random number of characters
 **       before receiving the terminator character.
 **
 ** \param [in]  pstcMfs       Pointer to MFS instance register area
 ** \param [in]  pcData        String destination
 ** \param [in]  u16MaxChars   Max. number of chars to read in
 ** \param [in]  cTerminator   EOL terminator
 ** \param [in]  bEcho         TRUE Echo input characters
 **
 ** \retval > 0                       Number of read characters
 ** \retval = 0                       Parameter u16MaxChars = 0 or no data
 **                                    (except terminator) entered
 ** \retval -1                        If one of following conditions are met:
 **         - pstcMfs == NULL
 **         - pcData  == NULL
 **         - pstcMfsInternData == NULL
 **                (invalid or disabled MFS unit (L3_PERIPHERAL_ENABLE_MFS))
 ******************************************************************************/
int16_t Mfs_ReadString( stc_mfsn_t* pstcMfs, 
                          char_t* pcData,
                          uint16_t u16MaxChars,
                          char_t cTerminator,
                          boolean_t  bEcho
                        )
 {
    stc_mfs_intern_data_t* pstcMfsInternData ; 
   
    char_t  cInputChar = '\0'; // Must be preset to something else
    int16_t i16CharsRead = 0;

    // Check for non-NULL pointer
    if ( pstcMfs == NULL ||
         pcData  == NULL
       )
    {
        return -1 ;
    }

    if ( u16MaxChars == 0 )
    {
        return 0 ;
    }
    
    // Get pointer to internal data structure ...
    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    // .. and check for null pointer
    if ( pstcMfsInternData == NULL )
    {
        return ErrorInvalidParameter ;
    }
    
    // Disable reception FIFO by setting trigger level to 1
    if (pstcMfsInternData->enFifoAvailable == MfsHasFifo)
    { 
      pstcMfs->FBYTE2 = 1;
    }
    
    // Wait for termination character
    for ( ;; )
    {
        cInputChar = Mfs_ReadChar( pstcMfs, TRUE );
        if ( cInputChar == cTerminator )
        {
            *pcData = '\0' ;
            break ;
        }
        *pcData++ = cInputChar ;    
        i16CharsRead++ ;
      
        if (bEcho)
        {
          // Echo
          Mfs_Write( pstcMfs, &cInputChar, 1, FALSE );
        }
        
        if ( i16CharsRead >= u16MaxChars )
        {
            break ;
        }
    }
    
    // Re-enable reception FIFO maximum trigger level
    if (pstcMfsInternData->enFifoAvailable == MfsHasFifo)
    { 
      pstcMfs->FBYTE2 = MFS_FIFO_MAX_VAL;
    }
    
    return i16CharsRead ;
} // Mfs_ReadString

/**
 ******************************************************************************
 ** \brief MFS wait for enter
 ** Wait for Enter (either CR or LF)
 **
 ** \param [in]  pstcMfs      Pointer to MFS instance register area
 **
 ******************************************************************************/
void Mfs_WaitEnter( stc_mfsn_t* pstcMfs ) 
{
    char_t cStartTest = '\0' ; // Must be preset to something else
    
    // Check for non-NULL pointer
    if( pstcMfs == NULL )
    {
        return ;
    }
    
    // Wait for either CR or LF
    while ( cStartTest != '\n' &&
            cStartTest != '\r'
          )
    {
        cStartTest = Mfs_ReadChar( pstcMfs, TRUE ) ;
    }
    Mfs_PurgeRx( pstcMfs ) ; // In case a terminal sends CR/LF on Enter
} // Mfs_WaitEnter

/**
 ******************************************************************************
 ** \brief Simple initialization for specific MFS to provide quick start
 **          for new users
 **
 ** \param [in]  pstcMfs      Pointer to MFS instance register area
 ** \param [in]  u32DataRate    Baudrate to setup
 ** \param [in]  enParity       Parity mode
 ** \param [in]  enStartStopBit Start-/Stop-bit config
 ** \param [in]  pfnTxCallback  Transmit-Event Callback function pointer for 
 ** \param [in]  pfnRxCallback  Receive-Event Callback function pointer for 
 **
 ** \retval Ok                        Setup succeeded
 ** \retval ErrorInvalidParameter     If one of following conditions are met:
 **         - pstcMfs == NULL
 **         - pstcMfsInternData == NULL 
 **                (invalid or disabled MFS unit (L3_PERIPHERAL_ENABLE_MFS))  
 ******************************************************************************/
en_result_t Mfs_AsyncInit( stc_mfsn_t*   pstcMfs,
                             uint32_t        u32DataRate,
                             en_mfs_parity_t enParity,
                             en_mfs_start_stopbit_t enStartStopBit,
                             mfs_tx_cb_func_ptr_t pfnTxCallback,
                             mfs_rx_cb_func_ptr_t pfnRxCallback
                          )
{    
    stc_mfs_config_t stcConfig ;  
  
    stcConfig.enMode         = MfsAsynchronous0;
    stcConfig.enClockSource  = MfsUseInternalBaudRateGenerator;
    
    stcConfig.bBitDirection  = 0;   // LSB first
    stcConfig.u32DataRate    = u32DataRate;
    stcConfig.enParity       = enParity;
    stcConfig.enStartStopBit = enStartStopBit;
    stcConfig.u8CharLength   = MfsEightBits;

    // Check for NULL Pointer
    if ( pstcMfs == NULL )
    {
        return ErrorInvalidParameter ;
    }

    // Check if we'd like to (re-)init the same channel
    if ( pstcMfsComChannel != NULL &&
         pstcMfsComChannel != pstcMfs
       )
    {
        return Error ;
    }
    pstcMfsComChannel = (stc_mfsn_t*) pstcMfs ; // Backup and mark it in use
    
    stcConfig.bSyncClockInversion          = FALSE;
    stcConfig.bSyncClockOutputDelay        = FALSE;
    stcConfig.pcTxBuffer                   = cTxBuffer;
    stcConfig.u16TxBufferSize              = MFS_BUFFER_LENGTH;
    stcConfig.pcRxBuffer                   = cRxBuffer;
    stcConfig.u16RxBufferSize              = MFS_BUFFER_LENGTH;
    stcConfig.enTxCallbackMode             = MfsOnTxBufferEmpty;
    stcConfig.pfnTxCallback                = pfnTxCallback;
    stcConfig.u16RxCallbackBufferFillLevel = 1;
    stcConfig.pfnRxCallback                = pfnRxCallback;
    
    return Mfs_Init( pstcMfs, &stcConfig ) ;
} // Mfs_AsyncInit

/**
 ******************************************************************************
 ** \brief Initialisation of a MFS module.
 ** This Function initialises the  MFS according the Options setup in the
 ** passed Config Struct. Several Checkings are done before that and an error
 ** is returned if invalid Modes are requested.
 **
 ** These rules are checked:
 ** - A startbit must be used in asynchronous mode.
 ** - External clock src as it is must be used in synchronous slave mode.
 ** - Pointer to receive and transmit buffer must be valid.
 **
 ** The required timing settings for data rate are calculated automatically 
 ** with respect to the current peripheral clock #0
 **
 ** All neccessary parameter to configure a MFS channel can be set with the 
 ** Mfs_Init#pstcConfig parameter, all other interfaces (Mfs_Read(), 
 ** Mfs_Write(), Mfs_Purge() and Mfs_SynchronousTransfer()) will only 
 ** operate with data to be sent or received over the configured MFS channel.
 **
 ** \pre Peripheral Clock 0 must be configured before
 **      to correctly set data rate.
 **      For MFS asynchronous mode the peripheral clock must be at least 
 **      5 times faster than the data rate to ensure proper oversampling.
 ** \pre The Application must configure corresponding MFS pins (SIN, SOT, SCK)
 **       according to requirements and settings of MFS instance!
 **
 ** \param [in]  pstcMfs     Pointer to MFS instance register area
 ** \param [in]  pstcConfig    MFS module configuration 
 **                            (stc_mfs_config_t#pfnTxCallback, 
 **                             stc_mfs_config_t#pfnRxCallback and 
 **                             stc_mfs_config_t#pfnErrCallback can be NULL).
 **
 ** \retval Ok                    Initializiation of MFS module successfully 
 **                               done.
 ** \retval ErrorInvalidParameter If one of following conditions are met:
 **         - pstcMfs == NULL
 **         - pstcConfig == NULL
 **         - (pstcConfig->enMode == MfsAsynchronous0) && (pstcConfig->enStartStopBit == MfsNoStartNoStop)
 **            i.E. MFS asynchronous mode and no stop bit configured
 **         - (pstcConfig->enMode == MfsSynchronousSlave2) && (pstcConfig->enClockSource != MfsUseExternalSerialClockSourceAsItIs)
 **            i.E. MFS synchronous slave mode and no external clock source configured
 **         - u32ReloadValue == 0 (u32ReloadValue is calculated with: (SysCtrl_GetPeripheralClockFrequencyHz(SysCtrlPeripheral0Clock) / pstcConfig->u32DataRate) - 1)  
 **            i.E. data rate invalid (calculated timing with current peripheral clock 0 is out of range)
 **         - pstcConfig->pcTxBuffer == NULL
 **         - pstcConfig->pcRxBuffer == NULL
 **         - pstcMfsInternData == NULL (pstcMfs is an invalid address)   
 **            i.E. invalid or disabled MFS unit (L3_PERIPHERAL_ENABLE_MFS)
 **
 ** \todo I2C slave mode not implemented yet.
 ******************************************************************************/
en_result_t Mfs_Init( stc_mfsn_t* pstcMfs, 
                      const stc_mfs_config_t* pstcConfig
                    )
{
    stc_mfs_intern_data_t* pstcMfsInternData;
    uint32_t u32ReloadValue;
    
    // Preset local register variables to zero
    stc_mfsn_smr_field_t  unSMR  = { 0 } ;
    stc_mfsn_scr_field_t  unSCR  = { 0 } ;
    stc_mfsn_escr_field_t unESCR = { 0 } ;
    stc_mfsn_ssr_field_t  unSSR  = { 0 } ; 
    stc_mfsn_fcr1_field_t unFCR1 = { 0 } ;
    
    // I2C additional registers
    stc_mfsn_i2c_smr_field_t  unSMR_I2C = { 0 } ;
    stc_mfsn_i2c_ibcr_field_t unIBCR    = { 0 } ;
    stc_mfsn_i2c_ibsr_field_t unIBSR    = { 0 } ;
    stc_mfsn_i2c_ssr_field_t  unSSR_I2C = { 0 } ;
    stc_mfsn_i2c_isba_field_t unISBA    = { 0 } ;
    stc_mfsn_i2c_ismk_field_t unISMK    = { 0 } ; 
      
    // Check for valid pointers
    if ( (pstcMfs == NULL) ||
         (pstcConfig == NULL)
       )
    {
        return ErrorInvalidParameter;
    }
    
    // Check for Options base on the requested operating mode
    switch ( pstcConfig->enMode )
    {
        case MfsAsynchronous0:
        case MfsAsynchronousMulti1:
        case MfsI2cMaster:   
        case MfsI2cSlave:
        default:
            break ;
    }
    
    // RX Callback Level must be set somewhat ge zero (except I2C mode)
    if ((pstcConfig->u16RxCallbackBufferFillLevel < 1) &&
        (pstcConfig->enMode != MfsI2cMaster) &&
        (pstcConfig->enMode != MfsI2cSlave) 
       )
    {
        return ErrorInvalidParameter;
    }
    
    // Get ptr to internal data struct ...
    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    // ... and check
    if ( pstcMfsInternData == NULL )
    {
        return ErrorInvalidParameter;
    }
  
    // Get Clock Frequency for PCLK2
    u32ReloadValue = (FM3_CRG->APBC2_PSR & 0x03);
    switch (u32ReloadValue)
    {
      case 0:
        u32ReloadValue = 1;
        break;
      case 1:
        u32ReloadValue = 2;
        break;
      case 2:
        u32ReloadValue = 4;
        break;
      case 3:
        u32ReloadValue = 8;
        break;
      default:
        u32ReloadValue = 1;
    }
    
    // Check if data rate or Clock Frequency is 0 (div by zero not wanted)
    if ( pstcConfig->u32DataRate == 0 ||
         u32ReloadValue == 0
       )
    {
        return ErrorInvalidParameter ;
    }

    u32ReloadValue = __HCLK / u32ReloadValue;
    
    // Calc reload value based on data rate set
    u32ReloadValue /= pstcConfig->u32DataRate ;
    u32ReloadValue -= 1 ;
    
    // For asynchronous communication, the reload value must be greater than 
    // or equal to 4 because five times over-sampling is performed internally
    if ( (u32ReloadValue < 4) && 
         ( (pstcConfig->enMode == MfsAsynchronous0) ||
           (pstcConfig->enMode == MfsAsynchronousMulti1) ||
           (pstcConfig->enMode == MfsI2cMaster) ||  
           (pstcConfig->enMode == MfsI2cSlave)
         )
       )
    {
        return ErrorInvalidParameter;
    }
  
    // First of all set MFS to Asynchronous mode 0
    pstcMfs->SMR = 0 ;
    
    // Then we disable TX and RX for safe operation
    pstcMfs->SCR = 0 ;
    
    // Clear MFS by setting the Software Reset bit
    pstcMfs->u8SCR.UPCL = 1;
    
    // Check for FIFO availability
    if (pstcMfs == &MFS0)
    {
      pstcMfsInternData->enFifoAvailable = MFS0_FIFO;
    }
    else if (pstcMfs == &MFS1)
    {
      pstcMfsInternData->enFifoAvailable = MFS1_FIFO;
    }
    else if (pstcMfs == &MFS2)
    {
      pstcMfsInternData->enFifoAvailable = MFS2_FIFO;
    }
    else if (pstcMfs == &MFS3)
    {
      pstcMfsInternData->enFifoAvailable = MFS3_FIFO;
    }
    else if (pstcMfs == &MFS4)
    {
      pstcMfsInternData->enFifoAvailable = MFS4_FIFO;
    }
    else if (pstcMfs == &MFS5)
    {
      pstcMfsInternData->enFifoAvailable = MFS5_FIFO;
    }
    else if (pstcMfs == &MFS6)
    {
      pstcMfsInternData->enFifoAvailable = MFS6_FIFO;
    }
    else if (pstcMfs == &MFS7)
    {
      pstcMfsInternData->enFifoAvailable = MFS7_FIFO;
    }
    
    // ************ Register Settings ************
    //
    // Local Serial Mode Register variable
    //

    // Operation Mode Selection
    switch ( pstcConfig->enMode )
    {
        case MfsAsynchronous0:
            unSMR.MD0 = 0 ; 
            unSMR.MD1 = 0 ; 
            unSMR.MD2 = 0 ; 
            break ;
        case MfsAsynchronousMulti1:
            unSMR.MD0 = 1 ; 
            unSMR.MD1 = 0 ; 
            unSMR.MD2 = 0 ; 
            break ;
        case MfsSynchronousSlave2:
        case MfsSynchronousMaster2:
            unSMR.MD0 = 0 ; 
            unSMR.MD1 = 1 ; 
            unSMR.MD2 = 0 ; 
            break ;
        case MfsI2cMaster:
        case MfsI2cSlave:
            unSMR_I2C.MD0 = 0 ; 
            unSMR_I2C.MD1 = 0 ; 
            unSMR_I2C.MD2 = 1 ; 
            break;
        default:
            return ErrorInvalidParameter ; // should never see the daylight
    }
   
    if ((pstcConfig->enMode != MfsI2cMaster) &&
        (pstcConfig->enMode != MfsI2cSlave))
    {
      // Bit direction
      unSMR.BDS = pstcConfig->bBitDirection == TRUE ? 1 : 0 ; 
    }
    
    // now setup hardware with correct mode first and then go on with
    //   bit settings
    pstcMfs->u8SMR = unSMR;
  
    if ((pstcConfig->enMode == MfsSynchronousMaster2) ||
        (pstcConfig->enMode == MfsSynchronousSlave2))
    {    
      // Wakup function not supported yet
      unSMR.WUCR = 0;
      
      // Set synchronous clock input/output according master/slave mode
      if ( pstcConfig->enMode == MfsSynchronousMaster2 )
      {
        unSMR.SCKE = 1; // Master: Clock output
      }
      else
      {
        unSMR.SCKE = 0; // Slave: Clock input
      }
      
      unSMR.SOE = 1;                     // Enable SOT
      
      // now setup hardware (synchronous)
      pstcMfs->u8SMR = unSMR;
    }
    else if ((pstcConfig->enMode == MfsI2cMaster) ||
             (pstcConfig->enMode == MfsI2cSlave))
    {
      // now setup hardware (I2C)
      pstcMfs->u8SMR_I2C = unSMR_I2C;
    }
    else  // asynchronous mode(s)
    {
      // Stop bit length 1
      switch ( pstcConfig->enStartStopBit )
      {
        case MfsOneStartOneStop:
          unSMR.SBL_SCINV = 0;
          break;
        case MfsOneStartTwoStop:
          unSMR.SBL_SCINV = 1;
          break;     
        case MfsOneStartThreeStop:
          unSMR.SBL_SCINV = 0;
          break;
        case MfsOneStartFourStop:
          unSMR.SBL_SCINV = 1;
          break;
        case MfsNoStartNoStop:
          unSMR.SBL_SCINV = 0;
          break;
        default:
          return ErrorInvalidParameter ;
      }
      
      unSMR.SOE = 1;                     // Enable SOT
      
      // now setup hardware (asynchronous)
      pstcMfs->u8SMR = unSMR;
    }
    
    //
    // Local Serial/I2C Control Register variable
    //
    if ((pstcConfig->enMode == MfsSynchronousMaster2) ||
        (pstcConfig->enMode == MfsSynchronousSlave2))
    { 
      // Serial Clock Delay Enable (SPI)
      unSCR.SPI_LBR = pstcConfig->bSyncClockOutputDelay ? 1 : 0 ;
  
      // Master/Slave Mode Select (0-Master, 1-Slave)
      // Note: If Slave Mode is selected, direct external clock source must be
      //       setup (SMR.SCKE = '0')
      unSCR.MS  = pstcConfig->enMode == MfsSynchronousSlave2 ? 1 : 0 ;
      
      // now setup hardware
      pstcMfs->u8SCR = unSCR;
    }
    else if ((pstcConfig->enMode == MfsI2cMaster) ||
             (pstcConfig->enMode == MfsI2cSlave))
    {
       // now setup hardware
      pstcMfs->u8IBCR = unIBCR;
    }
    else // Asynchronous
    {
      // now setup hardware
      pstcMfs->u8SCR = unSCR;
    }

    //
    // Local Extended Serial Control Register variable
    //
    if ((pstcConfig->enMode != MfsI2cMaster) &&
        (pstcConfig->enMode != MfsI2cSlave))
    {
      if ((pstcConfig->enMode == MfsSynchronousMaster2) ||
          (pstcConfig->enMode == MfsSynchronousSlave2))
      {
        // Wait time insertion
        switch ( pstcConfig->u8SyncClockWaitTime )
        {
        case MfsSyncWaitZero:
            unESCR.P_WT0_LBL1   = 0;
            unESCR.PEN_WT1_LBIE = 0;
            break;
          case MfsSyncWaitOne:
            unESCR.P_WT0_LBL1   = 1;
            unESCR.PEN_WT1_LBIE = 0;
            break;
          case MfsSyncWaitTwo:
            unESCR.P_WT0_LBL1   = 0;
            unESCR.PEN_WT1_LBIE = 1;
            break;
          case MfsSyncWaitThree:
            unESCR.P_WT0_LBL1   = 1;
            unESCR.PEN_WT1_LBIE = 1;
            break;
          default:
            return ErrorInvalidParameter ; // other values not allowed
        }
      } 
      else // if "mode 2" ...
      {
        // Setup Parity 1 (Only in Asynchronous mode 0 available)
        if (pstcConfig->enMode == MfsAsynchronous0)
        {
          switch ( pstcConfig->enParity )
          {
              case MfsParityNone:
                  unESCR.P_WT0_LBL1 = 0 ; // Parity type selection (0-even/1-odd)
                  break ;
              case MfsParityEven:
                  unESCR.P_WT0_LBL1 = 0 ; // Parity type selection (0-even/1-odd)
                  break ;
              case MfsParityOdd:
                  unESCR.P_WT0_LBL1 = 1 ; // Parity type selection (0-even/1-odd)
                  break ;
              default:
                  return ErrorInvalidParameter ; // should never see the daylight
          }  
        } // if (pstcConfig->enMode == MfsAsynchronous0)
        
        // Stop bit length 2 (not available in Synchronous mode 2)
        if ((pstcConfig->enMode != MfsSynchronousMaster2) &&
            (pstcConfig->enMode != MfsSynchronousMaster2))
        {
          switch ( pstcConfig->enStartStopBit )
          {
            case MfsOneStartOneStop:
              unESCR.ESBL = 0;
              break;
            case MfsOneStartTwoStop:
              unESCR.ESBL = 0;
              break;     
            case MfsOneStartThreeStop:
              unESCR.ESBL = 1;
              break;
            case MfsOneStartFourStop:
              unESCR.ESBL = 1;
              break;
            case MfsNoStartNoStop:
              unESCR.ESBL = 0;
              break;
            default:
              return ErrorInvalidParameter ;
          }
        }
      } // [if ((pstcConfig->enMode == MfsSynchronousMaster2) ...] else
      
      // Hardware flow not supported yet
      unESCR.FLWEN_SOP = 0;
      
      // NRZ / inverted NRZI
      unESCR.INV = pstcConfig->bSignalSystem == TRUE  ? 1 : 0 ; 
      
      // Caracter Length
      switch ( pstcConfig->u8CharLength )
      {
        case MfsFifeBits:
          unESCR.L0_DEL0 = 1;
          unESCR.L1_DEL1 = 0;
          unESCR.L2_LBL0 = 0;
          break;
        case MfsSixBits:
          unESCR.L0_DEL0 = 0;
          unESCR.L1_DEL1 = 1;
          unESCR.L2_LBL0 = 0;
          break;
        case MfsSevenBits:
          unESCR.L0_DEL0 = 1;
          unESCR.L1_DEL1 = 1;
          unESCR.L2_LBL0 = 0;
          break;   
        case MfsEightBits:
          unESCR.L0_DEL0 = 0;
          unESCR.L1_DEL1 = 0;
          unESCR.L2_LBL0 = 0;
          break;
        case MfsNineBits:
          unESCR.L0_DEL0 = 0;
          unESCR.L1_DEL1 = 0;
          unESCR.L2_LBL0 = 1;
          break;
        default:
          return ErrorInvalidParameter ; // should never see the daylight
      }
      
      // now setup hardware
      pstcMfs->u8ESCR = unESCR;
    }
    //
    // Local I2C Status Register
    //
    else  // if "not I2C mode"
    {
      // now setup hardware
      pstcMfs->u8IBSR = unIBSR;
    }

    //
    // Local Serial Status Register variable (USART)
    //    
    if ((pstcConfig->enMode != MfsI2cMaster) && 
        (pstcConfig->enMode != MfsI2cSlave))
    {
      // Clear possible reception errors
      pstcMfs->u8SSR.REC = 0;
  
      // Setup Parity 2 (Only in Asynchronous mode 0 available)
      if (pstcConfig->enMode == MfsAsynchronous0)
      {
        switch ( pstcConfig->enParity )
        {
            case MfsParityNone:
                unSSR.PE_LBD  = 0 ; // Parity disable
                break ;
            case MfsParityEven:
                unSSR.PE_LBD  = 1 ; // Parity enable
                break ;
            case MfsParityOdd:
                unSSR.PE_LBD  = 1 ; // Parity enable
                break ;
            default:
                return ErrorInvalidParameter ;
        }   
      }
      // now setup hardware
      pstcMfs->u8SSR = unSSR;
    } 
    //
    // Local Serial Status Register variable (I2C)
    //
    else // if "not I2C mode"
    {
      // now setup hardware
      pstcMfs->u8SSR_I2C = unSSR_I2C;    
      
      //
      // 7-Bit Slave Address Register
      //
      // now setup hardware
      pstcMfs->u8ISBA = unISBA;    
      
      //
      // 7-Bit Slave Mask Register
      //
      // now setup hardware
      pstcMfs->u8ISMK = unISMK;  
    }
    
    //
    //  Local Fifo Control Register 0 variable
    //
    if (pstcMfsInternData->enFifoAvailable == MfsHasFifo)
    {
      stc_mfsn_fcr0_field_t unFCR0 = { 0 } ; // Preset to zero
      
      unFCR0.FLD = 0;   // No reload (for transmission)  
      unFCR0.FSET = 0;  // FIFO2 read pointer not saved   
      unFCR0.FCL2 = 1;  // Reset FIFO2
      unFCR0.FCL1 = 1;  // Reset FIFO1
      unFCR0.FE2 = 1;   // Enable FIFO2
      unFCR0.FE1 = 1;   // Enable FIFO1
      
      // Setup hardware
      pstcMfs->u8FCR0 = unFCR0;
      
      //
      //  Local Fifo Control Register 1 variable
      //
      
      // No FIFO test (also not used for I2C)
      unFCR1.FTST0 = 0;  
      unFCR1.FTST1 = 0;  
      
      unFCR1.FLSTE = 0; // disable data lost detection  
      unFCR1.FRIIE = 0; // disable FIFO idle detection
      unFCR1.FDRQ  = 0; // no request for transmission FIFO data
      unFCR1.FTIE  = 0; // disable transmission FIFO interrupt
      unFCR1.FSEL  = 0; // FIFO1: transmission FIFO, FIFO2: reception FIFO
  
      // Setup hardware
      pstcMfs->u8FCR1 = unFCR1; 
      
      // Set maximum RX FIFO trigger (if not I2C mode)
      if ((pstcConfig->enMode != MfsI2cMaster) &&
          (pstcConfig->enMode != MfsI2cSlave))
      {      
        pstcMfs->FBYTE2 = MFS_FIFO_MAX_VAL;
      }
    }
    
    // Save RX Callback Buffer Fill Level (needed during servicing RX INT)
    pstcMfsInternData->u16RxCallbackBufferFillLevel = pstcConfig->u16RxCallbackBufferFillLevel ;

    // Set clock source
    pstcMfs->u16BGR.EXT = (pstcConfig->enClockSource == MfsUseExternalSerialClockSourceWithBaudRateGenerator) ? 1 : 0 ;
    
    // Set baud rate generation reload register
    pstcMfs->BGR = u32ReloadValue & 0x7FFF ;
    
    // Store callback functions.
    pstcMfsInternData->pfnTransmitCbFunction = pstcConfig->pfnTxCallback;
    pstcMfsInternData->pfnReceiveCbFunction  = pstcConfig->pfnRxCallback;
    pstcMfsInternData->pfnErrorCbFunction    = pstcConfig->pfnErrCallback;
    
    pstcMfsInternData->enTxCallbackMode = pstcConfig->enTxCallbackMode;
  
    // Initialise TX ring buffer
    pstcMfsInternData->stcTxBuffer.pcBuffer      = pstcConfig->pcTxBuffer;
    pstcMfsInternData->stcTxBuffer.u16BufferSize = pstcConfig->u16TxBufferSize;
    pstcMfsInternData->stcTxBuffer.u16InIndex    = 0 ; //  Reset Index
    pstcMfsInternData->stcTxBuffer.u16OutIndex   = 0 ; //  Reset Index
    pstcMfsInternData->stcTxBuffer.u16FillCount  = 0 ; //  Reset Counter
    pstcMfsInternData->stcTxBuffer.bOverflow     = 0 ; //  Reset Flag
    
    // Initialise RX ring buffer
    pstcMfsInternData->stcRxBuffer.pcBuffer      = pstcConfig->pcRxBuffer;
    pstcMfsInternData->stcRxBuffer.u16BufferSize = pstcConfig->u16RxBufferSize;
    pstcMfsInternData->stcRxBuffer.u16InIndex    = 0 ; //  Reset Index
    pstcMfsInternData->stcRxBuffer.u16OutIndex   = 0 ; //  Reset Index
    pstcMfsInternData->stcRxBuffer.u16FillCount  = 0 ; //  Reset Counter
    pstcMfsInternData->stcRxBuffer.bOverflow     = 0 ; //  Reset Flag
    
    pstcMfsInternData->bSubstLFwithCRLF          = pstcConfig->bSubstLFwithCRLF ;

    //
    // Serial Control Set Register
    //
    // Enable receiver, transmitter (USART mode)
    if ((pstcConfig->enMode != MfsI2cMaster) &&
        (pstcConfig->enMode != MfsI2cSlave))
    {
      pstcMfs->u8SCR.TXE = 1 ;
      pstcMfs->u8SCR.RXE = 1 ;
      
      // Init interrupts
      Mfs_InitIrq(pstcMfs);
    }
    
    return Ok;
} // Mfs_Init

/**
 ******************************************************************************
 ** \brief Deinitialisation of a MFS module.
 **
 ** All used mfs register are reset to their default values.
 **  
 ** \param [in]   pstcMfs       Pointer to MFS instance register area
 ** 
 ** \retval Ok                    Deinitialisation of MFS module successfully
 **                               done
 ** \retval ErrorInvalidParameter If one of following conditions are met:
 **             - pstcMfs == NULL
 **             - pstcMfsInternData == NULL (invalid or disabled MFS unit
 **              (L3_PERIPHERAL_ENABLE_MFS)) 
 ******************************************************************************/
en_result_t Mfs_DeInit( stc_mfsn_t* pstcMfs )
{
    stc_mfs_intern_data_t* pstcMfsInternData;

    // Check for NULL ptr
    if (pstcMfs == NULL)
    {
      return ErrorInvalidParameter;
    }

    // Get ptr to internal data struct ...
    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    // ... and check
    if (pstcMfsInternData == NULL)
    {
      return ErrorInvalidParameter ;
    }

    // First of all disable receiver, transmitter and deinit interrupts
    if ((pstcMfs->SMR & 0xE0) != 0x80)  // not I2C mode?
    {
      pstcMfs->u8SCR.TXE = 0 ;
      pstcMfs->u8SCR.RXE = 0 ;
  
      Mfs_DeInitIrq(pstcMfs);     // returns always en_result_t Ok
    }
    else
    {
      pstcMfs->u8ISMK.EN = 0 ;    // I2C disable ...
      pstcMfs->ISMK = 0 ;         //  ... and again with clearing mask bits
      pstcMfs->ISBA = 0 ;
    }

    // Baud Rate Generation Reload Reset
    pstcMfs->BGR = 0;
        
    // Serial Mode Register clear all bits (valid in any mode)
    pstcMfs->SMR = 0 ;
       
    // Reset Mfs receive and transmit bit to default
    // and clear all error flags
    pstcMfs->SCR = 0;

    // Clear reception Errors    
    pstcMfs->u8SSR.REC = 1 ;
       
    // Reset all other used register to default value
    pstcMfs->SSR   = 0 ;
    pstcMfs->ESCR  = 0 ;
    pstcMfs->FCR0  = 0 ;
    pstcMfs->FCR1  = 0 ;

    // Clear MFS by setting the Software Reset bit
    pstcMfs->u8SCR.UPCL = 1 ;
        
    // Reset Fifo buffer
    pstcMfsInternData->stcTxBuffer.pcBuffer  = NULL ;
    pstcMfsInternData->stcRxBuffer.pcBuffer  = NULL ;
  
    // Reset callback functions.
    pstcMfsInternData->pfnTransmitCbFunction = NULL ;
    pstcMfsInternData->pfnReceiveCbFunction  = NULL ;
    pstcMfsInternData->pfnErrorCbFunction    = NULL ;

    // Reset LIN Break Callback used for Mfs_Lin_DeInit()
    pstcMfsInternData->pfnLinBreakDetectionCbFunction = NULL;
    
    if ( pstcMfsComChannel == pstcMfs )
    {
        pstcMfsComChannel = NULL ; // Release async channel
    }
    
    return Ok ;
} // Mfs_DeInit

/**
 ******************************************************************************
 ** \brief Generate an I2C start condition
 **
 ** \note This function must only be called, if the recent Mfs instance was
 **       initialized to I2C mode! Otherwise the behavior of this function
 **       is undefined!
 **
 ** \pre Mfs_Init() must be called before!
 **
 ** \param [in] pstcI2c         Pointer to I2C (Mfs) instance register area
 ** \param [in] u8DestAddress   address of external device (7 bit, bit0 aligned)
 ** \param [in] enAccess        access type (I2CRead or I2CWrite)
 ** 
 ** \retval Ok                     Start condition was successful
 ** \retval Error                  A bus error, arbitration loss or no
 **                                acknowledge was detected
 ** \retval ErrorTimeout           A timeout error occurred
 ** \retval ErrorInvalidParameter  pstcI2c == NULL
 ******************************************************************************/
en_result_t Mfs_I2C_Start( volatile stc_mfsn_t* pstcI2c,
                           uint8_t  u8DestAddress,
                           en_mfs_i2c_datadirection_t enAccess
                         )
{
  uint16_t u16ErrorCount = 0;
  uint16_t u16ErrorCountPollInt;

  // Check for NULL ptr
  if (pstcI2c == NULL)
  {
    return ErrorInvalidParameter;
  }
  
  pstcI2c->ISBA = 0x00;
  pstcI2c->u8ISMK.EN = 1;
  
  pstcI2c->TDR  = (u8DestAddress << 1) | enAccess; // Send data with R/W bit
  pstcI2c->u8IBCR.MSS = 1;          // Master Mode  
  
  do
  {
    u16ErrorCountPollInt = 0;
    
    while(!(pstcI2c->u8IBCR.INT))  // Wait for transmission complete
    {
      L3_WAIT_LOOP_HOOK();
      
      u16ErrorCountPollInt++;
      if (u16ErrorCountPollInt > I2C_TIME_OUT_POLLING_TRIALS_INT)
      {
        return ErrorTimeout;
      }
    }
    
    if (pstcI2c->u8IBCR.BER)
    {
      return Error;
    }
    
    if (!(pstcI2c->u8IBCR.ACT_SCC))   // ACT == 0? ->  error
    {
      return Error;
    }
    
    // MSS is set ...
    if (pstcI2c->u8IBSR.RACK)  // busy or error?
    {
      pstcI2c->IBCR = 0xE0;  // MMS = CSS = ACKE = 1: Try restart (if busy)
      u16ErrorCount++;
      
      if (u16ErrorCount > I2C_TIME_OUT_POLLING_TRIALS)
      {
        return ErrorTimeout;        // too much retrials
      }
    }
    else
    {
      u16ErrorCount = 0; 
    }
    
    L3_WAIT_LOOP_HOOK();
  }while (u16ErrorCount);

  if (!(pstcI2c->u8IBSR.TRX))   // ->  error
  {
     return Error;
  }
  
  return Ok;
} // Mfs_I2C_Start

/**
 *****************************************************************************
 ** Generate a continued (repeated) start condition (after a normal start 
 ** condition is already created).
 **
 ** \note This function must only be called, if the recent Mfs instance was
 **       initialized to I2C mode! Otherwise the behavior of this function
 **       is undefined!
 **
 ** \pre Mfs_Init() and Mfs_I2C_Start() must be called before!
 **
 ** \param [in] pstcI2c         Pointer to I2C instance register area
 ** \param [in] u8DestAddress   address of external device (7-bit, bit0 aligned)
 ** \param [in] enAccess        access type (I2CRead or I2CWrite)
 **
 ** \retval Ok                     Continued start condition was successful
 ** \retval Error                  An arbitration loss or no acknowledge was
 **                                detected
 ** \retval ErrorInvalidParameter  pstcI2c == NULL or enAccess wrong type
*****************************************************************************/
en_result_t Mfs_I2C_Continue( volatile stc_mfsn_t* pstcI2c,
                              uint8_t  u8DestAddress,
                              en_mfs_i2c_datadirection_t enAccess
                            )
{
  uint8_t  u8Data;
  uint16_t u16ErrorCountPollInt;
  
  // Check for NULL ptr
  if (pstcI2c == NULL)
  {
    return ErrorInvalidParameter;
  }

  u8Data = (u8DestAddress << 1) | enAccess;
  
  pstcI2c->TDR  = u8Data; // Send data to EEPROM
  pstcI2c->IBCR = 0xE0;   // Set continuous mode MSS = SCC = ACKE = 1

  u16ErrorCountPollInt = 0;
  while(!(pstcI2c->u8IBCR .INT)) // Wait for transmission complete
  {
    L3_WAIT_LOOP_HOOK();
    
    u16ErrorCountPollInt++;
    if (u16ErrorCountPollInt > I2C_TIME_OUT_POLLING_TRIALS_INT)
    {
      return ErrorTimeout;
    }
  }
                                       
  if (pstcI2c->u8IBCR.BER)
  {
    return Error;
  }
  
  if (!(pstcI2c->u8IBCR.ACT_SCC))   // ACT == 0? ->  error
  {
    return Error;
  }
  
  // MSS is set, no reserved address
  if (pstcI2c->u8IBSR.RACK)
  {
    return Error;
  }

  if (enAccess == I2CRead)
  {
    if (pstcI2c->u8IBSR.TRX)
    {    
      return Error;
    }
  }
  else if (enAccess == I2CWrite)
  {
    if (!(pstcI2c->u8IBSR.TRX))
    {
      return Error; 
    }
  }
  else
  {
    return ErrorInvalidParameter;  
  }
  
  return Ok;  
} // Mfs_I2C_Continue

/**
 *****************************************************************************
 ** Generate a stop condition (for write and read communication)
 **
 ** \note This function must only be called, if the recent Mfs instance was
 **       initialized to I2C mode! Otherwise the behavior of this function
 **       is undefined!
 **
 ** \param [in] pstcI2c         Pointer to I2C instance register area
 ** \param [in] enAccess        access type (I2CRead or I2CWrite)
 **
 ** \retval Ok                     Communication stopped
 ** \retval ErrorInvalidParameter  pstcI2c == NULL
 *****************************************************************************/
en_result_t Mfs_I2C_Stop( volatile stc_mfsn_t* pstcI2c,
                          en_mfs_i2c_datadirection_t enAccess )
{
  // Check for NULL ptr
  if (pstcI2c == NULL)
  {
    return ErrorInvalidParameter;
  }
  
  if (enAccess == I2CWrite)
  {
    pstcI2c->IBCR = 0x20;    // MMS = CSS = INT = 0, ACKE = 1
  }
  else if (enAccess == I2CRead)
  {
    pstcI2c->IBCR = 0x01;    // INT = 1, MMS = ACKE = 0
  }
  else
  {
    return ErrorInvalidParameter;
  }
  
  return Ok;
} // Mfs_I2C_Stop

/**
 *****************************************************************************
 ** I2C writes n data to a selected device.
 **
 ** \note This function must only be called, if the recent Mfs instance was
 **       initialized to I2C mode! Otherwise the behavior of this function
 **       is undefined!
 **
 ** \pre Mfs_Init() and Mfs_I2C_Start() must be called before!
 **
 ** \param [in] pstcI2c         Pointer to I2C instance register area
 ** \param [in] pu8Data         Pointer to data to be sent
 ** \param [in] u8DataLength    Number of bytes references by pu8Data
 **
 ** \retval Ok                     Data transmission successful
 ** \retval Error                  An arbitration loss or no acknowledge was
 **                                detected
 ** \retval ErrorTimeout           A timeout error occurred
 ** \retval ErrorInvalidParameter  pstcI2c == NULL
 *****************************************************************************/
en_result_t Mfs_I2C_WriteSync( volatile stc_mfsn_t* pstcI2c,
                               uint8_t* pu8Data,
                               uint8_t  u8DataLength
                             )
{
  uint8_t  u8Count = u8DataLength;
  uint16_t u16ErrorCountPollInt;

  // Check for NULL ptr
  if (pstcI2c == NULL)
  {
    return ErrorInvalidParameter;
  }
  
  while(u8Count)
  {
    pstcI2c->TDR  = *pu8Data++;     // Send data
    pstcI2c->IBCR = 0xB0;           // WSEL = 1, ACKE = 1, Clear INT flag
    
    u16ErrorCountPollInt = 0;
    while(!(pstcI2c->u8IBCR.INT))  // Wait for transmission complete
    {
      L3_WAIT_LOOP_HOOK();
      
      if (u16ErrorCountPollInt > I2C_TIME_OUT_POLLING_TRIALS_INT)
      {
        return ErrorTimeout;
      }
    }
     
    if (pstcI2c->u8IBCR.BER)
    {
      return Error;
    }
    
    if (!(pstcI2c->u8IBCR.ACT_SCC))   // ACT == 0? ->  error
    {
      return Error;
    }
    
    // MSS is set, no reserved address
    if (pstcI2c->u8IBSR.RACK)
    {
      return Error;
    }
    
    if (!(pstcI2c->u8IBSR.TRX))   // TRX == 0? ->  error
    {
      return Error;
    }
    
    u8Count--;
  }
  
  return Ok;    
}

/**
 *****************************************************************************
 ** I2C Read data to a selected device.
 **
 ** \note This function must only be called, if the recent Mfs instance was
 **       initialized to I2C mode! Otherwise the behavior of this function
 **       is undefined!
 **
 ** \pre I2C_Init() and I2C_Start()/I2C_Continue() must be called before!
 **
 ** \param [in] pstcI2c         Pointer to I2C instance register area
 ** \param [in] pu8Data         Pointer to data buffer for receive read data
 ** \param [in] u8DataLength    Number of bytes to read
 **
 ** \retval Ok                     Data reception successful
 ** \retval Error                  An arbitration loss or no acknowledge was
 **                                detected
 ** \retval ErrorTimeout           A timeout error occurred
 ** \retval ErrorInvalidParameter  pstcI2c == NULL
 *****************************************************************************/
en_result_t Mfs_I2C_ReadSync( volatile stc_mfsn_t* pstcI2c,
                              uint8_t* pu8Data,
                              uint8_t  u8DataLength
                            )
{
  uint16_t u16ErrorCount;
  uint16_t u16ErrorCountPollInt;
  uint8_t  u8ReceiveData;
  uint8_t  u8Count = u8DataLength;

  // Check for NULL ptr
  if (pstcI2c == NULL)
  {
    return ErrorInvalidParameter;
  }
  
  while(u8Count)
  {
    do
    {
      // Finished read?
      if ((!(pstcI2c->u8IBSR.FBT))  
          && (pstcI2c->u8SSR.RDRF))
      {
        u8ReceiveData = pstcI2c->RDR;
        u16ErrorCount = 0;
      }
      else
      {
        u16ErrorCount++;
        if (u16ErrorCount > I2C_TIME_OUT_POLLING_TRIALS) // too much retrails
        {
          return ErrorTimeout; 
        }
        
        pstcI2c->IBCR = 0xA0;   // MSS = 1, ACKE = 1, Clear INT flag (V1.1: WSEL removed)
        
        u16ErrorCountPollInt = 0;
        while(!(pstcI2c->u8IBCR.INT))  // Wait for reception complete
        {
          L3_WAIT_LOOP_HOOK();
          u16ErrorCountPollInt++;
          
          if (u16ErrorCountPollInt > I2C_TIME_OUT_POLLING_TRIALS_INT)
          {
            return ErrorTimeout;
          }
        }
        
        if (u8DataLength > 1)   // Bug fix V1.1
        {
           pstcI2c->TDR = 0;    // Send dummy byte to create clock signal
        }
        
        if (pstcI2c->u8IBCR.BER)
        {
          return Error;
        }
        
        if (!(pstcI2c->u8IBCR.ACT_SCC))    // ACT == 0? ->  error
        {
          return Error;
        }
        
        // MSS is set, no reserved address
        if (pstcI2c->u8IBSR.RACK)
        {
          return Error;
        }
        
        if (pstcI2c->u8IBSR.TRX)       // TRX == 1? ->  error
        {
          return Error;
        }
      }
    }while(u16ErrorCount);  
    
    *pu8Data++ = u8ReceiveData;
    
    u8Count--;
  } // while(u8Count)
  
  return Ok;
} // Mfs_I2C_ReadSync

/**
 ******************************************************************************
 ** \brief Set I2C Auxiliary Noise Filter
 ** 
 ** \param [in]  pstcMfsDnf        Pointer to MFS Noise Filter structure
 **
 ** \retval Ok                     Noise filter settings updated
 ** \retval ErrorInvalidParameter  pstcMfsDnf == NULL or invalid enumerator in
 **                                configuration
 ******************************************************************************/
en_result_t Mfs_I2C_SetNoiseFilter( stc_mfs_dnf_t* pstcMfsDnf )
{
  stc_mfs_i2cdnf_t         unNFC;
  volatile stc_mfs_nfc_t*  pstcMFS_NFC = &MFS_NFC;
  
  if (pstcMfsDnf == NULL)
  {
    return ErrorInvalidParameter;
  }
  
#if (L3_PERIPHERAL_ENABLE_MFS0 == L3_ON)
  switch(pstcMfsDnf->enDnf0)
  {
    case DnfNoFilter:
      unNFC.I2CDNF0 = 0;
      break;
    case Dnf1StepFilter:
      unNFC.I2CDNF0 = 1;
      break;    
    case Dnf2StepFilter:
      unNFC.I2CDNF0 = 2;
      break;
    case Dnf3StepFilter:
      unNFC.I2CDNF0 = 3;
      break;
    default:
      return ErrorInvalidParameter;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_MFS1 == L3_ON)
  switch(pstcMfsDnf->enDnf1)
  {
    case DnfNoFilter:
      unNFC.I2CDNF1 = 0;
      break;
    case Dnf1StepFilter:
      unNFC.I2CDNF1 = 1;
      break;    
    case Dnf2StepFilter:
      unNFC.I2CDNF1 = 2;
      break;
    case Dnf3StepFilter:
      unNFC.I2CDNF1 = 3;
      break;
    default:
      return ErrorInvalidParameter;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_MFS2 == L3_ON)
  switch(pstcMfsDnf->enDnf2)
  {
    case DnfNoFilter:
      unNFC.I2CDNF2 = 0;
      break;
    case Dnf1StepFilter:
      unNFC.I2CDNF2 = 1;
      break;    
    case Dnf2StepFilter:
      unNFC.I2CDNF2 = 2;
      break;
    case Dnf3StepFilter:
      unNFC.I2CDNF2 = 3;
      break;
    default:
      return ErrorInvalidParameter;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_MFS3 == L3_ON)
  switch(pstcMfsDnf->enDnf3)
  {
    case DnfNoFilter:
      unNFC.I2CDNF3 = 0;
      break;
    case Dnf1StepFilter:
      unNFC.I2CDNF3 = 1;
      break;    
    case Dnf2StepFilter:
      unNFC.I2CDNF3 = 2;
      break;
    case Dnf3StepFilter:
      unNFC.I2CDNF3 = 3;
      break;
    default:
      return ErrorInvalidParameter;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_MFS4 == L3_ON)
  switch(pstcMfsDnf->enDnf4)
  {
    case DnfNoFilter:
      unNFC.I2CDNF4 = 0;
      break;
    case Dnf1StepFilter:
      unNFC.I2CDNF4 = 1;
      break;    
    case Dnf2StepFilter:
      unNFC.I2CDNF4 = 2;
      break;
    case Dnf3StepFilter:
      unNFC.I2CDNF4 = 3;
      break;
    default:
      return ErrorInvalidParameter;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_MFS5 == L3_ON)
  switch(pstcMfsDnf->enDnf5)
  {
    case DnfNoFilter:
      unNFC.I2CDNF5 = 0;
      break;
    case Dnf1StepFilter:
      unNFC.I2CDNF5 = 1;
      break;    
    case Dnf2StepFilter:
      unNFC.I2CDNF5 = 2;
      break;
    case Dnf3StepFilter:
      unNFC.I2CDNF5 = 3;
      break;
    default:
      return ErrorInvalidParameter;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_MFS6 == L3_ON)
  switch(pstcMfsDnf->enDnf6)
  {
    case DnfNoFilter:
      unNFC.I2CDNF6 = 0;
      break;
    case Dnf1StepFilter:
      unNFC.I2CDNF6 = 1;
      break;    
    case Dnf2StepFilter:
      unNFC.I2CDNF6 = 2;
      break;
    case Dnf3StepFilter:
      unNFC.I2CDNF6 = 3;
      break;
    default:
      return ErrorInvalidParameter;
  }
#endif
  
#if (L3_PERIPHERAL_ENABLE_MFS7 == L3_ON)
  switch(pstcMfsDnf->enDnf7)
  {
    case DnfNoFilter:
      unNFC.I2CDNF7 = 0;
      break;
    case Dnf1StepFilter:
      unNFC.I2CDNF7 = 1;
      break;    
    case Dnf2StepFilter:
      unNFC.I2CDNF7 = 2;
      break;
    case Dnf3StepFilter:
      unNFC.I2CDNF7 = 3;
      break;
    default:
      return ErrorInvalidParameter;
  }
#endif
  
  // Update Hardware
  pstcMFS_NFC->u16I2CDNF = unNFC;
  
  return Ok;  
} // Mfs_I2C_SetNoiseFilter

#if (L3_DEVICE_TYPE != L3_TYPE3) && \
    (L3_DEVICE_TYPE != L3_TYPE6) && \
    (L3_DEVICE_TYPE != L3_TYPE7)

/**
 ******************************************************************************
 ** \brief LIN Initialisation of an MFS module.
 ** This Function initialises the  MFS according the LIN-Options setup in the
 ** passed Config Struct. Several Checkings are done before that and an error
 ** is returned if invalid Modes are requested.
 **
 ** \param [in]  pstcMfs       Pointer to MFS instance register area
 ** \param [in]  pstcConfig    MFS LIN configuration 
 **
 ** \retval Ok                    Initializiation of MFS module successfully 
 **                               done.
 ** \retval ErrorInvalidParameter If one of following conditions are met:
 **         - pstcMfs == NULL
 **         - pstcConfig == NULL
 **         - pstcMfsInternData == NULL (pstcMfs could not be resolved)
 **         - one or more enumerated values in pstcMfs out of enumaration
 **
 ******************************************************************************/
en_result_t Mfs_Lin_Init( stc_mfsn_t*                 pstcMfs, 
                          const stc_mfs_lin_config_t* pstcConfig
                        )
{
  stc_mfs_intern_data_t* pstcMfsInternData;
  uint32_t u32ReloadValue;
  
  // Preset local register variables to zero
  stc_mfsn_smr_field_t  unSMR  = { 0 } ;
  stc_mfsn_scr_field_t  unSCR  = { 0 } ;
  stc_mfsn_escr_field_t unESCR = { 0 } ;
  stc_mfsn_ssr_field_t  unSSR  = { 0 } ; 
  stc_mfsn_fcr1_field_t unFCR1 = { 0 } ;
    
  // Check for valid pointers
  if ( (pstcMfs == NULL) ||
       (pstcConfig == NULL)
     )
  {
    return ErrorInvalidParameter;
  }
  
  // Get Clock Frequency for PCLK2
  u32ReloadValue = (FM3_CRG->APBC2_PSR & 0x03);
  switch (u32ReloadValue)
  {
    case 0:
      u32ReloadValue = 1;
      break;
    case 1:
      u32ReloadValue = 2;
      break;
    case 2:
      u32ReloadValue = 4;
      break;
    case 3:
      u32ReloadValue = 8;
      break;
    default:
      u32ReloadValue = 1;
  }
  
  // Check if data rate or Clock Frequency is 0 (div by zero not wanted)
  if ( pstcConfig->u32DataRate == 0 ||
       u32ReloadValue == 0
     )
  {
      return ErrorInvalidParameter ;
  }

  // Get ptr to internal data struct ...
  pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
  // ... and check
  if ( pstcMfsInternData == NULL )
  {
      return ErrorInvalidParameter;
  }
  
  u32ReloadValue = __HCLK / u32ReloadValue;
  
  // Calc reload value based on data rate set
  u32ReloadValue /= pstcConfig->u32DataRate ;
  u32ReloadValue -= 1 ;
 
  // Set LIN mode to HW and preset variable
  pstcMfs->SMR = MFS_SMR_LIN_MODE;
  unSMR.MD0 = 1;
  unSMR.MD1 = 1;
  unSMR.MD2 = 0;
  
  // Serial Output
  unSMR.SOE = 1;
  
  // Check for Lin master or slave
  switch (pstcConfig->enLinMode)
  {
    case LinMaster:
      unSCR.MS = 0;
      break;
    case LinSlave:
      unSCR.MS = 1;
      break;
    default:
      return ErrorInvalidParameter;
  }
  
  // Set LIN master configuration
  if (pstcConfig->enLinMode == LinMaster)
  {
    switch (pstcConfig->enLinStopBits)
    {
      case LinOneStopBit:
        unSMR.SBL_SCINV = 0;
        unESCR.ESBL     = 0;
        break;
      case LinTwoStopBits:
        unSMR.SBL_SCINV = 1;
        unESCR.ESBL     = 0;
        break;
      case LinThreeStopBits:
        unSMR.SBL_SCINV = 0;
        unESCR.ESBL     = 1;
        break;
      case LinFourStopBits:
        unSMR.SBL_SCINV = 1;
        unESCR.ESBL     = 1;
        break;
      default:
        return ErrorInvalidParameter;
    }
    
    switch (pstcConfig->enLinBreakLength)
    {
      case LinBreakLength13:
        unESCR.L2_LBL0    = 0;
        unESCR.P_WT0_LBL1 = 0;
        break;
      case LinBreakLength14:
        unESCR.L2_LBL0    = 1;
        unESCR.P_WT0_LBL1 = 0;
        break;
      case LinBreakLength15:
        unESCR.L2_LBL0    = 0;
        unESCR.P_WT0_LBL1 = 1;
        break;
      case LinBreakLength16:
        unESCR.L2_LBL0    = 1;
        unESCR.P_WT0_LBL1 = 1;
        break;
      default:
        return ErrorInvalidParameter;
    }

    switch (pstcConfig->enLinDelimiterLength)
    {
      case LinDelimiterLength1:
        unESCR.L0_DEL0 = 0;
        unESCR.L1_DEL1 = 0;
        break;
      case LinDelimiterLength2:
        unESCR.L0_DEL0 = 1;
        unESCR.L1_DEL1 = 0;
        break;
      case LinDelimiterLength3:
        unESCR.L0_DEL0 = 0;
        unESCR.L1_DEL1 = 1;
        break;
      case LinDelimiterLength4:
        unESCR.L0_DEL0 = 1;
        unESCR.L1_DEL1 = 1;
        break;
      default:
        return ErrorInvalidParameter;
    }    
  }
  
  unESCR.PEN_WT1_LBIE = (pstcConfig->bLinBreakIrqEnable == TRUE) ? 1 : 0 ;
  
  // Set intern data
  pstcMfsInternData->pfnTransmitCbFunction          = pstcConfig->pfnLinTxCallback;
  pstcMfsInternData->pfnReceiveCbFunction           = pstcConfig->pfnLinRxCallback;
  pstcMfsInternData->pfnLinBreakDetectionCbFunction = pstcConfig->pfnLinBreakCallback;

    // Initialise TX ring buffer
  pstcMfsInternData->stcTxBuffer.pcBuffer      = pstcConfig->pcLinTxBuffer;
  pstcMfsInternData->stcTxBuffer.u16BufferSize = pstcConfig->u16LinTxBufferSize;
  pstcMfsInternData->stcTxBuffer.u16InIndex    = 0 ; //  Reset Index
  pstcMfsInternData->stcTxBuffer.u16OutIndex   = 0 ; //  Reset Index
  pstcMfsInternData->stcTxBuffer.u16FillCount  = 0 ; //  Reset Counter
  pstcMfsInternData->stcTxBuffer.bOverflow     = 0 ; //  Reset Flag
  
  // Initialise RX ring buffer
  pstcMfsInternData->stcRxBuffer.pcBuffer      = pstcConfig->pcLinRxBuffer;
  pstcMfsInternData->stcRxBuffer.u16BufferSize = pstcConfig->u16LinRxBufferSize;
  pstcMfsInternData->stcRxBuffer.u16InIndex    = 0 ; //  Reset Index
  pstcMfsInternData->stcRxBuffer.u16OutIndex   = 0 ; //  Reset Index
  pstcMfsInternData->stcRxBuffer.u16FillCount  = 0 ; //  Reset Counter
  pstcMfsInternData->stcRxBuffer.bOverflow     = 0 ; //  Reset Flag
  
  // Setup hardware
  pstcMfs->BGR    = u32ReloadValue & 0x7FFF ;
  pstcMfs->u8SMR  = unSMR;
  pstcMfs->u8SCR  = unSCR;
  pstcMfs->u8ESCR = unESCR;
  pstcMfs->u8FCR1 = unFCR1;
  
  pstcMfs->u8SCR.TXE = 1 ;
  pstcMfs->u8SCR.RXE = 1 ;
  
  // Init interrupts
  Mfs_InitIrq(pstcMfs);
  
  return Ok;
} // Mfs_Lin_Init

/**
 ******************************************************************************
 ** \brief Set LIN Break with an MFS module in LIN Master Mode
 **
 ** \pre MFS module must be initialized to LIN Master mode
 **
 ** This Function sets a LIN break and break delimiter length with the
 ** configuration by the previous initialization.
 ** 
 ** \param [in]  pstcMfs       Pointer to MFS instance register area
 **
 ** \retval Ok                        LIN Break is (being) generated
 ** \retval ErrorInvalidParameter     pstcMfs == NULL
 ** \retval ErrorInvalidMode          MFS not in LIN master mode
 ** \retval ErrorOperationInProgress  MFS not ready for generating break
 **
 ******************************************************************************/
en_result_t Mfs_Lin_SetBreak( stc_mfsn_t* pstcMfs )
{
  // Check for valid pointer
  if (pstcMfs == NULL)
  {
    return ErrorInvalidParameter;
  }
  
  // Check correct LIN master mode
  if ( ((pstcMfs->SMR & MFS_SMR_MODEBITS_MASK) != MFS_SMR_LIN_MODE) ||
       (pstcMfs->u8SCR.MS != 0) )
  {
    return ErrorInvalidMode;
  }
  
  // Check if transmission bus is free (no transmission ongoing)
  if (pstcMfs->u8SSR.TBI == 0)
  {
    return ErrorOperationInProgress;
  }
  
  // Finally generate LIN break with configured length and delimiter
  pstcMfs->u8SCR.SPI_LBR = 1;
  
  return Ok;
} // Mfs_Lin_SetBreak

/**
 ******************************************************************************
 ** \brief Set new baud divisor (NOT Baud Rate!) in LIN Slave Mode
 **
 ** \pre MFS module must be initialized to LIN Slave mode
 **
 ** This Function sets a new (calculated) baud divisor, if the MFS is in LIN
 ** Slave mode.
 **
 ** \note This function should only be called:
 **       - After a complete LIN frame was received and before next LIN Break
 **       - Shortly after the second ICU interrupt within the LIN Synch Field
 **         and before the next start bit of the LIN Header byte!
 ** 
 ** \param [in]  pstcMfs         Pointer to MFS instance register area
 ** \param [in]  u32BaudDivisor  New (calculated) Baud Divisor
 **
 ** \retval Ok                        Baud rate was set correctly
 ** \retval ErrorInvalidParameter     pstcMfs == NULL
 ** \retval ErrorInvalidMode          MFS not in LIN slave mode
 **
 ******************************************************************************/
en_result_t Mfs_Lin_SetNewBaudDivisor( stc_mfsn_t* pstcMfs,
                                       uint32_t u32BaudDivisor )
{
  // Check for valid pointer
  if (pstcMfs == NULL)
  {
    return ErrorInvalidParameter;
  }
  
  // Check correct LIN master mode
  if ( ((pstcMfs->SMR & MFS_SMR_MODEBITS_MASK) != MFS_SMR_LIN_MODE) ||
       (pstcMfs->u8SCR.MS != 1) )
  {
    return ErrorInvalidMode;
  } 
  
  // Update Baud Rate register
  pstcMfs->BGR = u32BaudDivisor;
    
  return Ok;
} // Mfs_Lin_SetNewBaudDivisor

/**
 ******************************************************************************
 ** \brief Transfers n bytes of recent receive buffer to user buffer
 **
 ** \param [in]  pstcMfs       Pointer to MFS instance register area
 ** \param [in]  pcData        Pointer to data buffer
 ** \param [in]  u16ReadCount  Number of bytes to be transfered
 **
 ** \retval Ok                        Baud rate was set correctly
 ** \retval ErrorInvalidParameter     pstcMfs == NULL
 **
 ******************************************************************************/
en_result_t Mfs_Lin_TransferRxBuffer( stc_mfsn_t* pstcMfs, 
                                      char_t*     pcData, 
                                      uint16_t    u16ReadCount )
{
    stc_mfs_intern_data_t* pstcMfsInternData ;
    uint16_t u16Count;
    int32_t  i32BufferCount;

    // Check for NULL Pointers
    if ( pstcMfs == NULL ||
         pcData  == NULL
       )
    {
        return ErrorInvalidParameter ;
    }

    // Check if nothing to do
    if ( u16ReadCount == 0 )
    {
        return Ok ;
    }

    // Get ptr to internal data struct ...
    pstcMfsInternData = MfsGetInternDataPtr( pstcMfs ) ;
    // ... and check
    if ( pstcMfsInternData == NULL )
    {
        return ErrorInvalidParameter ;
    }
    
    // Calculate internal buffer index
    i32BufferCount = pstcMfsInternData->stcRxBuffer.u16InIndex - u16ReadCount;
    
    // Adjust ring buffer index?
    if (i32BufferCount < 0)
    {
      i32BufferCount += pstcMfsInternData->stcRxBuffer.u16BufferSize;
    }
    
    for (u16Count = 0; u16Count < u16ReadCount; u16Count++)
    {
        *pcData = pstcMfsInternData->stcRxBuffer.pcBuffer[i32BufferCount];
        *pcData++;
        i32BufferCount++;
        
        if (i32BufferCount > pstcMfsInternData->stcRxBuffer.u16BufferSize)
        {
             i32BufferCount = 0;
        }
    }
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Disable reception interrupt
 **
 ** \param [in]  pstcMfs       Pointer to MFS instance register area
 **
 ******************************************************************************/
void Mfs_Lin_DisableRxInterrupt( stc_mfsn_t* pstcMfs )
{
   pstcMfs->u8SCR.RIE = 0;
}

/**
 ******************************************************************************
 ** \brief De-Inits a LIN MFS instance
 **
 ** This function just uses Mfs_Init()
 ** 
 ** \param [in]  pstcMfs       Pointer to MFS instance register area
 **
 ** \retval Ok                     LIN Deinitialisation of MFS module 
 **                                successfully done
 ** \retval ErrorInvalidMode       MFS not in LIN mode
 ** \retval ErrorInvalidParameter  If one of following conditions are met:
 **             - pstcMfs == NULL
 **             - pstcMfsInternData == NULL (invalid or disabled MFS unit
 **              (L3_PERIPHERAL_ENABLE_MFS)) 
 **
 ******************************************************************************/
en_result_t Mfs_Lin_DeInit( stc_mfsn_t* pstcMfs )
{
  // Check correct LIN master mode
  if ((pstcMfs->SMR & MFS_SMR_MODEBITS_MASK) != MFS_SMR_LIN_MODE)
  {
    return ErrorInvalidMode;
  }

  return Mfs_DeInit( pstcMfs );
} // Mfs_Lin_DeInit

#endif // #if (L3_DEVICE_TYPE != L3_TYPE3) &&
       //     (L3_DEVICE_TYPE != L3_TYPE6) &&
       //     (L3_DEVICE_TYPE != L3_TYPE7)

//@} // MfsGroup

#endif // #if (defined(L3_PERIPHERAL_MFS_ACTIVE))
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
