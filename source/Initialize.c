/**********************************************************************
* ETM Electromatic Inc.
* 
* FileName:        Initialize.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC30F6014
* Compiler:        MPLAB® C30 v3.00 or higher
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Devin Henderlong  3/5/12  First release of source file
*                             
*                             
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* NOTES:
*
**********************************************************************/

#include <p30f6014A.h>
#include <dsp.h>
#include "main.h"
#include <libpic30.h>
#include "Initialize.h"
#include "SerialComm.h"

void Initialize(void);
void InitPins(void);
void InitADC(void);
void LEDCheck(void);
unsigned char GetPersonality(void);
int SendPersonalityToPLC(unsigned char);
void InitTimer2(void);
void InitUART(void);
void InitINT3(void);
void InitINT4(void);

void Initialize(void)
{
    int x;

    InitPins();

        //Do the LED check for close to 5 seconds while the PLC is turning on
    for (x = 0; x < 13; x++)
    {
        LEDCheck();
        ClrWdt()
    }

    InitADC();
    //InitTimer2();

    //For Serial Communication, uncomment out the next line
    //Also uncomment DoSerialCommand(); in main.c
    InitUART();

    InitINT3(); //Trigger Interrupt
}

void InitPins()
{
        PIN_PW_SHIFT_OUT = !OLL_PW_SHIFT;
        PIN_PW_CLR_CNT_OUT = OLL_PW_CLR_CNT;			 // clear active
        PIN_PW_HOLD_LOWRESET_OUT = !OLL_PW_HOLD_LOWRESET;	 // reset active

	TRIS_PIN_PW_SHIFT_OUT = TRIS_OUTPUT_MODE;
        TRIS_PIN_PW_CLR_CNT_OUT = TRIS_OUTPUT_MODE;
	TRIS_PIN_PW_HOLD_LOWRESET_OUT = TRIS_OUTPUT_MODE;	
    

	PIN_LED_HVON = !OLL_LED_ON;
	TRIS_PIN_LED_HVON = TRIS_OUTPUT_MODE;
    
	PIN_LED_STANDBY = !OLL_LED_ON;
	TRIS_PIN_LED_STANDBY = TRIS_OUTPUT_MODE;
    
	PIN_LED_WARMUP = !OLL_LED_ON;
	TRIS_PIN_LED_WARMUP = TRIS_OUTPUT_MODE;
    
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	TRIS_PIN_LED_XRAY_ON = TRIS_OUTPUT_MODE;
    
	PIN_LED_SUMFLT = !OLL_LED_ON;
	TRIS_PIN_LED_SUMFLT = TRIS_OUTPUT_MODE;

	// Personality
 	PIN_ID_SHIFT_OUT = !OLL_ID_SHIFT;
	TRIS_PIN_ID_SHIFT_OUT = TRIS_OUTPUT_MODE;

 	PIN_ID_CLK_OUT = !OLL_ID_CLK;
	TRIS_PIN_ID_CLK_OUT = TRIS_OUTPUT_MODE;
   
	TRIS_PIN_ID_DATA_IN = TRIS_INPUT_MODE;

    //STATUS from PLC
    TRIS_PIN_CPU_STANDBY_IN     = TRIS_INPUT_MODE;
    TRIS_PIN_CPU_READY_IN       = TRIS_INPUT_MODE;
    TRIS_PIN_CPU_XRAY_ENABLE_IN = TRIS_INPUT_MODE;
    
    TRIS_PIN_CPU_SUMFLT_IN		= TRIS_INPUT_MODE;
    TRIS_PIN_CPU_WARMUP_IN		= TRIS_INPUT_MODE;
    
    TRIS_PIN_CPU_WARNING_LAMP_IN = TRIS_INPUT_MODE;
    TRIS_PIN_CPU_HV_ENABLE_IN	 = TRIS_INPUT_MODE;
    
    TRIS_PIN_LOW_MODE_IN 		 = TRIS_INPUT_MODE;
    TRIS_PIN_HIGH_MODE_IN 		 = TRIS_INPUT_MODE;
    TRIS_PIN_PACKAGE_ID1_IN		 = TRIS_INPUT_MODE;
    
    
    TRIS_PIN_PACKAGE_VALID_IN	 = TRIS_INPUT_MODE;
    TRIS_PIN_KEY_LOCK_IN		 = TRIS_INPUT_MODE;
    TRIS_PIN_PANEL_IN			 = TRIS_INPUT_MODE;


    //Control to PFN control board
    TRIS_PIN_MODE_OUT                   = TRIS_OUTPUT_MODE;
  
	// CONTROL to PLC, original
    
    TRIS_PIN_READY_FOR_ANALOG_OUT  = TRIS_OUTPUT_MODE;
    PIN_READY_FOR_ANALOG_OUT	   = OLL_READY_FOR_ANALOG;
  
	// STATUS from board A35487
    TRIS_PIN_XRAY_CMD_MISMATCH_IN  = TRIS_INPUT_MODE;
    TRIS_PIN_10SEC_IN			   = TRIS_INPUT_MODE;
    
	TRIS_PIN_TRIG_INPUT            = TRIS_INPUT_MODE;
    
    
    // CONTROL to board A35487
    PIN_CUSTOMER_BEAM_ENABLE_IN = !ILL_CUSTOMER_BEAM_ENABLE;
    TRIS_PIN_CUSTOMER_BEAM_ENABLE_IN = TRIS_INPUT_MODE;

    PIN_CUSTOMER_XRAY_ON_IN = !ILL_CUSTOMER_XRAY_ON;
    TRIS_PIN_CUSTOMER_XRAY_ON_IN = TRIS_INPUT_MODE;

    PIN_ENERGY_CPU_OUT = !OLL_ENERGY_CPU;
    TRIS_PIN_ENERGY_CPU_OUT = TRIS_OUTPUT_MODE;

    TRIS_PIN_AFC_TRIGGER_OK_OUT = TRIS_OUTPUT_MODE;
    PIN_AFC_TRIGGER_OK_OUT = OLL_AFC_TRIGGER_OK;

    
    
    PIN_RF_POLARITY_OUT = OLL_RF_POLARITY;
    TRIS_PIN_RF_POLARITY_OUT = TRIS_OUTPUT_MODE;

    PIN_HVPS_POLARITY_OUT = !OLL_HVPS_POLARITY;
    TRIS_PIN_HVPS_POLARITY_OUT = TRIS_OUTPUT_MODE;

    PIN_GUN_POLARITY_OUT = !OLL_GUN_POLARITY;
    TRIS_PIN_GUN_POLARITY_OUT = TRIS_OUTPUT_MODE;
    
    PIN_LD_DELAY_PFN_OUT = 0;
    TRIS_PIN_LD_DELAY_PFN_OUT = TRIS_OUTPUT_MODE;
    
    PIN_LD_DELAY_AFC_OUT = 0;
    TRIS_PIN_LD_DELAY_AFC_OUT = TRIS_OUTPUT_MODE;
    
    PIN_LD_DELAY_GUN_OUT = 0;
    TRIS_PIN_LD_DELAY_GUN_OUT = TRIS_OUTPUT_MODE;
    
   // SPI Interface
    PIN_SPI_CLK_OUT = 0;
    TRIS_PIN_SPI_CLK_OUT = TRIS_OUTPUT_MODE;
    
    PIN_SPI_DATA_OUT = 0;
    TRIS_PIN_SPI_DATA_OUT = TRIS_OUTPUT_MODE;
    
    TRIS_PIN_SPI_DATA_IN = TRIS_INPUT_MODE;


    TRIS_PIN_ENERGY_CMD_IN1 = TRIS_INPUT_MODE;
    TRIS_PIN_ENERGY_CMD_IN2 = TRIS_INPUT_MODE;
    
    TRIS_PIN_40US_IN1		= TRIS_INPUT_MODE;
    TRIS_PIN_40US_IN2		= TRIS_INPUT_MODE;
    
    COMM_DRIVER_ENABLE_TRIS = TRIS_OUTPUT_MODE;
    COMM_DRIVER_ENABLE_PIN = 0;
    COMM_RX_TRIS = TRIS_INPUT_MODE;
    COMM_TX_TRIS = TRIS_OUTPUT_MODE;

}


