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
/** \file mfs.h
 **
 ** Headerfile for MFS functions
 **  
 **
 ** History:
 **   - 2012-02-21  1.0  MWi  First version.
 **   - 2012-05-11  1.1  MWi  L3 address definition for instance pointer used,
 **                           I2C noise filter added
 **   - 2012-07-09  1.2  MWi  No LIN support for Device Type 3, 6, 7 added
 **   - 2012-08-29  1.3  MWi  Doxygen description upgraded
 **
 ******************************************************************************/

#ifndef __MFS_H__
#define __MFS_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "l3.h"
#include "mcu.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup MfsGroup Mfs Functions (MFS)
 **
 ** Provided functions of MFS module:
 ** 
 ** - Mfs_Init()
 ** - Mfs_DeInit() 
 ** - Mfs_Read()
 ** - Mfs_Write()
 ** - Mfs_SynchronousTransfer() 
 ** - Mfs_Purge()
 ** - Mfs_PurgeRx()
 ** - Mfs_PurgeTx()
 ** - Mfs_BuffersEmpty()
 ** - Mfs_ReadChar()
 ** - Mfs_WriteString()
 ** - Mfs_ReadString
 ** - Mfs_I2C_Start()
 ** - Mfs_I2C_Continue()
 ** - Mfs_I2C_Stop()
 ** - Mfs_I2C_WriteSync()
 ** - Mfs_I2C_ReadSync()
 ** - Mfs_I2C_SetNoiseFilter()
 ** - Mfs_Lin_Init()
 ** - Mfs_Lin_SetBreak()
 ** - Mfs_Lin_SetNewBaudDivisor()
 ** - Mfs_Lin_DisableRxInterrupt()
 ** - Mfs_Lin_TransferRxBuffer()
 ** - Mfs_Lin_DeInit()
 **
 ** \note MFS uses a fixed internal clock and fixed 8 data bits.
 ** \note MFS can read/write data synchronously and asynchronously 
 **        (API handling).
 **
 ** \brief How to use MFS module
 **
 ** Mfs_Init() must be used for activation and setup of an MFS channel. All
 ** MFS connection related parameters can be set with parameter 
 ** pstcConfig of type #stc_mfs_config_t. 
 **
 ** The following restrictions have to be considered:
 **   -There must be a start bit in MFS asynchronous mode.
 **   -The clock source must be #MfsUseExternalSerialClockSourceAsItIs in 
 **    MFS synchronous slave mode.         
 **   -Continous clock output must only be active in MFS synchronous master
 **    mode. 
 **
 ** On initialisation, the API has to provide a memory location for receive and 
 ** transmit buffer handling (also for DMA usage). The memory location will 
 ** be set on initialisation with stc_mfs_config_t#pcTxBuffer, 
 ** stc_mfs_config_t#u16TxBufferSize, stc_mfs_config_t#pcRxBuffer and 
 ** stc_mfs_config_t#u16RxBufferSize. 
 ** 
 ** The MFS initialisation will create a 
 ** receive and transmit ring buffer on this memory location and handle all 
 ** ring buffer related parameters (#stc_mfs_buffer_t). So each MFS 
 ** instance will consist of a 2 step buffer handling by combination of the 
 ** ring buffer with the hardware internal fifo buffer. The length of the 
 ** receive and transmit hardware fifo is #MFS_FIFO_MAX_VAL (usually 16). 
 **
 ** The receive and transmit interrupt level of each hardware fifo is set 
 ** internally by the MFS implementation and
 ** will be changed according to the transmit and receive data length while any
 ** transmission or receiption of data is ongoing. Beside the Mfs_Read() and 
 ** Mfs_Write() function it is possible to simulate a SPI transfer by using 
 ** the function Mfs_SynchronousTransfer(). The MFS has to be initialised in 
 ** synchronous mode for using Mfs_SynchronousTransfer(). To change the mode of
 ** MFS (synchronous to asynchronous and vice versa) or any other MFS connection 
 ** related settings it will be neccessary to call first Mfs_DeInit() to 
 ** reset all MFS related  register and to call Mfs_Init() afterwards with 
 ** new mode and settings. 
 **
 ** Any ongoing or pending transmission or receiption will be cancelled
 ** and any data in the hardware fifos and ring buffer will be lost.
 ** The API can call Mfs_Purge() to interrupt an ongoing transmission or 
 ** receiption without changing the MFS mode and settings. All transmission 
 ** and receiption related buffer will be reset and all data in the buffer will 
 ** be lost.
 ** 
 ** If the MFS module is used in synchronous receive and/or transmit mode
 ** (stc_mfs_config_t#pfnTxCallback and/or stc_mfs_config_t#pfnRxCallback 
 ** are NULL) then the corresponding read and write functions (Mfs_Read(), 
 ** Mfs_Write() and Mfs_SynchronousTransfer()) have to be called with parameter
 ** bWait set to TRUE. Note that this meaning of 
 ** synchronous or asynchronous API handling is not the same like MFS 
 ** synchronous or asynchronous data transfer. The MFS module uses the receive
 ** and transmit buffer provided by the API to create ring buffers. So there 
 ** are 2 receive and transmit steps for sending and receiving data: the
 ** ring software buffer and the hardware fifo. All neccessary parameter to 
 ** configure a MFS channel can be set with the Mfs_Init#pstcConfig 
 ** parameter, all other interfaces (Mfs_Read(), Mfs_Write(), Mfs_Purge()
 ** and Mfs_SynchronousTransfer()) will only operate with data to be sent or
 ** received over the configured MFS channel.     
 **
 ** The API can handle the receive and transmit communication by using the
 ** Mfs_Read(), Mfs_Write() and Mfs_SynchronousTransfer() either in synchronous
 ** or asynchronous way. There is no need for setting the callback function
 ** stc_mfs_config_t#pfnTxCallback in #stc_mfs_config_t when using the 
 ** Mfs_Write() function in synchronous handling mode by setting the 
 ** parameter bWait to TRUE. Note that in synchronous API mode the function 
 ** will block until all data is put into the transmit hardware fifo. 
 ** The stc_mfs_config_t#pfnTxCallback in #stc_mfs_config_t must
 ** be set to a valid value if the API will use the Mfs_Write() function in 
 ** an asynchronous way by setting the parameter bWait to FALSE.
 ** The same for Mfs_Read() function and Mfs_SynchronousTransfer() function
 ** (since Mfs_SynchronousTransfer() uses receive interrupt to signal
 ** transmission complete).
 ** The API can use Mfs_Read() and Mfs_SynchronousTransfer in an aysynchronous
 ** way by setting the bWait parameter to FALSE. In this case the 
 ** stc_mfs_config_t#pfnRxCallback in #stc_mfs_config_t must be set to a 
 ** valid value on MFS initialisation.
 ** The function Mfs_Read() will block until the amount of received data 
 ** matches the Mfs_Read#u16ReadCount value if used in synchronous way. 
 ** The function Mfs_SynchronousTransfer() will only block if it is used in
 ** asynchronous way and data should be received.
 ** The Mfs_SynchronousTransfer() function will neither block in asynchronous
 ** nor in synchronous mode if it is used only for sending data 
 ** (Mfs_Transfer#pu8RxData set to NULL) due to the 
 ** Mfs_Transfer#u8TransferSize restriction (Mfs_Transfer#u8TransferSize 
 ** maximum length is the hardware fifo length).
 **
 ** For each transmission function (Mfs_Read(), Mfs_Write()) 
 ** is only one asynchronous operation possible at a time. 
 **
 ** The I2C functionality is done w/o interrupts or FIFO usage. It uses only
 ** the following parameter of the MFS configuration:
 ** stc_mfs_config_t#enMode and stc_mfs_config_t#u32DataRate.
 ** After initialization of the MFS to I2C master mode, a start condition has
 ** to be sent via Mfs_I2C_Start(). Then depending on the protocol bytes can
 ** be sent via Mfs_I2C_WriteSync(). If a continued (or repeated) start
 ** condition has to be sent, Mfs_I2C_Continue() has to be used. Any
 ** communication has to be finished by the Stop Condition, which is sent out
 ** by Mfs_I2C_Stop() with the parameter en_mfs_i2c_datadirection_t#I2CWrite
 ** or en_mfs_i2c_datadirection_t#I2CRead.
 **
 ** Mfs_I2C_SetNoiseFilter() sets the MFS's noise filter.
 **
 ** For using MFS's LIN Mode use only function with 'Mfs_Lin_' prefix in the
 ** name (Except for Mfs_Read() and Mfs_Write()) Also #stc_mfs_lin_config_t has
 ** to be used as the MFS configuration instead of #stc_mfs_config_t.
 ** Mfs_Lin_Init() is used to initialize an MFS instance to LIN mode with its
 ** dedicated LIN configuration (#stc_mfs_lin_config_t).
 ** Mfs_Lin_SetBreak() set the LIN Break in LIN Master mode. The baud rate
 ** divisor (not the rate itself!) can be adjusted by
 ** Mfs_Lin_SetNewBaudDivisor() after measurement with a dedicated ICU in LIN
 ** Slave mode.
 ** Note that the LIN functionality only works properly when the MFS is
 ** connected to a LIN transceiver, which means, that the SOT line as always
 ** read-back by SIN input!
 ** Mfs_Lin_DisableRxInterrupt() is used to disable the Rx interrupt, if a LIN
 ** frame was completely read and a new frame beginning with the LIN break
 ** is awaited to avoid unnecessary reception of a '0'-Byte with a framing
 ** error.
 ** Mfs_Lin_TransferRxBuffer() transfers the reception data from the internal
 ** ring buffer to a user buffer. This function can be used for LIN Master and
 ** Slave mode, because of the external LIN transceiver, every data
 ** (transmission and/or reception) is always read completely to the reception
 ** buffer.
 ** Note that Device Types 3, 6, and 7 do not support LIN mode.
 ** 
 ******************************************************************************/
//@{

/**
 ******************************************************************************
 ** \brief Mfscallback function prototypes.
 ******************************************************************************/
typedef void (*mfs_tx_cb_func_ptr_t)(void);
typedef void (*mfs_rx_cb_func_ptr_t)(void); 
typedef void (*mfs_err_cb_func_ptr_t)(void);

/******************************************************************************
 * Global definitions
 ******************************************************************************/

#define MFS0    (*((volatile stc_mfsn_t *) L3_MFS0_BASE))
#define MFS1    (*((volatile stc_mfsn_t *) L3_MFS1_BASE))
#define MFS2    (*((volatile stc_mfsn_t *) L3_MFS2_BASE))
#define MFS3    (*((volatile stc_mfsn_t *) L3_MFS3_BASE))
#define MFS4    (*((volatile stc_mfsn_t *) L3_MFS4_BASE))
#define MFS5    (*((volatile stc_mfsn_t *) L3_MFS5_BASE))
#define MFS6    (*((volatile stc_mfsn_t *) L3_MFS6_BASE))
#define MFS7    (*((volatile stc_mfsn_t *) L3_MFS7_BASE))
#define MFS_NFC (*((volatile stc_mfs_nfc_t *) L3_MFS_NFC_BASE))

// Definition of FIFO availability
#define MFS0_FIFO MfsHasNoFifo
#define MFS1_FIFO MfsHasNoFifo
#define MFS2_FIFO MfsHasNoFifo
#define MFS3_FIFO MfsHasNoFifo
#define MFS4_FIFO MfsHasFifo
#define MFS5_FIFO MfsHasFifo
#define MFS6_FIFO MfsHasFifo
#define MFS7_FIFO MfsHasFifo

/// I2C Start connection trials until time out
#define I2C_TIME_OUT_POLLING_TRIALS 200

/// I2C receive INT polling time-out. Note that this value can be shortened, if
///   L3_WAIT_LOOP_HOOK() consumes significant time.
#define I2C_TIME_OUT_POLLING_TRIALS_INT 1000

/// SMR Mode bits mask
#define MFS_SMR_MODEBITS_MASK 0xE0   // MD2, MD1, MD0, 0, 0, 0, 0, 0
/// SMR LIN Mode bits
#define MFS_SMR_LIN_MODE      0x60   // MD2 = 0, MD1 = 1, MD0 = 1

/**
 ******************************************************************************
 ** \brief Fifo related settings
 ** These values represent the minimum and maximum values of the hardware
 ** Fifo level settings. (Interrupt) Trigger levels should be set to a level
 ** that will allow at least 1 remaining byte to be transmitted or received 
 ** also if the interrupt is pending. Using half of the FiFo size is always 
 ** a good choice to get balanced load and continuous data flow.
 ******************************************************************************/
#define MFS_FIFO_MIN_VAL            0
#define MFS_FIFO_MAX_VAL            8

#define MFS_BUFFER_LENGTH 256

/******************************************************************************
 * Global type definitions
 ******************************************************************************/

/**
 ******************************************************************************
 ** \brief Redefinition of MFS SMR bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_smr_field
{
  __IO  uint8_t SOE       :1;
  __IO  uint8_t SCKE      :1;
  __IO  uint8_t BDS       :1;
  __IO  uint8_t SBL_SCINV :1;   ///< SBL: uart mode, SCINV: csio mode
  __IO  uint8_t WUCR      :1;
  __IO  uint8_t MD0       :1;
  __IO  uint8_t MD1       :1;
  __IO  uint8_t MD2       :1; 
}stc_mfsn_smr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS SCR bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_scr_field
{
  __IO  uint8_t TXE       :1;
  __IO  uint8_t RXE       :1;
  __IO  uint8_t TBIE      :1;
  __IO  uint8_t TIE       :1;
  __IO  uint8_t RIE       :1;
  __IO  uint8_t SPI_LBR   :1;   ///< SPI: csio mode, LBR: lin mode
  __IO  uint8_t MS        :1;   ///< csio, lin mode
  __IO  uint8_t UPCL      :1; 
}stc_mfsn_scr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS ESCR bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_escr_field
{
  __IO  uint8_t L0_DEL0      :1;   ///< L0: csio mode, DEL0 lin mode
  __IO  uint8_t L1_DEL1      :1;   ///< L1: csio mode, DEL1 lin mode
  __IO  uint8_t L2_LBL0      :1;   ///< L2: csio mode, LBL0 lin mode
  __IO  uint8_t P_WT0_LBL1   :1;   ///< P: uart mode, WT0: csio mode, LBL1 lin mode
  __IO  uint8_t PEN_WT1_LBIE :1;   ///< PEN: uart mode, WT1: csio mode, LBIE lin mode
  __IO  uint8_t INV          :1;
  __IO  uint8_t ESBL         :1;
  __IO  uint8_t FLWEN_SOP    :1;   ///< FLWEN: uart mode, SOP: csio mode
}stc_mfsn_escr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS SSR bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_ssr_field
{
  __IO  uint8_t TBI       :1;
  __IO  uint8_t TDRE      :1;
  __IO  uint8_t RDRF      :1;
  __IO  uint8_t ORE       :1;
  __IO  uint8_t FRE       :1;
  __IO  uint8_t PE_LBD    :1;
  __IO  uint8_t Reserved  :1;
  __IO  uint8_t REC       :1; 
}stc_mfsn_ssr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS BGR bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_bgr_field
{
  __IO  uint16_t BGR      :15;
  __IO  uint16_t EXT      :1; 
}stc_mfsn_bgr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS FCR0 bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_fcr0_field
{
  __IO  uint8_t FE1       :1;
  __IO  uint8_t FE2       :1;
  __IO  uint8_t FCL1      :1;
  __IO  uint8_t FCL2      :1;
  __IO  uint8_t FSET      :1;
  __IO  uint8_t FLD       :1;
  __IO  uint8_t FLST      :1;
  __IO  uint8_t Reserved  :1;
}stc_mfsn_fcr0_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS FCR1 bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_fcr1_field
{
  __IO  uint8_t FSEL      :1;
  __IO  uint8_t FTIE      :1;
  __IO  uint8_t FDRQ      :1;
  __IO  uint8_t FRIIE     :1;
  __IO  uint8_t FLSTE     :1;
  __IO  uint8_t Reserved  :1;
  __IO  uint8_t FTST0     :1;
  __IO  uint8_t FTST1     :1; 
}stc_mfsn_fcr1_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS-I2C SMR bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_i2c_smr_field
{
  __IO  uint8_t Reserved  :2;
  __IO  uint8_t TIE       :1;
  __IO  uint8_t RIE       :1;
  __IO  uint8_t WUCR      :1;
  __IO  uint8_t MD0       :1;
  __IO  uint8_t MD1       :1;
  __IO  uint8_t MD2       :1; 
}stc_mfsn_i2c_smr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS-I2C IBCR bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_i2c_ibcr_field
{
  __IO  uint8_t INT       :1;
  __IO  uint8_t BER       :1;  
  __IO  uint8_t INTE      :1;
  __IO  uint8_t CNDE      :1;
  __IO  uint8_t WSEL      :1;
  __IO  uint8_t ACKE      :1;
  __IO  uint8_t ACT_SCC   :1;
  __IO  uint8_t MSS       :1; 
}stc_mfsn_i2c_ibcr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS-I2C IBSR bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_i2c_ibsr_field
{
  __IO  uint8_t BB        :1;
  __IO  uint8_t SPC       :1;  
  __IO  uint8_t RSC       :1;
  __IO  uint8_t AL        :1;
  __IO  uint8_t TRX       :1;
  __IO  uint8_t RSA       :1;
  __IO  uint8_t RACK      :1;
  __IO  uint8_t FBT       :1; 
}stc_mfsn_i2c_ibsr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS-I2C SSR bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_i2c_ssr_field
{
  __IO  uint8_t TBI       :1;
  __IO  uint8_t TDRE      :1;  
  __IO  uint8_t RDRF      :1;
  __IO  uint8_t ORE       :1;
  __IO  uint8_t TBIE      :1;
  __IO  uint8_t DMA       :1;
  __IO  uint8_t TSET      :1;
  __IO  uint8_t REC       :1; 
}stc_mfsn_i2c_ssr_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS-I2C ISBA bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_i2c_isba_field
{
  __IO  uint8_t SA0       :1;
  __IO  uint8_t SA1       :1;  
  __IO  uint8_t SA2       :1;
  __IO  uint8_t SA3       :1;
  __IO  uint8_t SA4       :1;
  __IO  uint8_t SA5       :1;
  __IO  uint8_t SA6       :1;
  __IO  uint8_t SAEN      :1; 
}stc_mfsn_i2c_isba_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS-I2C ISMK bit field structures for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn_i2c_ismk_field
{
  __IO  uint8_t SM0       :1;
  __IO  uint8_t SM1       :1;  
  __IO  uint8_t SM2       :1;
  __IO  uint8_t SM3       :1;
  __IO  uint8_t SM4       :1;
  __IO  uint8_t SM5       :1;
  __IO  uint8_t SM6       :1;
  __IO  uint8_t EN        :1; 
}stc_mfsn_i2c_ismk_field_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS register structure for unification of
 **        all modes
 ******************************************************************************/
typedef struct stc_mfsn {
  union {
    uint8_t                   SMR;
    stc_mfsn_smr_field_t      u8SMR;
    stc_mfsn_i2c_smr_field_t  u8SMR_I2C;
  };
  union {
    uint8_t                   SCR;
    stc_mfsn_scr_field_t      u8SCR;
    uint8_t                   IBCR;
    stc_mfsn_i2c_ibcr_field_t u8IBCR;
  };
  uint16_t                    Reserved0;
  union {
    uint8_t                   ESCR;
    stc_mfsn_escr_field_t     u8ESCR;
    uint8_t                   IBSR;
    stc_mfsn_i2c_ibsr_field_t u8IBSR;
  };
  union {
    uint8_t                   SSR;
    stc_mfsn_ssr_field_t      u8SSR;
    stc_mfsn_i2c_ssr_field_t  u8SSR_I2C;
  };
  uint16_t                    Reserved1;
  union {
    __IO uint16_t             TDR;
    __IO uint16_t             RDR;
  };
  uint16_t                    Reserved2;
  union {
    uint16_t                  BGR;
    stc_mfsn_bgr_field_t      u16BGR;
  };
  uint16_t                    Reserved3;
  union {
    uint8_t                   ISBA;
    stc_mfsn_i2c_isba_field_t u8ISBA;    
  };
  union {
    uint8_t                   ISMK;
    stc_mfsn_i2c_ismk_field_t u8ISMK;   
  }; 
  uint16_t                    Reserved4;
  union {
    uint8_t                   FCR0;
    stc_mfsn_fcr0_field_t     u8FCR0;
  };
  union {
    uint8_t                   FCR1;
    stc_mfsn_fcr1_field_t     u8FCR1;
  };
  uint16_t                    Reserved5;
  __IO uint8_t                FBYTE1;
  __IO uint8_t                FBYTE2;
} stc_mfsn_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS Nois Filter register structure
 ******************************************************************************/
typedef struct stc_mfs_i2cdnf
{
  __IO uint16_t I2CDNF0 : 2;
  __IO uint16_t I2CDNF1 : 2;
  __IO uint16_t I2CDNF2 : 2;
  __IO uint16_t I2CDNF3 : 2;
  __IO uint16_t I2CDNF4 : 2;
  __IO uint16_t I2CDNF5 : 2;
  __IO uint16_t I2CDNF6 : 2;
  __IO uint16_t I2CDNF7 : 2;
} stc_mfs_i2cdnf_t;

/**
 ******************************************************************************
 ** \brief Redefinition of MFS Nois Filter register structure
 ******************************************************************************/
typedef struct stc_mfs_nfc
{
  stc_mfs_i2cdnf_t u16I2CDNF;
} stc_mfs_nfc_t ;

/**
 ******************************************************************************
 ** \brief Mfs mode
 ** 
 ** To select between synchronous and asynchronous MFS transmission
 **
 ** \note For using LIN mode use own LIN functions with 'Mfs_Lin' prefix!
 ******************************************************************************/
typedef enum en_mfs_mode
{
  MfsAsynchronous0      = 0,  ///< Normal mode, in this mode it is not allowed to use #MfsNoStartNoStop for en_mfs_start_stopbit_t#enStartStopBit
  MfsAsynchronousMulti1 = 1,  ///< Multi-Processor Mode, in this mode it is not allowed to use #MfsNoStartNoStop for en_mfs_start_stopbit_t#enStartStopBit
  MfsSynchronousMaster2 = 2,  ///< Master mode (generating serial clock)
  MfsSynchronousSlave2  = 3,  ///< Slave mode  (external serial clock)
  MfsI2cMaster          = 4,  ///< I2C Master Mode
  MfsI2cSlave           = 5   ///< I2C Slave Mode
} en_mfs_mode_t;


/**
 ******************************************************************************
 ** \brief Mfs clock source
 ** 
 ** To select clock source
 ******************************************************************************/
typedef enum en_mfs_clk_src
{
  MfsUseInternalBaudRateGenerator                      = 0,   ///< For asynchronous operation
  MfsUseExternalSerialClockSourceWithBaudRateGenerator = 1,   ///< For synchronous master operation
  MfsUseExternalSerialClockSourceAsItIs                = 3    ///< Must be used for synchronous slave operation
} en_mfs_clk_src_t;


/**
 ******************************************************************************
 ** \brief Mfs parity.
 ******************************************************************************/
typedef enum en_mfs_parity
{
  MfsParityNone  = 0,    ///< No parity bit is used.
  MfsParityEven  = 2,    ///< Even parity bit is used.
  MfsParityOdd   = 3     ///< Odd parity bit is used.
} en_mfs_parity_t;

/**
 ******************************************************************************
 ** \brief Mfs character length.
 ******************************************************************************/
typedef enum en_mfs_characterlength
{
  MfsFifeBits  = 5,    ///< 5 Bit character length
  MfsSixBits   = 6,    ///< 6 Bit character length
  MfsSevenBits = 7,    ///< 7 Bit character length
  MfsEightBits = 8,    ///< 8 Bit character length
  MfsNineBits  = 9     ///< 9 Bit character length
} en_mfs_characterlength_t;

/**
 ******************************************************************************
 ** \brief Mfs start/stop bit.
 **
 ** Enum value #MfsNoStartNoStop is not allowed in asynchronous mode! 
 ******************************************************************************/
typedef enum en_mfs_start_stopbit
{
  MfsNoStartNoStop     = 0,    ///< No start and no stop bit (only valid for sync mode)
  MfsOneStartOneStop   = 1,    ///< 1 start and 1 stop bit (only valid for async mode)
  MfsOneStartTwoStop   = 3,    ///< 1 start and 2 stop bits (only valid for async mode)
  MfsOneStartThreeStop = 4,    ///< 1 start and 3 stop bits (only valid for async mode)
  MfsOneStartFourStop  = 5     ///< 1 start and 4 stop bits (only valid for async mode)
} en_mfs_start_stopbit_t;

/**
 ******************************************************************************
 ** \brief Mfs synchronous wait time insertion.
 ******************************************************************************/
typedef enum en_mfs_sync_wait_time
{
  MfsSyncWaitZero  = 0,    ///< 0 wait time insertion
  MfsSyncWaitOne   = 6,    ///< 1 wait time insertion
  MfsSyncWaitTwo   = 7,    ///< 2 wait time insertion
  MfsSyncWaitThree = 8,    ///< 3 wait time insertion
} en_mfs_sync_wait_time_t;

/**
 ******************************************************************************
 ** \brief Mfs callback mode.
 **
 ** Decides transmit interrupt trigger point.  
 ******************************************************************************/
typedef enum en_mfs_tx_callback_mode
{
  MfsOnTxBufferEmpty = 0,          ///< Transmit interrupt when last byte of fifo buffer was sent to hardware shift register.
  MfsOnTxFinished    = 1           ///< Transmit interrupt when last bit was shift out of shift register, no more data in hardware fifo.
} en_mfs_tx_callback_mode_t;

/**
 ******************************************************************************
 ** \brief Mfs FIFO availability
 **
** Set by Mfs_Init() in #stc_mfs_intern_data.  
 ******************************************************************************/
typedef enum en_mfs_fifo_available
{
  MfsHasNoFifo = 0,                 ///< MFS has no FIFO available.
  MfsHasFifo   = 1                  ///< MFS has a FIFO available.
} en_mfs_fifo_available_t;

/**
 ******************************************************************************
 ** \brief Mfs ring buffer.
 ** 
 ** Contains all parameter for ring buffer handling. 
 ******************************************************************************/
typedef struct stc_mfs_buffer
{
    char_t*   pcBuffer;       ///< Pointer to character buffer.
    uint16_t  u16BufferSize;  ///< Size of buffer.
    uint16_t  u16InIndex;     ///< Index of next element to store on buffer.
    uint16_t  u16OutIndex;    ///< Index of next element to read from buffer.
    uint16_t  u16FillCount;   ///< Indicates if elements are available in buffer.
    boolean_t bOverflow ;     ///< Indicates Overrun Condition
} stc_mfs_buffer_t;

/**
 ******************************************************************************
 ** \brief Mfs I2C data direction (read/write)
 ******************************************************************************/
typedef enum en_mfs_i2c_datadirection
{
  I2CWrite = 0,  // Do not change enumeration numbers!
  I2CRead  = 1
} en_mfs_i2c_datadirection_t ;

#if (L3_DEVICE_TYPE != L3_TYPE3) && \
    (L3_DEVICE_TYPE != L3_TYPE6) && \
    (L3_DEVICE_TYPE != L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Mfs Lin Mode (Master, Slave)
 ******************************************************************************/
typedef enum en_lin_mode
{
  LinMaster = 0,    ///< Lin Master Mode
  LinSlave  = 1     ///< Lin Slave Mode
} en_lin_mode_t;

/**
 ******************************************************************************
 ** \brief Mfs Lin Stop Bit Length
 ******************************************************************************/
typedef enum en_lin_stop_bit_length
{
  LinOneStopBit    = 0, ///< 1 Stop Bit
  LinTwoStopBits   = 1, ///< 2 Stop Bits
  LinThreeStopBits = 2, ///< 3 Stop Bits
  LinFourStopBits  = 3  ///< 4 Stop Bits
} en_lin_stop_bit_length_t;

/**
 ******************************************************************************
 ** \brief Mfs Lin Break Generation Length (only applicable in LIN master mode)
 ******************************************************************************/
typedef enum en_lin_break_length
{
  LinBreakLength13 = 0, ///< Lin Break Length 13 Bit Times
  LinBreakLength14 = 1, ///< Lin Break Length 14 Bit Times
  LinBreakLength15 = 2, ///< Lin Break Length 15 Bit Times
  LinBreakLength16 = 3  ///< Lin Break Length 16 Bit Times
} en_lin_break_length_t;

/**
 ******************************************************************************
 ** \brief Mfs Lin Break Delimiter Length (only applicable in LIN master mode)
 ******************************************************************************/
typedef enum en_lin_delimiter_length
{
  LinDelimiterLength1 = 0, ///< Lin Break Delimiter Length 1 Bit Time
  LinDelimiterLength2 = 1, ///< Lin Break Delimiter Length 2 Bit Times
  LinDelimiterLength3 = 2, ///< Lin Break Delimiter Length 3 Bit Times
  LinDelimiterLength4 = 3  ///< Lin Break Delimiter Length 4 Bit Times
} en_lin_delimiter_length_t;

#endif // #if (L3_DEVICE_TYPE != L3_TYPE3) &&
       //     (L3_DEVICE_TYPE != L3_TYPE6) &&
       //     (L3_DEVICE_TYPE != L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Mfs I2C Auxiliary Nois Filter Settings
 ******************************************************************************/
typedef enum en_mfs_dnf
{
  DnfNoFilter    = 0,   ///< Does not add auxiliary noise filter. APB1 bus clock <= 40 MHz
  Dnf1StepFilter = 1,   ///< Add 1 step of auxiliary noise filter. 40 MHz < APB1 bus clock <= 60 MHz
  Dnf2StepFilter = 2,   ///< Add 2 steps of auxiliary noise filter. 60 MHz < APB1 bus clock <= 80 MHz
  Dnf3StepFilter = 3    ///< Add 3 steps of auxiliary noise filter. 80 MHz < APB1 bus clock <= 100 MHz
} en_mfs_dnf_t;

/**
 ******************************************************************************
 ** \brief Mfs configuration.
 **
 ** Contains all parameter for configuratin an MFS channel.  
 ******************************************************************************/
typedef struct stc_mfs_config
{
    en_mfs_mode_t             enMode;                         ///< See description of #en_mfs_mode_t.
    en_mfs_clk_src_t          enClockSource;                  ///< See description of #en_mfs_clk_src_t. Must be used in synchronous slave mode.
    uint32_t                  u32DataRate;                    ///< Bits per second, not valid in #MfsSynchronousSlave2 mode.
    en_mfs_parity_t           enParity;                       ///< See description of #en_mfs_parity_t.
    en_mfs_start_stopbit_t    enStartStopBit;                 ///< See description of #en_mfs_start_stopbit_t.
    en_mfs_characterlength_t  u8CharLength;                   ///< 5..9 Bit Character Length, see description of #en_mfs_characterlength_t.
    boolean_t                 bBitDirection;                  ///< FALSE: LSB first
                                                              ///< TRUE: MSB first
    boolean_t                 bSyncClockInversion;            ///< Determines sampling point for receive data. 
                                                              ///< FALSE: Sampling on rising edge.
                                                              ///< TRUE: Sampling on falling edge.
                                                              ///< Sampling clock is an internal clock which is equal to output clock in case of #bSyncClockOutputDelay == FALSE
    en_mfs_sync_wait_time_t   u8SyncClockWaitTime;            ///< Synchronous clock wait time insertion (0 - 3)
    boolean_t                 bSignalSystem;                  ///< FALSE: NRZ
                                                              ///< TRUE: Inverted NRZ
    boolean_t                 bSyncClockOutputDelay;          ///< If this bit is set the serial output clock is delayed (SPI).
    char_t*                   pcTxBuffer;                     ///< Char store for transmit buffer.      
    uint16_t                  u16TxBufferSize;                ///< Size of transmit fifo buffer.
    char_t*                   pcRxBuffer;                     ///< Char store for receive buffer.
    uint16_t                  u16RxBufferSize;                ///< Size of receive fifo buffer.
    uint16_t                  u16RxCallbackBufferFillLevel ;  ///< # of Data into Buffer until RX Callback fct will be called
    en_mfs_tx_callback_mode_t enTxCallbackMode;               ///< See description of #en_mfs_tx_callback_mode_t.
    mfs_tx_cb_func_ptr_t      pfnTxCallback;                  ///< Can be NULL for synchronous operation (mode without interrupts).
    mfs_rx_cb_func_ptr_t      pfnRxCallback;                  ///< Can be NULL for synchronous operation (mode without interrupts).
    mfs_err_cb_func_ptr_t     pfnErrCallback;                 ///< Can be NULL for synchronous operation (mode without interrupts).
    boolean_t                 bSubstLFwithCRLF;               ///< If enabled, each string sent via Usart_WriteString will add a CR if last char is a LF
} stc_mfs_config_t;

#if (L3_DEVICE_TYPE != L3_TYPE3) && \
    (L3_DEVICE_TYPE != L3_TYPE6) && \
    (L3_DEVICE_TYPE != L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Mfs LIN configuration.
 **
 ** Contains all parameter for configuratin a MFS channel, if set to LIN mode.
 ** This configuration was rolled-out from #stc_mfs_config_t to save RAM
 ** memory since LIN is a special mode. Only use functions with prefix
 ** 'Mfs_Lin_'!
 ******************************************************************************/
typedef struct stc_mfs_lin_config
{
  en_lin_mode_t              enLinMode;             ///< See #en_lin_mode_t for details
  boolean_t                  bExtWakeUp;            ///< TRUE: Sets external wake-up function
  uint32_t                   u32DataRate;           ///< Bits per second
  en_lin_stop_bit_length_t   enLinStopBits;         ///< See #en_lin_stop_bit_length_t for details
  en_lin_break_length_t      enLinBreakLength;      ///< See #en_lin_break_length_t for details. Only applicable in LIN master mode
  en_lin_delimiter_length_t  enLinDelimiterLength;  ///< See #en_lin_delimiter_length_t
  boolean_t                  bLinBreakIrqEnable;    ///< TRUE: Enable LIN break receptioninterrupt
  char_t*                    pcLinTxBuffer;         ///< Char store for LIN transmission buffer
  uint16_t                   u16LinTxBufferSize;    ///< Buffer Size of LIN transmission buffer
  char_t*                    pcLinRxBuffer;         ///< Char store for LIN reception buffer
  uint16_t                   u16LinRxBufferSize;    ///< Buffer Size of LIN reception buffer
  mfs_rx_cb_func_ptr_t       pfnLinBreakCallback;   ///< Callback function, if LIN break was detected
  mfs_rx_cb_func_ptr_t       pfnLinRxCallback;      ///< Callback function, if LIN RX Buffer is full
  mfs_tx_cb_func_ptr_t       pfnLinTxCallback;      ///< Callback function, if LIN RX Buffer is full
} stc_mfs_lin_config_t;

#endif // #if (L3_DEVICE_TYPE != L3_TYPE3) &&
       //     (L3_DEVICE_TYPE != L3_TYPE6) &&
       //     (L3_DEVICE_TYPE != L3_TYPE7)

/**
 ******************************************************************************
 ** \brief Mfs I2C Noise Filter configuration
 ******************************************************************************/
typedef struct stc_mfs_dnf
{
  en_mfs_dnf_t     enDnf0;  ///< I2C Filter for MFS0
  en_mfs_dnf_t     enDnf1;  ///< I2C Filter for MFS1
  en_mfs_dnf_t     enDnf2;  ///< I2C Filter for MFS2
  en_mfs_dnf_t     enDnf3;  ///< I2C Filter for MFS3
  en_mfs_dnf_t     enDnf4;  ///< I2C Filter for MFS4
  en_mfs_dnf_t     enDnf5;  ///< I2C Filter for MFS5
  en_mfs_dnf_t     enDnf6;  ///< I2C Filter for MFS6
  en_mfs_dnf_t     enDnf7;  ///< I2C Filter for MFS7
} stc_mfs_dnf_t;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/// Enumeration to define an index for each enabled MFS instance
typedef enum en_mfs_instance_index
{
    #if (L3_PERIPHERAL_ENABLE_MFS0 == L3_ON)
    MfsInstanceIndexMfs0,
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS1 == L3_ON)
    MfsInstanceIndexMfs1,
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS2 == L3_ON)
    MfsInstanceIndexMfs2,
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS3 == L3_ON)
    MfsInstanceIndexMfs3,
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS4 == L3_ON)
    MfsInstanceIndexMfs4,
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS5 == L3_ON)
    MfsInstanceIndexMfs5,
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS6 == L3_ON)
    MfsInstanceIndexMfs6,
    #endif
    #if (L3_PERIPHERAL_ENABLE_MFS7 == L3_ON)
    MfsInstanceIndexMfs7,
    #endif
    MfsInstanceIndexMax
} en_mfs_instance_index_t;

