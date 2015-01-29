/**********************************************************************
* ETM Electromatic Inc.
* 
* FileName:        Initialize.h
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC30F6014
* Compiler:        MPLAB® C30 v3.00 or higher
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Devin Henderlong  12/28/11  First release of source file
*                             
*                             
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* NOTES:
*
**********************************************************************/

#ifndef  __SPIBusInterface_h
#define __SPIBusInterface_h

void WriteToSPIBus(unsigned long data, unsigned int addr);

#endif