void InitADC(void)
{
	ADCON1bits.ADSIDL = 1;			//Discontinue ADC in Sleep mode
	ADCON1bits.FORM = 0;			//Integer output (Dout = 0000 dddd dddd dddd)
	ADCON1bits.SSRC = 0x000;		//Clearing SAMP bit ends sampling and starts conversion
	ADCON1bits.ASAM = 0;			//Sampling/Conversion is controlled manually
	ADCON1bits.SAMP = 0;			// 1=Start Sampling, 0=Start Conversion
	ADCON1bits.DONE = 0;			//Status:  1=A/D done

	ADCON2 = 0x0000;
	//ADCON2bits.VCFG = 0x000;		//AVdd and AVss are Vref
	ADCON2bits.CSCNA = 0;			//Do not scan inputs
	ADCON2bits.SMPI = 0;			//Interrupts at end of each conversion
	ADCON2bits.ALTS = 0;			//Always use MUX A, do not alternate between MUX A and B

	ADCON3bits.SAMC = 0x07;			//Auto Sample Time bits (0 = 0Tad)
	ADCON3bits.ADRC = 0;			//Clock derived from system clock
	ADCON3bits.ADCS = 0xAA;                 //A/D conversion clock = AA * Tcy

	ADCHSbits.CH0NA = 0;			//Negative input select = Vref-/AVss
	ADCHSbits.CH0SA = 4;			//Positive input select = AN4/RB4

	ADPCFG = 0x00ff;				//Set ch8 to ch15 for Analog
	//ADPCFGbits.PCFG4 = 0;			//AN4 = Analog
	//ADPCFGbits.PCFG3 = 0;			//AN3 = Analog
	//ADPCFGbits.PCFG2 = 0;			//AN2 = Analog

	ADCSSL = 0;						//Do not scan the analog pins

	IFS0bits.ADIF = 0;				//Clear A/D interrupt flag
	IEC0bits.ADIE = 0;				//Disable A/D interrupts

	ADCON1bits.ADON = 1;			//Turn the ADC ON
}