/// MFS module internal data, storing internal information for each enabled MFS instance.
typedef struct stc_mfs_intern_data
{
    mfs_tx_cb_func_ptr_t        pfnTransmitCbFunction;  ///< Transmission complete callback function
    mfs_rx_cb_func_ptr_t        pfnReceiveCbFunction;   ///< Receive callback function
    mfs_rx_cb_func_ptr_t        pfnLinBreakDetectionCbFunction; ///< LIN break detection callback function
    mfs_err_cb_func_ptr_t       pfnErrorCbFunction;     ///< Error callback function
    stc_mfs_buffer_t            stcRxBuffer;            ///< Store location for receive Fifo buffer
    stc_mfs_buffer_t            stcTxBuffer;            ///< Store location for transmission Fifo buffer
    uint16_t                    u16RxCallbackBufferFillLevel ;///< # of Data into Buffer until Callback fct will be called
    en_mfs_tx_callback_mode_t   enTxCallbackMode;       ///< Transmit callback mode
    boolean_t                   bSubstLFwithCRLF;       ///< If enabled, each string sent via Usart_WriteString will add a CR to any occurence of a LF
    en_mfs_fifo_available_t     enFifoAvailable;        ///< Availability of MFS FIFO
} stc_mfs_intern_data_t;

/// MFS instance data type
typedef struct stc_mfs_instance_data
{
    volatile stc_mfsn_t*  pstcInstance;  ///< pointer to registers of an instance
    stc_mfs_intern_data_t stcInternData; ///< module internal data of instance
} stc_mfs_instance_data_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/// Look-up table for all enabled MFS instances and their internal data
extern stc_mfs_instance_data_t m_astcMfsInstanceDataLut[];

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/

