/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Electronics Corp. and is
* only intended for use with Renesas products.  

* No other uses are authorized.

* This software is owned by Renesas Electronics Corp. and is 
* protected under the applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WAWRRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH
* WARRANTIES ARE EXPRESSLY DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER 
* RENESAS ELECTRONICS CORP. NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR AND DIRECT, INDIRECT, SPECIAL, INCIDENTAL
* OR COSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE,
* EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE
* POSSIBILITIES OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue availability of this software.
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
*******************************************************************************/
/* Copyright (C) 2010. Renesas Electronics Corp., All Rights Reserved         */
/******************************************************************************
* File Name     : intprg.c
* Version       : 1.1
* Device(s)     : R5F562N8
* Tool-Chain    : Renesas RX Standard Toolchain 1.0.1
* OS            : None
* H/W Platform  : YRDKRX62N
* Description   : Interrupt vectors
* Limitations   : None
*******************************************************************************
* History : DD.MMM.YYYY   Version    Description
*         : 08.Oct.2010   1.00       First release
*         : 02.Dec.2010   1.10       Second YRDK release
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <machine.h>
#include "vect.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern void int_cmt0_isr(void);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

#pragma section IntPRG

// Exception(Supervisor Instruction)
void Excep_SuperVisorInst(void)
{/* brk(); */
}

// Exception(Undefined Instruction)
void Excep_UndefinedInst(void)
{/* brk(); */
}

// Exception(Floating Point)
void Excep_FloatingPoint(void)
{/* brk(); */
}

// NMI
void NonMaskableInterrupt(void)
{/* brk(); */
}

// Dummy
void Dummy(void)
{/* brk(); */
}

// BRK
void Excep_BRK(void)
{
    wait();
}

// BUSERR
void Excep_BUSERR(void)
{
}

// FCU_FCUERR
void Excep_FCU_FCUERR(void)
{
}

// FCU_FRDYI
void Excep_FCU_FRDYI(void)
{
}

// CMTU0_CMT0
void Excep_CMTU0_CMT0(void)
{
    int_cmt0_isr();
}

// CMTU0_CMT1
void Excep_CMTU0_CMT1(void)
{
}

// CMTU1_CMT2
void Excep_CMTU1_CMT2(void)
{
}

// CMTU1_CMT3
void Excep_CMTU1_CMT3(void)
{
}

// IRQ0
void Excep_IRQ0(void)
{
}

// IRQ1
void Excep_IRQ1(void)
{
}

// IRQ2
void Excep_IRQ2(void)
{
}

// IRQ3
void Excep_IRQ3(void)
{
}

// IRQ4
void Excep_IRQ4(void)
{
}

// IRQ5
void Excep_IRQ5(void)
{
}

// IRQ6
void Excep_IRQ6(void)
{
}

// IRQ7
void Excep_IRQ7(void)
{
}

// IRQ8
void Excep_IRQ8(void)
{
}

// IRQ9
void Excep_IRQ9(void)
{
}

// IRQ10
void Excep_IRQ10(void)
{
}

// IRQ11
void Excep_IRQ11(void)
{
}

// IRQ12
void Excep_IRQ12(void)
{
}

// IRQ13
void Excep_IRQ13(void)
{
}

// IRQ14
void Excep_IRQ14(void)
{
}

// IRQ15
void Excep_IRQ15(void)
{
}

// WDT_WOVI
void Excep_WDT_WOVI(void)
{
}

// AD0_ADI0
void Excep_AD0_ADI0(void)
{
}

// AD1_ADI1
void Excep_AD1_ADI1(void)
{
}

// AD2_ADI2
void Excep_AD2_ADI2(void)
{
}

// AD3_ADI3
void Excep_AD3_ADI3(void)
{
}

// TPU0_TGI0A
void Excep_TPU0_TGI0A(void)
{
}

// TPU0_TGI0B
void Excep_TPU0_TGI0B(void)
{
}

// TPU0_TGI0C
void Excep_TPU0_TGI0C(void)
{
}

// TPU0_TGI0D
void Excep_TPU0_TGI0D(void)
{
}

// TPU0_TCI0V
void Excep_TPU0_TCI0V(void)
{
}

// TPU1_TGI1A
void Excep_TPU1_TGI1A(void)
{
}