void LEDCheck(void)
{
	unsigned int y;
	unsigned int z = 0xFFFF;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;
	
	while (y != z)
		y++;

        PIN_LED_WARMUP = OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = OLL_LED_ON;
        PIN_LED_STANDBY = OLL_LED_ON;
	PIN_LED_HVON = OLL_LED_ON;
	PIN_LED_XRAY_ON = OLL_LED_ON;
	PIN_LED_SUMFLT = OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = OLL_LED_ON;
        PIN_LED_STANDBY = OLL_LED_ON;
	PIN_LED_HVON = OLL_LED_ON;
	PIN_LED_XRAY_ON = OLL_LED_ON;
	PIN_LED_SUMFLT = OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = OLL_LED_ON;
        PIN_LED_STANDBY = OLL_LED_ON;
	PIN_LED_HVON = OLL_LED_ON;
	PIN_LED_XRAY_ON = OLL_LED_ON;
	PIN_LED_SUMFLT = OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = !OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;

	y = 0;
	while (y != z)
		y++;

        PIN_LED_WARMUP = OLL_LED_ON;
        PIN_LED_STANDBY = !OLL_LED_ON;
	PIN_LED_HVON = !OLL_LED_ON;
	PIN_LED_XRAY_ON = !OLL_LED_ON;
	PIN_LED_SUMFLT = !OLL_LED_ON;
}


unsigned char GetPersonality(void)
{
      unsigned int data;
      unsigned char i, data1, data2, data3, data4, ret;

      PIN_ID_CLK_OUT   = !OLL_ID_CLK;
      PIN_ID_SHIFT_OUT = !OLL_ID_SHIFT; // load the reg 
      __delay32(1); // 100ns for 10M TCY
      PIN_ID_SHIFT_OUT = OLL_ID_SHIFT;  // enable shift 
      __delay32(1); // 100ns for 10M TCY
      
      data = PIN_ID_DATA_IN;      
      
      for (i = 0; i < 8; i++)
      {
      	PIN_ID_CLK_OUT = OLL_ID_CLK;
        data <<= 1;
        data |= PIN_ID_DATA_IN;
      	PIN_ID_CLK_OUT = !OLL_ID_CLK;
        __delay32(1); // 100ns for 10M TCY
      }   

      //if bits do not match then bad module
      data1 = data & 0x01;
      data2 = data & 0x10;
      if (data1 != (data2 >> 4))
          return 0;
      data1 = data & 0x02;
      data2 = data & 0x20;
      if (data1 != (data2 >> 4))
          return 0;
      data1 = data & 0x04;
      data2 = data & 0x40;
      if (data1 != (data2 >> 4))
          return 0;
      data1 = data & 0x08;
      data2 = data & 0x80;
      if (data1 != (data2 >> 4))
          return 0;

      //bit 3 is 1 except when 0,1,2 are 1
      data1 = data & 0x08;
      data2 = data & 0x07;
      if (data1 != data2)
          return data;

      return 0;
}

