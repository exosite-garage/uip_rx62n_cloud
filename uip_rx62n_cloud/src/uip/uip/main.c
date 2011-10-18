/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Adam Dunkels.
 * 4. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: main.c,v 1.7 2011-09-05 11:17:13 hansrempel Exp $
 *
 */
#include "iodefine_RPDL.h"
#include "typedefine.h"


#include "uip.h"
#include "uip_arp.h"
// Renesas -- #include "network-device.h"
#include <stdio.h>
#include <string.h>
#include "r_ether.h"
//#include "httpd.h"
#include "timer.h"
#include "user-app.h"
#include "lcd.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

void DisplayuIPDemo(void);
void DisplayIPAddress(const unsigned short ipaddr[] );

/*----------------------------------------------------------------------------*/ 
int
main(void)
{
  int i;
  // Renesas -- uip_ipaddr_t ipaddr;
  struct timer periodic_timer, arp_timer;
  struct uip_eth_addr my_mac;
  
  uint32_t ch = 0;

  // Renesas ++
  InitialiseLCD();
  DisplayuIPDemo();

  timer_init();
  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);

  Exosite_Get_MAC((unsigned char *)&my_mac);
  // Renesas -- network_device_init();
  /* Wait until Ether device initailize succesfully. 
     Make sure Ethernet cable is plugged in. */ 
  while (R_ETHER_ERROR == R_Ether_Open(ch, (uint8_t*)&my_mac));

  // Renesas ++ set Ethernet address
  uip_setethaddr(my_mac);
  uip_init();

  // Renesas -- 
  //uip_ipaddr(ipaddr, 192,168,0,2);
  //uip_sethostaddr(ipaddr);
  dhcpc_init(&my_mac, 6);
  if (!Exosite_Init(APP_NAME, APP_VERSION)) DisplayLCD(LCD_LINE8, "==NEED CIK=="); 

  while (1)
  {
    // Renesas -- uip_len = network_device_read();
    uip_len = R_Ether_Read(ch, (void *)uip_buf);
    if (uip_len > 0)
    {
      if (BUF->type == htons(UIP_ETHTYPE_IP))
      {
        uip_arp_ipin();
        uip_input();
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if (uip_len > 0)
        {
          uip_arp_out();
          // Renesas -- network_device_send();
          R_Ether_Write(ch, (void *)uip_buf, (uint32_t)uip_len);
        }
      }
      else if (BUF->type == htons(UIP_ETHTYPE_ARP))
      {
        uip_arp_arpin();
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if (uip_len > 0)
        {
          // Renesas -- network_device_send();
          R_Ether_Write(ch, (void *)uip_buf, (uint32_t)uip_len);
        }
      }

    }
    else if (timer_expired(&periodic_timer))
    {
      timer_reset(&periodic_timer);
      for (i = 0; i < UIP_CONNS; i++)
      {
        uip_periodic(i);
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if (uip_len > 0)
        {
          uip_arp_out();
          // Renesas -- network_device_send();
          R_Ether_Write(ch, (void *)uip_buf, (uint32_t)uip_len);
        }
      }

#if UIP_UDP
      for (i = 0; i < UIP_UDP_CONNS; i++)
      {
        uip_udp_periodic(i);
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if (uip_len > 0)
        {
          uip_arp_out();
          // Renesas -- network_device_send();
          R_Ether_Write(ch, (void *)uip_buf, (uint32_t)uip_len);
        }
      }
#endif /* UIP_UDP */

      /* Call the ARP timer function every 10 seconds. */
      if (timer_expired(&arp_timer))
      {
        timer_reset(&arp_timer);
        uip_arp_timer();
      }
    }

    // Insert user aplications here.
    // Call WEB application that controls LEDs on the target board.
    user_app();
	
  }
  return 0;
}
/*---------------------------------------------------------------------------*/

// Renesas ++
/**********************************************************************************
Name:			DisplayuIPDemo
Description:	Clears the LCD and displays "uIP Demo" on line 1.    
Parameters:		None
Returns:		None
**********************************************************************************/
void DisplayuIPDemo(void)
{
    ClearLCD();
    DisplayLCD(LCD_LINE1, "Exosite Demo");
}

// Renesas ++
/**********************************************************************************
Name:			DisplayIPAddress
Description:	Clears the LCD and displays IP address in dotted decimal format. 
                First two decimals are displyed on line 1 and the others on
                line 2.    
Parameters:		(unsigned short []) ipaddr - IP address to display
Returns:		None
**********************************************************************************/
void DisplayIPAddress(const unsigned short ipaddr[])
{
    char dis_buf[9];
    char *p = (char *)ipaddr;
    DisplayLCD(LCD_LINE1, "Exosite Demo");
    DisplayLCD(LCD_LINE3, "IP Addr =");
    sprintf(dis_buf, "%d.%d.", *p, *(p+1));
    DisplayLCD(LCD_LINE4, (const uint8_t *)dis_buf);
    sprintf(dis_buf, "%d.%d", *(p+2), *(p+3));
    DisplayLCD(LCD_LINE5, (const uint8_t *)dis_buf);
}