// TPU1_TGI1B
void Excep_TPU1_TGI1B(void)
{
}

// TPU1_TCI1V
void Excep_TPU1_TCI1V(void)
{
}

// TPU1_TCI1U
void Excep_TPU1_TCI1U(void)
{
}

// TPU2_TGI2A
void Excep_TPU2_TGI2A(void)
{
}

// TPU2_TGI2B
void Excep_TPU2_TGI2B(void)
{
}

// TPU2_TCI2V
void Excep_TPU2_TCI2V(void)
{
}

// TPU2_TCI2U
void Excep_TPU2_TCI2U(void)
{
}

// TPU3_TGI3A
void Excep_TPU3_TGI3A(void)
{
}

// TPU3_TGI3B
void Excep_TPU3_TGI3B(void)
{
}

// TPU3_TGI3C
void Excep_TPU3_TGI3C(void)
{
}

// TPU3_TGI3D
void Excep_TPU3_TGI3D(void)
{
}

// TPU3_TCI3V
void Excep_TPU3_TCI3V(void)
{
}

// TPU4_TGI4A
void Excep_TPU4_TGI4A(void)
{
}

// TPU4_TGI4B
void Excep_TPU4_TGI4B(void)
{
}

// TPU4_TCI4V
void Excep_TPU4_TCI4V(void)
{
}

// TPU4_TCI4U
void Excep_TPU4_TCI4U(void)
{
}

// TPU5_TGI5A
void Excep_TPU5_TGI5A(void)
{
}

// TPU5_TGI5B
void Excep_TPU5_TGI5B(void)
{
}

// TPU5_TCI5V
void Excep_TPU5_TCI5V(void)
{
}

// TPU5_TCI5U
void Excep_TPU5_TCI5U(void)
{
}

// TPU6_TGI6A
void Excep_TPU6_TGI6A(void)
{
}

// TPU6_TGI6B
void Excep_TPU6_TGI6B(void)
{
}

// TPU6_TGI6C
void Excep_TPU6_TGI6C(void)
{
}

// TPU6_TGI6D
void Excep_TPU6_TGI6D(void)
{
}

// TPU6_TCI6V
void Excep_TPU6_TCI6V(void)
{
}

// TPU7_TGI7A
void Excep_TPU7_TGI7A(void)
{
}

// TPU7_TGI7B
void Excep_TPU7_TGI7B(void)
{
}

// TPU7_TCI7V
void Excep_TPU7_TCI7V(void)
{
}

// TPU7_TCI7U
void Excep_TPU7_TCI7U(void)
{
}

// TPU8_TGI8A
void Excep_TPU8_TGI8A(void)
{
}

// TPU8_TGI8B
void Excep_TPU8_TGI8B(void)
{
}

// TPU8_TCI8V
void Excep_TPU8_TCI8V(void)
{
}

// TPU8_TCI8U
void Excep_TPU8_TCI8U(void)
{
}

// TPU9_TGI9A
void Excep_TPU9_TGI9A(void)
{
}

// TPU9_TGI9B
void Excep_TPU9_TGI9B(void)
{
}

// TPU9_TGI9C
void Excep_TPU9_TGI9C(void)
{
}

// TPU9_TGI9D
void Excep_TPU9_TGI9D(void)
{
}

// TPU9_TCI9V
void Excep_TPU9_TCI9V(void)
{
}

// TPU10_TGI10A
void Excep_TPU10_TGI10A(void)
{
}

// TPU10_TGI10B
void Excep_TPU10_TGI10B(void)
{
}

// TPU10_TCI10V
void Excep_TPU10_TCI10V(void)
{
}

// TPU10_TCI10U
void Excep_TPU10_TCI10U(void)
{
}

// TPU11_TGI11A
void Excep_TPU11_TGI11A(void)
{
}

// TPU11_TGI11B
void Excep_TPU11_TGI11B(void)
{
}

// TPU11_TCI11V
void Excep_TPU11_TCI11V(void)
{
}

// TPU11_TCI11U
void Excep_TPU11_TCI11U(void)
{
}

// TMR0_CMI0A
void Excep_TMR0_CMI0A(void)
{
}

// TMR0_CMI0B
void Excep_TMR0_CMI0B(void)
{
}