int SendPersonalityToPLC(unsigned char id)
{
    //The PIC will use the gun driver,PFN, and RF faults to the PLC
    //as outputs momentarily to tell the PLC which personality
    //module is installed

    int ret = 0;

    TRIS_PIN_CPU_RF_OK_IN = TRIS_OUTPUT_MODE;
    TRIS_PIN_CPU_GUNDRIVER_OK_IN = TRIS_OUTPUT_MODE;
    TRIS_PIN_CPU_PFN_OK_IN = TRIS_OUTPUT_MODE;

    if (id == HIGH_DOSE)
    {
        PIN_CPU_PFN_OK_IN = 0;
        NOP()
        PIN_CPU_GUNDRIVER_OK_IN = 0;
        NOP()
        PIN_CPU_RF_OK_IN = 0;
        NOP()
    }
    else if (id == MEDIUM_DOSE)
    {
        PIN_CPU_PFN_OK_IN = 1;
        NOP()
        PIN_CPU_GUNDRIVER_OK_IN = 1;
        NOP()
        PIN_CPU_RF_OK_IN = 0;
        NOP()
    }
    else if (id == LOW_DOSE)
    {
        PIN_CPU_PFN_OK_IN = 1;
        NOP()
        PIN_CPU_GUNDRIVER_OK_IN = 0;
        NOP()
        PIN_CPU_RF_OK_IN = 1;
        NOP()
    }
   else if (id == ULTRALOW_DOSE)
   {
        PIN_CPU_PFN_OK_IN = 0;
        NOP()
        PIN_CPU_GUNDRIVER_OK_IN = 1;
        NOP()
        PIN_CPU_RF_OK_IN = 1;
        NOP()
   }
   else
   {
        PIN_CPU_PFN_OK_IN = 1;
        NOP()
        PIN_CPU_GUNDRIVER_OK_IN = 1;
        NOP()
        PIN_CPU_RF_OK_IN = 1;
        NOP()
   }

    PIN_READY_FOR_ANALOG_OUT = !OLL_READY_FOR_ANALOG;   //PLCin = 1
    NOP()
    __delay32(DELAY_PLC); // 250ms for 10M TCY
    if (PIN_PACKAGE_VALID_IN == !ILL_PACKAGE_VALID)
        ret = 1;       //Failure to Communicate
    PIN_READY_FOR_ANALOG_OUT = OLL_READY_FOR_ANALOG;    //PLCin = 0
    NOP()
    __delay32(DELAY_PLC); // 250ms for 10M TCY
    if (PIN_PACKAGE_VALID_IN == ILL_PACKAGE_VALID)
        ret = 1;       //Failure to Communicate


    if (id == HIGH_DOSE)
    {
        PIN_CPU_PFN_OK_IN = 1;
        NOP()
        PIN_CPU_GUNDRIVER_OK_IN = 1;
        NOP()
        PIN_CPU_RF_OK_IN = 1;
        NOP()
    }
    else if (id == MEDIUM_DOSE)
    {
        PIN_CPU_PFN_OK_IN = 0;
        NOP()
        PIN_CPU_GUNDRIVER_OK_IN = 0;
        NOP()
        PIN_CPU_RF_OK_IN = 1;
        NOP()
    }
    else if (id == LOW_DOSE)
    {
        PIN_CPU_PFN_OK_IN = 0;
        NOP()
        PIN_CPU_GUNDRIVER_OK_IN = 1;
        NOP()
        PIN_CPU_RF_OK_IN = 0;
        NOP()
    }
   else if (id == ULTRALOW_DOSE)
   {
        PIN_CPU_PFN_OK_IN = 1;
        NOP()
        PIN_CPU_GUNDRIVER_OK_IN = 0;
        NOP()
        PIN_CPU_RF_OK_IN = 0;
        NOP()
   }
   else
   {
        PIN_CPU_PFN_OK_IN = 1;
        NOP()
        PIN_CPU_GUNDRIVER_OK_IN = 1;
        NOP()
        PIN_CPU_RF_OK_IN = 1;
        NOP()
   }

    PIN_READY_FOR_ANALOG_OUT = !OLL_READY_FOR_ANALOG;   //PLCin = 1
    NOP()
    __delay32(DELAY_PLC); // 250ms for 10M TCY
    if (PIN_PACKAGE_VALID_IN == !ILL_PACKAGE_VALID)
        ret = 1;       //Failure to Communicate
    PIN_READY_FOR_ANALOG_OUT = OLL_READY_FOR_ANALOG;    //PLCin = 0
    NOP()
    __delay32(DELAY_PLC); // 250ms for 10M TCY
    if (PIN_PACKAGE_VALID_IN == ILL_PACKAGE_VALID)
        ret = 1;       //Failure to Communicate

    TRIS_PIN_CPU_RF_OK_IN = TRIS_INPUT_MODE;
    TRIS_PIN_CPU_GUNDRIVER_OK_IN = TRIS_INPUT_MODE;
    TRIS_PIN_CPU_PFN_OK_IN = TRIS_INPUT_MODE;

    return ret;       //Communication Successful = 0
}