extern void MfsIrqHandlerTx( volatile stc_mfsn_t* pstcMfs, 
                               stc_mfs_intern_data_t* pstcMfstInternData
                             ) ;

extern void MfsIrqHandlerRx( volatile stc_mfsn_t* pstcMfs, 
                               stc_mfs_intern_data_t* pstcMfsInternData
                             ) ;

en_result_t Mfs_Init( stc_mfsn_t* pstcMfs, 
                               const stc_mfs_config_t* pstcConfig
                             ) ;

en_result_t Mfs_DeInit( stc_mfsn_t* pstcMfs ) ;

en_result_t Mfs_Read( stc_mfsn_t* pstcMfs, 
                               char_t* pcBuffer,                         
                               uint16_t* pu16DataCount, 
                               uint16_t u16ReadCount, 
                               boolean_t bWait
                             ) ;

en_result_t Mfs_Write( stc_mfsn_t* pstcMfs, 
                                const char_t* pcData, 
                                uint16_t u16DataLength, 
                                boolean_t bWait
                              ) ;

en_result_t Mfs_SynchronousTransfer( stc_mfsn_t* pstcMfs, 
                                              uint8_t* pu8TxData, 
                                              uint8_t* pu8RxData, 
                                              uint8_t u8TransferSize
                                            ) ;

