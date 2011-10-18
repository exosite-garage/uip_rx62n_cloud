/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Technology Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Technology Corp. and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* TECHNOLOGY CORP. NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THE THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************
* Copyright (C) 2008. Renesas Technology Corp., All Rights Reserved.
*******************************************************************************	
* File Name    : user-app.c
* Version      : 1.00
* Description  : User application demonstrating sending/receiving data to/from
*                the cloud
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 08.31.2011 1.00    First Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_pdl_io_port.h"      //IO Port RPDL function definitions
#include "YRDKRX62N.h"          //Board specific definitions
#include "r_pdl_definitions.h"  //General RPDL function definitions */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "user-app.h"
#include "uip.h"
#include "timer.h"
#include "r_ether.h"
#include "lcd.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern uint16_t usADC_Result;

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
void write_to_lcd(char line_number, char * lcd_line);
void update_ui(void);
void set_led_value(u16_t value);
u16_t atoi(char ch);

struct timer net_timer;
struct timer wait_timer;

enum APPSTATES  
{
  INIT_APP,  
  WAITING_FOR_NET, 
  REQ_BASE,
  REQ_A_VAL,
  REQ_END,
  GET_A_VAL,
  SEND_A_VAL
};

static char appstate = INIT_APP;
static char ledvalue = 0;
static char ledstate = 1;
char alias_str[20];

#define ADC_SEND_ALIAS  "adc1"
#define LED_CTRL_ALIAS  "led_ctrl"
#define LED_VAL_ALIAS   "ledval"

//the RECOVERYCIK value should not be necessary unless your flash has been erased
//if your flash has been erased, you can get a CIK from your account at
//https://renesas.exosite.com
#define RECOVERYCIK       "PUTYOUR40CHARCIKHERE"

/******************************************************************************
* Function Name: user_app
* Description  : This is an example user application that sends the value read 
*                from ADC1 to the cloud.  It also reads from two different data
*                sources in the cloud to control the LEDs on the RDK         
* Arguments    : none
* Return Value : none
******************************************************************************/
void user_app(void)
{
  static unsigned char read_seq = REQ_BASE + 1;
  
  switch (appstate) {
 
    //Sends the value of adc1 to the cloud as datasource ADC_SEND_ALIAS
    case SEND_A_VAL:
      if (timer_expired(&net_timer)) {
        int volts;
        timer_reset(&net_timer);
        //Read ADC (on-board potentiometer)		  
        CB_ReadADC(); //todo: add parameter for adc channel
        volts = (usADC_Result*33/1024);
        sprintf(alias_str,"%s=%d.%d",ADC_SEND_ALIAS,volts/10,volts%10);
        update_ui();

        //Post the info to the cloud
        Exosite_Post(alias_str);
        
        //Advance to the next state
        appstate = read_seq;
      }
      break; 

    //Requests the most recent value stored in cloud datasource LED_VAL_ALIAS            
    case REQ_A_VAL: 
      if (timer_expired(&net_timer)) {
        timer_reset(&net_timer);
        //Set the name of the data we want to read
        sprintf(alias_str,"%s&%s",LED_VAL_ALIAS,LED_CTRL_ALIAS);
        update_ui();
        write_to_lcd(3,LED_VAL_ALIAS);        
        write_to_lcd(5,LED_CTRL_ALIAS);
        //Request the info from the cloud
        Exosite_Request_Get(alias_str); 
        appstate = GET_A_VAL;
      }
      break;
            
    //Retreives the most recent value read from cloud datasource LED_VAL_ALIAS      
    case GET_A_VAL: // waiting for get response
      if (timer_expired(&net_timer)) {
        timer_reset(&net_timer);
        write_to_lcd(4,"Retrying...");
        appstate = REQ_A_VAL;
        break;
      }
      if (!Exosite_Get(alias_str,LED_VAL_ALIAS)) break;
      write_to_lcd(4,alias_str); 
      {
        char len,i,j = 0;
        unsigned short value = 0;    
        len = strlen(alias_str);
        if (len > 4) len = 4;
        j = len - 1;
        //convert up to four digits into an integer
        for (i=0; i<len; i++){
          if (0 != j) value += (j*10)*atoi(alias_str[i]);  
            else value += atoi(alias_str[i]);
          j--;
        }
        ledvalue = value;       
      }
      if (!Exosite_Get(alias_str,LED_CTRL_ALIAS)) break;         
      write_to_lcd(6,alias_str); 
      //do something cool with the data
      if ('0' == *alias_str) ledstate = 0;
        else ledstate = 1;
      update_ui();        	
      appstate = SEND_A_VAL;
      if (REQ_END == ++read_seq) read_seq = REQ_BASE + 1;
      break;
      
    //Initializes timers 
    case INIT_APP:
      Init_ADCOneshot(); // initialize ADC converter
      timer_set(&wait_timer, CLOCK_SECOND*5);  //we can use the network in about 5 seconds (wait for dhcp)
      timer_set(&net_timer, CLOCK_SECOND*10); //setup communication interval
      appstate = WAITING_FOR_NET;
      if (!Exosite_Check_State())
        Exosite_Set_CIK(RECOVERYCIK);        
      update_ui();   
      break;
      
    //Sets up initial delay to allow DHCP to get an IP address
    case WAITING_FOR_NET:
      if (timer_expired(&wait_timer)) {
        //wait until network is ready and then enter normal mode
        appstate = SEND_A_VAL;
        ClearLCD();
        write_to_lcd(0, "Exosite Demo");
        update_ui();                     
      }
      break;      
    default:
      break;
  } 
}