void InitTimer2(void)
{
      // Configure T2 Interrupt.  This is used to compensate for a walking
      //                            magnetron by walking the gun driver.

    T2CONbits.TCKPS = 0x10;     //64 prescale
    T2CONbits.TCS = 1;          //Enable Tsync from external clock output (10MHz)
    PR2 = 1302;
    TMR2 = 0;
    _T2IF = 0;
    _T2IE = 1;
    _T2IP = 5;

    T2CONbits.TON = 1;

}


 void InitUART()
 {
  // Initialize the UART

  // ----------------- UART #2 Setup and Data Buffer -------------------------//
  // Setup the UART input and output buffers
  BufferByte64Initialize(&uart2_input_buffer);
  BufferByte64Initialize(&uart2_output_buffer);

  //U1MODE = A35997_U1MODE_VALUE;
  // U1STA = A35997_U1STA_VALUE;
  U2BRG = A35997_U2BRG_VALUE;
  U2MODE = 0b0000000000000000;

  // Begin UART operation
  command_string.data_state = COMMAND_BUFFER_EMPTY;  // The command buffer is empty

  _U2TXIF = 0;	// Clear the Transmit Interrupt Flag
  _U2RXIF = 0;	// Clear the Recieve Interrupt Flag
  _U2TXIE = 1;	// Enable Transmit Interrupts
  _U2TXIP = 1;  // Low Priority
  _U2RXIE = 1;	// Enable Recieve Interrupts
  _U2RXIP = 1;  //Low Priority
  U2MODEbits.UARTEN = 1;	// And turn the peripheral on
  U2STA = 0b0000010000000000;   // The U1STA register must be set AFTER the module is enabled for some reason
 }

 void InitINT3()
 {
  // Set up Interrupts
  // Set up external INT3 */
  // This is the trigger interrupt
  _INT3IF = 0;		// Clear Interrupt flag
  _INT3IE = 1;		// Enable INT3 Interrupt
  _INT3EP = 1; 	        // Interrupt on falling edge
  _INT3IP = 7;		// Set interrupt to highest priority
 }

 void InitINT4()
 {
  // Set up Interrupts
  // Set up external INT4 */
  // This is the trigger interrupt
  _INT4IF = 0;		// Clear Interrupt flag
  _INT4IE = 1;		// Enable INT3 Interrupt
  _INT4EP = 0; 	        // Interrupt on falling edge
  _INT4IP = 7;		// Set interrupt to highest priority
 }