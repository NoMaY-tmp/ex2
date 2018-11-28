/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**********************************************************************************************************************
* File Name    : rskrx65n_uart.c
* Device(s)    : RSKRX65-2M
* Tool-Chain   : Renesas RX
* Description  :
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
***********************************************************************************************************************/

/*****************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <string.h>              // For strlen
#include "FreeRTOS.h"
#include "serial_term_uart.h"   // Serial Transfer Demo interface file.
#include "platform.h"           // Located in the FIT BSP module
#include "r_sci_rx_if.h"        // The SCI module API interface file.
#include "r_byteq_if.h"         // The BYTEQ module API interface file.
#include "r_sci_rx_config.h"    // User configurable options for the SCI module
#include "r_pinset.h"


/*******************************************************************************
 Macro definitions
 *******************************************************************************/

/*******************************************************************************
 Exported global variables and functions (to be accessed by other files)
 *******************************************************************************/

/*******************************************************************************
 Private variables and functions
 *******************************************************************************/

/*****************************************************************************
Private global variables and functions
******************************************************************************/
static void my_sci_callback(void *pArgs);

/* Handle storage. */
sci_hdl_t     my_sci_handle;

/*****************************************************************************
* Function Name: uart_config
* Description  : prepares UART for operation
* Arguments    : none
* Return Value : none
******************************************************************************/
void uart_config(void)
{
    sci_cfg_t   my_sci_config;
    sci_err_t   my_sci_err;

    /* Initialize the I/O port pins for communication on this SCI channel.
    * This is specific to the MCU and ports chosen. For the RSKRX65-2M we will use the
    * SCI channel connected to the USB serial port emulation. */
#if (BSP_CFG_BOARD_REVISION == 0) || (BSP_CFG_BOARD_REVISION == 2)
    R_SCI_PinSet_SCI2();
#elif (BSP_CFG_BOARD_REVISION == 1)
    R_SCI_PinSet_SCI8();
#elif (BSP_CFG_BOARD_REVISION == 3)
    R_SCI_PinSet_SCI12();
#elif (BSP_CFG_BOARD_REVISION == 4)
    R_SCI_PinSet_SCI7();
#elif (BSP_CFG_BOARD_REVISION == 5)
    R_SCI_PinSet_SCI5();
#elif (BSP_CFG_BOARD_REVISION == 6)
    R_SCI_PinSet_SCI0();
#endif
    /* Set up the configuration data structure for asynchronous (UART) operation. */
    my_sci_config.async.baud_rate    = 115200;
    my_sci_config.async.clk_src      = SCI_CLK_INT;
    my_sci_config.async.data_size    = SCI_DATA_8BIT;
    my_sci_config.async.parity_en    = SCI_PARITY_OFF;
    my_sci_config.async.parity_type  = SCI_EVEN_PARITY;
    my_sci_config.async.stop_bits    = SCI_STOPBITS_1;
    my_sci_config.async.int_priority = 3;    // 1=lowest, 15=highest

    /* OPEN ASYNC CHANNEL
    *  Provide address of the configure structure,
    *  the callback function to be assigned,
    *  and the location for the handle to be stored.*/
#if (BSP_CFG_BOARD_REVISION == 0) || (BSP_CFG_BOARD_REVISION == 2)
    my_sci_err = R_SCI_Open(SCI_CH2, SCI_MODE_ASYNC, &my_sci_config, my_sci_callback, &my_sci_handle);
#elif (BSP_CFG_BOARD_REVISION == 1)
    my_sci_err = R_SCI_Open(SCI_CH8, SCI_MODE_ASYNC, &my_sci_config, my_sci_callback, &my_sci_handle);
#elif (BSP_CFG_BOARD_REVISION == 3)
    my_sci_err = R_SCI_Open(SCI_CH12, SCI_MODE_ASYNC, &my_sci_config, my_sci_callback, &my_sci_handle);
#elif (BSP_CFG_BOARD_REVISION == 4)
    my_sci_err = R_SCI_Open(SCI_CH7, SCI_MODE_ASYNC, &my_sci_config, my_sci_callback, &my_sci_handle);
#elif (BSP_CFG_BOARD_REVISION == 5)
    my_sci_err = R_SCI_Open(SCI_CH5, SCI_MODE_ASYNC, &my_sci_config, my_sci_callback, &my_sci_handle);
#elif (BSP_CFG_BOARD_REVISION == 6)
    my_sci_err = R_SCI_Open(SCI_CH0, SCI_MODE_ASYNC, &my_sci_config, my_sci_callback, &my_sci_handle);
#endif

    /* If there were an error this would demonstrate error detection of API calls. */
    if (SCI_SUCCESS != my_sci_err)
    {
        R_NOP(); // Your error handling code would go here.
    }
} /* End of function uart_config() */


/*****************************************************************************
* Function Name: my_sci_callback
* Description  : This is a template for an SCI Async Mode callback function.
* Arguments    : pArgs -
*                pointer to sci_cb_p_args_t structure cast to a void. Structure
*                contains event and associated data.
* Return Value : none
******************************************************************************/
static void my_sci_callback(void *pArgs)
{
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
        R_NOP();
    }
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
        R_NOP();
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        R_NOP();
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        R_NOP();
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        R_NOP();
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */


void uart_string_printf(char *pString) {
	int32_t length = 0;
	sci_err_t sci_err;
	uint32_t retry = 0xFFFF;

	length = strlen(pString);

	if (length > 0) {
		do {
			sci_err = R_SCI_Send(my_sci_handle, pString, (uint16_t) length);
			retry--;
		} while ((SCI_ERR_XCVR_BUSY == sci_err) && (retry > 0)); // retry if previous transmission still in progress.

		if (SCI_SUCCESS != sci_err) {
			R_NOP(); //TODO error handling code
		}
	}
}