/******************************************************************************
* void update_ui(void)
* 
* Sets LCD and LEDs to values based on data read or written
* 
******************************************************************************/
void update_ui(void) {   
  if (Exosite_Check_State())write_to_lcd(7,"==Cloud On==");
    else write_to_lcd(7,"==NEED CIK==");   
  switch (appstate) {
    case SEND_A_VAL:
        write_to_lcd(2,"Sending:");
        write_to_lcd(3,alias_str);        
        write_to_lcd(4,""); 
        write_to_lcd(5,"");  
        write_to_lcd(6,"");                
      break;  
    case GET_A_VAL: 	
        write_to_lcd(2,"Got:");       
      break;
    case REQ_A_VAL: 
        write_to_lcd(2,"Getting:");
        write_to_lcd(4,"");  
        write_to_lcd(6,"");                   
      break;
    default:
      break;
  } 
  if (ledstate) set_led_value(ledvalue);
    else set_led_value(0);      
}

/******************************************************************************
* void write_to_lcd(char line_number, char * line_str)
* 
* Helper function for writing to RDK LCD
* 
******************************************************************************/
void write_to_lcd(char line_number, char * line_str) {
  if (line_number > 7) line_number = 7;
  /* Display the contents of the local string lcd_buffer */
  DisplayLCD(line_number*8, (const uint8_t *)line_str);	
}

/******************************************************************************
* void set_led_value(u16_t value)
* 
* Sets RDK LEDs to pattern specified by value.
* 
******************************************************************************/
void set_led_value(u16_t value)
{
  char porte_value=0xff;
  char portd_value=0xff;
  
  if(value & 0x01) porte_value = porte_value & 0xf7;          // LED5 -- E3
  if((value>>=1) & 0x01) porte_value = portd_value & 0xfb;    // LED6 -- D2
  if((value>>=1) & 0x01) porte_value = porte_value & 0xfE;    // LED7 -- E0
  if((value>>=1) & 0x01) portd_value = portd_value & 0xEF;    // LED8 -- D4
  if((value>>=1) & 0x01) porte_value = porte_value & 0xfb;    // LED9 -- E2
  if((value>>=1) & 0x01) portd_value = portd_value & 0xfD;    // LED10 -- D1  
  if((value>>=1) & 0x01) portd_value = portd_value & 0x7F;    // LED11 -- D7
  if((value>>=1) & 0x01) portd_value = portd_value & 0xf7;    // LED12 -- D3  
  
  R_IO_PORT_Write(PDL_IO_PORT_D, portd_value);
  R_IO_PORT_Write(PDL_IO_PORT_E, porte_value);
}

/******************************************************************************
* u16_t atoi(char ch)
* 
* Converts ascii char ch to integer return value.
* 
******************************************************************************/
u16_t atoi(char ch)
{ 
  u16_t result=0;
  switch(ch)
  {
    case '1': result=1; break;
    case '2': result=2; break;
    case '3': result=3; break;
    case '4': result=4; break;
    case '5': result=5; break;
    case '6': result=6; break;
    case '7': result=7; break;
    case '8': result=8; break;
    case '9': result=9; break;
    case '0': result=0; break;
   default:  result=0; break;
  }
  return result;
}

/**
 * Device specific configutation parameters in flash are defined here.               
 * These parameters can be serial number, model name, etc.                           
 *                                                                                   
 * The size of this section is 64 bytes and all members must be                      
 * defined as constants.                                                             
 *                                                                                   
 * Name              Size (bytes)    Info                                            
 * -----            --------------  ------                                                             
 * my_ip               4             Default IP address if DHCP server not available 
 * my_netmask          4             Default net mask                                
 * my_default_router   4             Default router address                          
 */
#pragma section _FLASH_CONFIG_PARAMS

// Default IP address configuration
// Please make sure this IP address in the same subnet with the DHCP server
const u8_t my_ip[4]             = {192, 168, 1, 10};
const u8_t my_netmask[4]        = {255, 255, 255, 0};
const u8_t my_default_router[4] = {192, 168, 1, 1};

// Reserved 
const u8_t reserved[52] = {0};

#pragma section
