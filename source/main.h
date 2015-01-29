/**********************************************************************
* ETM Electromatic Inc.
* 
* FileName:        main.h
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC30F6012
* Compiler:        MPLAB® C30 v3.00 or higher
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Devin Henderlong  02/04/12  First release of source file
*                             
*                             
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* NOTES:
*
*		Summary:  The program does the following 
*					(see 78-34487-002 for more details):
*				1.  Read all 32 analog setpoints from the PLC
*				2.  Shift in appropriate data based on energy level
*				3.  Monitor if PLC wants to change data or energy level
*				
*
**********************************************************************/
#ifndef __main_h
#define __main_h


//Define Macros
#define NOP() {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}

#define HI              1
#define LOW             0

#define FCY             10000000    //40MHz clock / 4 = 10MHz

#define HIGH_DOSE       0x77
#define MEDIUM_DOSE     0xCC
#define LOW_DOSE        0xAA
#define ULTRALOW_DOSE   0x99

#define DELAY_PLC       1500000

#define HYSTERESIS      13   //sets the amount of bits to allow a change in dose
#define DOSE_LEVELS     11   //sets the amount of bits to converge into single dose level


#define COMM_DRIVER_ENABLE_TRIS		_TRISG1		//Enable the communications driver
#define COMM_DRIVER_ENABLE_PIN		_RG1

#endif




