/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack
 *
 * $Id: clock-arch.c,v 1.1 2011-08-04 11:02:35 hansrempel Exp $
 */

/**
 * \file
 *         Implementation of architecture-specific clock functionality
 * \author
 *         Adam Dunkels <adam@sics.se>
 */


/* IO Port RPDL function definitions */
#include "r_pdl_io_port.h"
/* CMT RPDL function definitions */
#include "r_pdl_cmt.h"
/* General RPDL function definitions */
#include "r_pdl_definitions.h"

#include "clock-arch.h"
#include "YRDKRX62N.h"

// Renesas -- #include <sys/time.h>
void int_cmt0_isr(void);

clock_time_t timer_clock_ticks = 0;


/**
 * System timer tick initialization
 *
 * This function initializes a timer interrupt with 100ms tick. 
 *
 */
void  timer_init(void)
{
  /* Declare error flag */
  bool err = true;
  
  /* CMT is configured for a 100ms interval, and executes the callback 
     function CB_CompareMatch on every compare match */
  err &=  R_CMT_Create
      (
        3,
        PDL_CMT_PERIOD,
        100E-3,
        int_cmt0_isr,
        3
      );
      
  /* Halt in while loop when RPDL errors detected */  
  while(!err);	
}

/**
 * System timer tick 
 *
 * This function is called from timer interrupt to increment a 
 * system timer tick.  Also an LED on the target board is 
 * toggled to show activity. 
 *
 */
void int_cmt0_isr(void)
{
    R_IO_PORT_Modify( LED4, PDL_IO_PORT_XOR, 1 );
	      
    timer_clock_ticks++;
}


/*---------------------------------------------------------------------------*/
clock_time_t
clock_time(void)
{
  return timer_clock_ticks;
}
/*---------------------------------------------------------------------------*/