en_result_t Mfs_PurgeRx( stc_mfsn_t* pstcMfs ) ;

en_result_t Mfs_PurgeTx( stc_mfsn_t* pstcMfs ) ;

en_result_t Mfs_Purge( stc_mfsn_t* pstcMfs ) ;

en_result_t Mfs_BuffersEmpty( stc_mfsn_t* pstcMfs ) ;

char_t      Mfs_ReadChar ( stc_mfsn_t* pstcMfs, 
                                    boolean_t bWait
                                  ) ;

en_result_t Mfs_WriteString( stc_mfsn_t* pstcMfs, 
                                      char_t* pcData,
                                      boolean_t bWait
                                    ) ;

int16_t Mfs_ReadString( stc_mfsn_t* pstcMfs, 
                          char_t* pcData,
                          uint16_t u16MaxChars,
                          char_t cTerminator,
                          boolean_t  bEcho
                        ) ;

en_result_t Mfs_AsyncInit( stc_mfsn_t*   pstcMfs,
                                    uint32_t        u32DataRate,
                                    en_mfs_parity_t enParity,
                                    en_mfs_start_stopbit_t enStartStopBit,
                                    mfs_tx_cb_func_ptr_t pfnTxCallback,
                                    mfs_rx_cb_func_ptr_t pfnRxCallback
                                  ) ;