// TMR0_OV0I
void Excep_TMR0_OV0I(void)
{
}

// TMR1_CMI1A
void Excep_TMR1_CMI1A(void)
{
}

// TMR1_CMI1B
void Excep_TMR1_CMI1B(void)
{
}

// TMR1_OV1I
void Excep_TMR1_OV1I(void)
{
}

// TMR2_CMI2A
void Excep_TMR2_CMI2A(void)
{
}

// TMR2_CMI2B
void Excep_TMR2_CMI2B(void)
{
}

// TMR2_OV2I
void Excep_TMR2_OV2I(void)
{
}

// TMR3_CMI3A
void Excep_TMR3_CMI3A(void)
{
}

// TMR3_CMI3B
void Excep_TMR3_CMI3B(void)
{
}

// TMR3_OV3I
void Excep_TMR3_OV3I(void)
{
}

// DMAC_DMTEND0
void Excep_DMAC_DMTEND0(void)
{
}

// DMAC_DMTEND1
void Excep_DMAC_DMTEND1(void)
{
}

// DMAC_DMTEND2
void Excep_DMAC_DMTEND2(void)
{
}

// DMAC_DMTEND3
void Excep_DMAC_DMTEND3(void)
{
}

// SCI0_ERI0
void Excep_SCI0_ERI0(void)
{
}

// SCI0_RXI0
void Excep_SCI0_RXI0(void)
{
}

// SCI0_TXI0
void Excep_SCI0_TXI0(void)
{
}

// SCI0_TEI0
void Excep_SCI0_TEI0(void)
{
}

// SCI1_ERI1
void Excep_SCI1_ERI1(void)
{
}

// SCI1_RXI1
void Excep_SCI1_RXI1(void)
{
}

// SCI1_TXI1
void Excep_SCI1_TXI1(void)
{
}

// SCI1_TEI1
void Excep_SCI1_TEI1(void)
{
}

// SCI2_ERI2
void Excep_SCI2_ERI2(void)
{
}

// SCI2_RXI2
void Excep_SCI2_RXI2(void)
{
}

// SCI2_TXI2
void Excep_SCI2_TXI2(void)
{
}

// SCI2_TEI2
void Excep_SCI2_TEI2(void)
{
}

// SCI3_ERI3
void Excep_SCI3_ERI3(void)
{
}

// SCI3_RXI3
void Excep_SCI3_RXI3(void)
{
}

// SCI3_TXI3
void Excep_SCI3_TXI3(void)
{
}

// SCI3_TEI3
void Excep_SCI3_TEI3(void)
{
}

// SCI4_ERI4
void Excep_SCI4_ERI4(void)
{
}

// SCI4_RXI4
void Excep_SCI4_RXI4(void)
{
}

// SCI4_TXI4
void Excep_SCI4_TXI4(void)
{
}

// SCI4_TEI4
void Excep_SCI4_TEI4(void)
{
}

// SCI5_ERI5
void Excep_SCI5_ERI5(void)
{
}

// SCI5_RXI5
void Excep_SCI5_RXI5(void)
{
}

// SCI5_TXI5
void Excep_SCI5_TXI5(void)
{
}

// SCI5_TEI5
void Excep_SCI5_TEI5(void)
{
}

// SCI6_ERI6
void Excep_SCI6_ERI6(void)
{
}

// SCI6_RXI6
void Excep_SCI6_RXI6(void)
{
}

// SCI6_TXI6
void Excep_SCI6_TXI6(void)
{
}

// SCI6_TEI6
void Excep_SCI6_TEI6(void)
{
}

// RIIC0_EEI0
void Excep_RIIC0_EEI0(void)
{
}

// RIIC0_RXI0
void Excep_RIIC0_RXI0(void)
{
}

// RIIC0_TXI0
void Excep_RIIC0_TXI0(void)
{
}

// RIIC0_TEI0
void Excep_RIIC0_TEI0(void)
{
}

// RIIC1_EEI1
void Excep_RIIC1_EEI1(void)
{
}

// RIIC1_RXI1
void Excep_RIIC1_RXI1(void)
{
}

// RIIC1_TXI1
void Excep_RIIC1_TXI1(void)
{
}

// RIIC1_TEI1
void Excep_RIIC1_TEI1(void)
{
}


/* End of File */
