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
*******************************************************************************
* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.    */ 
/*******************************************************************************
* File Name    : ADCone.c 
* Version      : 2.00
* Device       : R5F562N8
* Tool Chain   : RX Family C Compiler
* H/W Platform : RSK+RX62N
* Description  : Defines oneshot ADC functions used in this sample.
*******************************************************************************/
/*******************************************************************************
* History	   : 23.08.2010 Ver. 1.00 First Release
*				 18.04.2011 Ver. 2.00 Second Release (Updated to RPDL CS4)
*******************************************************************************/

/*******************************************************************************
* Project Includes
*******************************************************************************/
/* ADC RPDL function definitions */
#include "r_pdl_adc_10.h"
/* General RPDL function definitions */
#include "r_pdl_definitions.h"
/* Provides declarations of functions defined in this file */
#include "ADCone.h"

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* Global variable which stores the value of the last AD conversion */
uint16_t usADC_Result = 0x0;
/* Switch flag which indicates which switches have been pressed */
extern volatile uint8_t gSwitchFlag;

/*******************************************************************************
* Local Function Prototypes
*******************************************************************************/
/* ADC read function prototype declaration */
void CB_ReadADC(void);
/* 16 bit integer to character string function prototype delcaration */
void uint16_ToString(uint8_t *, uint8_t, uint16_t);
					 
/*******************************************************************************
* Outline      : Init_ADCOneshot
* Description  : This function initialises the ADC unit for oneshot operation,
*				 sets up the switch callback function.
* Argument     : none
* Return value : none
*******************************************************************************/
void Init_ADCOneshot(void)
{
	/* Declare error flag */
	bool err = true;
	
	/* Configure the ADC unit */
	err &=	R_ADC_10_Create
			(
				1,
				PDL_ADC_10_MODE_SINGLE|PDL_ADC_10_CHANNELS_OPTION_1,
				48E6,
				4E-6,
				PDL_NO_FUNC,
				0
				
//               0,
//               PDL_ADC_10_SCAN_SINGLE|PDL_ADC_10_CHANNEL_4|PDL_ADC_10_DATA_ALIGNMENT_RIGHT|PDL_ADC_10_DIV_8,
//               PDL_ADC_10_TRIGGER_SOFTWARE,
//               PDL_NO_DATA,
//               PDL_NO_FUNC,
//               0				
			);
	
	/* Configure switch callback function */
//	SetSwitchReleaseCallback(CB_ReadADC);
	
	/* Halt in while loop when RPDL errors detected */	
	while(!err);
}
/*******************************************************************************
* End of function Init_ADCOneshot
*******************************************************************************/

/*******************************************************************************
* Outline      : Read_ADCOneshot
* Description  : Function first checks which switch triggered the interrupt, and
*				 if it was SW3, an AD conversion is triggered. The function then
*				 fetches the ADC result, and stores it in the variable usADC_
*				 Result. The function then converts the result into a character
*				 string, and displays it on the debug LCD.
* Argument     : none
* Return value : none
*******************************************************************************/
void CB_ReadADC(void)
{
	/* Declare error flag */
	bool err = true;

	/* Trigger AD conversion */
	err &=	R_ADC_10_Control
			(
				PDL_ADC_10_1_ON
			);
	
	/* Poll ADC unit until conversion is complete and store result in the
	   global variable usADC_Result */	
	err &=	R_ADC_10_Read
			(
				1,
				&usADC_Result
			);
			
	/* Halt in while loop when RPDL errors detected */	
	while(!err);
}
/*******************************************************************************
* End of function Read_ADCOneshot
*******************************************************************************/

/*******************************************************************************
* Outline      : uint16_ToString
* Description  : Function converts a 16 bit integer into a character string, 
*				 inserts it into the array via the pointer passed at execution.
* Argument     : * output_string : Pointer to uint8_t array that will hold 
*								   character string.
*							 pos : uint8_t number, element number to begin 
*								   inserting the character string from (offset).
*					input_number : 16 bit integer to convert into a string.
* Return value : none
* Note		   : No input validation is used, so output data can overflow the
*				 array passed.
*******************************************************************************/
void uint16_ToString(uint8_t * output_string, uint8_t pos, 
					 uint16_t input_number)
{
	/* Declare temporary character storage variable, and bit_shift variable */	
	uint8_t a = 0x00, bit_shift = 12u;
	
	/* Declare 16bit mask variable */
	uint16_t mask = 0xF000;
	
	/* Loop through until each hex digit is converted to an ASCII character */
	while(bit_shift < 30u)
	{
		/* Mask and shift the hex digit, and store in temporary variable, a */ 
		a = (input_number & mask) >> bit_shift;
		
		/* Convert the hex digit into an ASCII character, and store in output
		   string */
		output_string[pos] = (a < 0x0A) ? (a + 0x30) : (a + 0x37);
		
		/* Shift the bit mask 4 bits to the right, to convert the next digit */
		mask = mask >> 4u;
		
		/* Decrement the bit_shift counter by 4 (bits in a each digit) */
		bit_shift -= 4u;
		
		/* Increment the output string location */
		pos++;
	}
}
/*******************************************************************************
* End of function uint16_t_ToString
*******************************************************************************/