en_result_t Mfs_I2C_Start( volatile stc_mfsn_t* pstcI2c,
                           uint8_t  u8DestAddress,
                           en_mfs_i2c_datadirection_t enAccess
                         ) ;

en_result_t Mfs_I2C_Continue( volatile stc_mfsn_t* pstcI2c,
                              uint8_t  u8DestAddress,
                              en_mfs_i2c_datadirection_t enAccess
                            ) ;

en_result_t Mfs_I2C_Stop( volatile stc_mfsn_t* pstcI2c,
                          en_mfs_i2c_datadirection_t enAccess ) ;

en_result_t Mfs_I2C_WriteSync( volatile stc_mfsn_t* pstcI2c,
                               uint8_t* pu8Data,
                               uint8_t u8DataLength
                             ) ;

en_result_t Mfs_I2C_ReadSync( volatile stc_mfsn_t* pstcI2c,
                              uint8_t* pu8Data,
                              uint8_t u8DataLength
                            ) ;

en_result_t Mfs_I2C_SetNoiseFilter( stc_mfs_dnf_t* pstcMfsDnf ) ;

#if (L3_DEVICE_TYPE != L3_TYPE3) && \
    (L3_DEVICE_TYPE != L3_TYPE6) && \
    (L3_DEVICE_TYPE != L3_TYPE7)

en_result_t Mfs_Lin_Init( stc_mfsn_t*                 pstcMfs, 
                          const stc_mfs_lin_config_t* pstcConfig
                        ) ;

en_result_t Mfs_Lin_SetBreak( stc_mfsn_t* pstcMfs ) ;

en_result_t Mfs_Lin_SetNewBaudDivisor( stc_mfsn_t* pstcMfs,
                                       uint32_t u32BaudRate );

void Mfs_Lin_DisableRxInterrupt( stc_mfsn_t* pstcMfs ) ;

en_result_t Mfs_Lin_TransferRxBuffer( stc_mfsn_t* pstcMfs, 
                                      char_t*     pcData, 
                                      uint16_t    u16ReadCount ) ;

en_result_t Mfs_Lin_DeInit( stc_mfsn_t* pstcMfs ) ;

#endif // #if (L3_DEVICE_TYPE != L3_TYPE3) &&
       //     (L3_DEVICE_TYPE != L3_TYPE6) &&
       //     (L3_DEVICE_TYPE != L3_TYPE7)

//@} // MfsGroup

#ifdef __cplusplus
}
#endif

#endif /* __MFS_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
