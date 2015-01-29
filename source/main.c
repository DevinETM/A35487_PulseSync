/**********************************************************************
* ETM Electromatic Inc.
* 
* FileName:        main.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC30F6014
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
*	This program utilizes the following:
*
**********************************************************************/

#include <p30f6014A.h>
#include <dsp.h>
#include "Initialize.h"
#include "main.h"
#include "tables.h"
#include <libpic30.h>
#include "SerialComm.h"


//#define TEST_WITHOUT_PLC    1		// don't define this for normal operation



//Global Variable Declaration
unsigned char energy_level;			// 1 = HI, 0 = LOW
//unsigned char MODE;				// 1 = interleaved, 0 = single
unsigned char last_state_40us;		// remember last 40us input
unsigned char current_state_40us;      // hold the current state
#ifndef TEST_WITHOUT_PLC
	unsigned char Analog[33];       //0 = Nothing, Can Not Implement
                                        //1 = High Energy Grid Start 3
                                        //2 = High Energy Grid Start 2
                                        //3 = High Energy Grid Start 1
                                        //4 = High Energy Grid Start 0
                                        //5 = Low Energy Grid Start 3
                                        //6 = Low Energy Grid Start 2
                                        //7 = Low Energy Grid Start 1
                                        //8 = Low Energy Grid Start 0
                                        //9 = High Energy Grid Stop 3
                                        //10 = High Energy Grid Stop 2
                                        //11 = High Energy Grid Stop 1
                                        //12 = High Energy Grid Stop 0
                                        //13 = Low Energy Grid Stop 3
                                        //14 = Low Energy Grid Stop 2
                                        //15 = Low Energy Grid Stop 1
                                        //16 = Low Energy Grid Stop 0
                                        //17 = High Energy PFN Delay
                                        //18 = High Energy RF Delay
                                        //19 = Low Energy PFN Delay
                                        //20 = Low Energy RF Delay
                                        //19 = High Energy AFC Delay
                                        //20 = Not Implemented
                                        //21 = Low Energy AFC Delay
                                        //22 = Not Implemented
#else // test interpolation
	unsigned char Analog[33] = {0, 200, 150, 100, 50, 100, 75, 50, 25, 220, 160, 120, 60, 110, 80, 60, 30,
	                            230, 180, 170, 90, 235, 185, 175, 95};
	#define  TEST_LENGTH   5        
	unsigned test_width[TEST_LENGTH] = { 8, 30, 120, 200, 300};
#endif

unsigned int  trig_pulse_width = 0;  // init to minimum
unsigned int trig_pulse_width_unfiltered;
unsigned char grid_delay;
unsigned char grid_width;
unsigned char grid_width_high_energy;
unsigned char grid_width_low_energy;
unsigned char grid_delay_high_energy;
unsigned char grid_delay_low_energy;
unsigned char RF_delay_high_energy;
unsigned char RF_delay_low_energy;
unsigned char RF_delay;
unsigned int pulsesON;

const unsigned int  dos_intensities[4] = {15, 95, 175, 255};  // fixed constants
//const unsigned int  dos_intensities[4] = {255, 175, 95, 15};  // fixed constants

unsigned int mainloopcounter;       //counts each cycle of the main loop
unsigned int heartbeat;             //Cycles the Standby LED to indicate the following:
                                    // x1 = main loop is healthy
                                    // /5 = Failure Load Personality to PLC

unsigned char personality_id;       //stores the personality module installed
                                    //1001 = Star = Ultra Low Dose
                                    //1010 = Triangle = Low Dose
                                    //1100 = Square = Medium Dose
                                    //0111 = Circle = High Dose
int FlagPersonalityToPLCFailed;        //1 = Failure to send personality info to PLC
int FlagAnalogsFromPLCFailed;

unsigned int trigger_old_Hi;
unsigned int trigger_old_Low;

unsigned int state_trig_stayed_on;


unsigned int compensationCounter;   //counts when xray is on for overshoot compensation
unsigned int pulseCounter;                 //counts the pulses that occured while compensating
unsigned int flagCompensationDone;  //Indicates if the overshoot compensation is complete
unsigned int flagXrayON;            //Flag that indicates whether xrays are on or not
unsigned int flagNotPulsing;        //Signify when the linac is not pulsing

unsigned int previousTriggerWidth;  //Stores the previous pulse width sampled
unsigned int triggerPulseCounter;   //Counts how many pulses have passed to allow a change of trigger width


/* Configuration Bit Settings */
//_FOSC(LPRC & CSW_FSCM_OFF);
_FOSC(EC & CSW_FSCM_OFF); // Primary Oscillator without PLL and Startup with User Selected Oscillator Source, CLKOUT 10MHz is used to measure trigger width.
// DPARKER this is for use with 40MHz ext clock

//_FOSC(FRC_PLL16 & CSW_FSCM_OFF); // Primary Oscillator with PLL and Startup with User Selected Oscillator Source
                                 // External Oscillator is 7.3738MHz with 16x PLL and 4x Clocks per Instruction cycle
                                 // This yields an Instruction Cycle Frequency of 29.4952 MHz
                                 // This yields an Instruction Cycle Time of 33.904nS


// Watchdog Timeout is 2 Millisconds with no pre scallers
//_FWDT(WDT_ON & WDTPSA_1 & WDTPSB_2);  // Watchdog Timer is enabled, 4ms TIMEOUT
//_FWDT(WDT_ON & WDTPSA_1 & WDTPSB_16);  // Watchdog Timer is enabled, 32ms TIMEOUT
_FWDT(WDT_ON & WDTPSA_512 & WDTPSB_2);  // Watchdog Timer is enabled, 1024ms TIMEOUT
//_FWDT(WDT_OFF & WDTPSA_1 & WDTPSB_16);  // Watchdog Timer is disabled, 32ms TIMEOUT


_FBORPOR(PWRT_64 & BORV_27 & PBOR_ON & MCLR_EN); // Brown out and Power on Timer settings
//_FBORPOR(PWRT_OFF & BORV_45 & PBOR_OFF & MCLR_EN); // Brown out and Power on Timer settings

// _FBS()  // No Boot Segment

_FBS(WR_PROTECT_BOOT_OFF & NO_BOOT_CODE & NO_BOOT_EEPROM & NO_BOOT_RAM);

_FSS(WR_PROT_SEC_OFF & NO_SEC_CODE & NO_SEC_EEPROM & NO_SEC_RAM);

_FGS(CODE_PROT_OFF);

_FICD(PGD);


int main(void);
int GetAnalogs(void);
void AcquireData(unsigned int id);
unsigned int SampleAnalogChannel(unsigned int ch);
void ProgramShiftRegisters(void);
void ReadTrigPulseWidth(void);
void FaultsAndLEDs(void);
unsigned int FilterTrigger(unsigned int param);
void ReadEnergyCommand(void);


int main (void)
{
	ClrWdt()
	Initialize();									//Initialize all peripherals/pins
    
        state_trig_stayed_on = 0; 
        FlagPersonalityToPLCFailed = 0;
        personality_id = GetPersonality();
        FlagPersonalityToPLCFailed = SendPersonalityToPLC(personality_id);
        
        compensationCounter = 12;
	//FlagAnalogsFromPLCFailed = GetAnalogs();									//Get initial 32 analog values
	
//	MODE = MODE_CPU_PIN;
	
   /*	if (PIN_ENERGY_CMD_IN1 == HI)
		energy_level = HI;
	else */
	energy_level = LOW;  // keep energy low for the first pulse

	ProgramShiftRegisters();						//Shift out data
    
    last_state_40us = PIN_40US_IN1;

#ifdef TEST_WITHOUT_PLC
    unsigned int i;
    for (i = 0; i < TEST_LENGTH; i++)
    {
    	trig_pulse_width = test_width[i];
		ProgramShiftRegisters();				//Shift out data
    }

#endif

    while (1)               						//MAIN LOOP
    {    
        ClrWdt()							//Clear the Watchdog Timer
        FaultsAndLEDs();
        ClrWdt()

        //For Serial Communication, uncomment out the next line
        //Also uncomment InitUART(); and InitINT3(); in Initialize.c
        DoSerialCommand();
        ClrWdt()

        if (PIN_PACKAGE_VALID_IN == ILL_PACKAGE_VALID) 	//If the PLC wants to send new data
        {
                FlagAnalogsFromPLCFailed = GetAnalogs();

                if (PIN_ENERGY_CMD_IN1 == HI)
                    energy_level = HI;
                else
                    energy_level = LOW;

                ProgramShiftRegisters();
        }

        if (state_trig_stayed_on)
        {
            if (PIN_TRIG_INPUT != ILL_TRIG_ON)
            {
                PIN_PW_HOLD_LOWRESET_OUT = OLL_PW_HOLD_LOWRESET;   // clear reset only when trig pulse is low
                state_trig_stayed_on = 0;
            }
        }
    }
    return 0;
}

int GetAnalogs(void)        //returns 0 if executed ok
{
    unsigned int z;
    int ret = 0;

    PIN_PW_CLR_CNT_OUT = OLL_PW_CLR_CNT;			 // clear active
    PIN_PW_HOLD_LOWRESET_OUT = !OLL_PW_HOLD_LOWRESET;	 // reset start to disable pulse
    NOP()

    
    NOP()
#ifndef TEST_WITHOUT_PLC
	for (z = 0; z < 3; z++)
	{
            PIN_READY_FOR_ANALOG_OUT = !OLL_READY_FOR_ANALOG;   //PLCin = 1
            NOP()
            __delay32(DELAY_PLC); // 250ms for 10M TCY
            if (PIN_PACKAGE_VALID_IN == !ILL_PACKAGE_VALID)
                ret = 1;       //Failure to Communicate
            AcquireData(z);
            PIN_READY_FOR_ANALOG_OUT = OLL_READY_FOR_ANALOG;    //PLCin = 0
            NOP()
            __delay32(DELAY_PLC); // 250ms for 10M TCY
            if (PIN_PACKAGE_VALID_IN == ILL_PACKAGE_VALID)
                ret = 1;       //Failure to Communicate
	}
#endif
    return ret;       //Communication Successful
}

void AcquireData (unsigned int id)
{
    unsigned int x;

    for (x = 8; x < 16; x++)
    {
        Analog[(x-7)+(id*8)] = SampleAnalogChannel(x);
    }

    //Store the difference between the grid pulse width and when to start the trigger current sample

    //Set the RF pulse width delay with respect to the gun pulse stop delay
    //if (id == 2)
    //{
    //    if (Analog[18] > 196)
    //        Analog[18] = Analog[9] + (Analog[18] - 196);
    //    else if (Analog[18] < 196)
    //        Analog[18] = Analog[9] - (196 - Analog[18]);

    //    if (Analog[20] > 197)
    //        Analog[20] = Analog[13] + (Analog[20] - 197);
    //    else if (Analog[20] < 197)
    //        Analog[20] = Analog[13] - (197 - Analog[20]);
    //}

    //Analog[18] = Analog[9] - 9;
    //Analog[20] = Analog[13] - 9;
}

unsigned int SampleAnalogChannel (unsigned int ch)
{	
    unsigned int loop;
    ADCHSbits.CH0SA = ch;
    ADCON1bits.SAMP = 1;						//Start sampling
    for (loop = 0; loop < 10000; loop++)
    {
        NOP()
        NOP()
        NOP()
        NOP()
        NOP()
    }
    ADCON1bits.SAMP = 0;
    while (!ADCON1bits.DONE);
    return (ADCBUF0 >> 4);
}

// calculate the interpolation value
unsigned GetInterpolationValue(unsigned int low_point, unsigned int high_point, unsigned low_value, unsigned high_value, unsigned point)
{
   double dtemp, dslope;
   unsigned int ret = low_value;
   
   if (high_point > low_point)  // high point has to be bigger
   {
   	dslope = ((double)high_value - (double)low_value) / ((double)high_point - (double)low_point);
        dtemp = (double)point - (double)low_point;
        dtemp *= dslope;
        dtemp += low_value;
        ret = (unsigned)dtemp;
   } 
   
   return (ret); 

}
void ProgramShiftRegisters(void)
{
    unsigned int ehi[]  = {4, 3, 2, 1, 12, 11, 10, 9,  18, 17, 22, 21};
    unsigned int elow[] = {8, 7, 6, 5, 16, 15, 14, 13, 20, 19, 24, 23};
    unsigned int p;
    unsigned int q;
    unsigned long temp;
    unsigned long bittemp;
    
    unsigned int delay[4], width[4];  // delay or width from intensity 0 to 3
    
    PIN_PW_CLR_CNT_OUT = OLL_PW_CLR_CNT;			 // clear width count
    NOP()
    PIN_PW_HOLD_LOWRESET_OUT = !OLL_PW_HOLD_LOWRESET;	 // reset start to disable pulse
    NOP()

    // find breakpoints for the dose
    for (p = 0; p < 4; p++)
    {
        if (energy_level == HI)
        {
            delay[p] = Analog[ehi[p]];
            width[p] = Analog[ehi[p + 4]];
        }
	else
        {
            delay[p] = Analog[elow[p]];
            width[p] = Analog[elow[p + 4]];
        }
    }
    // do inteplation for grid delay and grid width
    for (p = 0; p < 4; p++)
    {
    	if (trig_pulse_width <= dos_intensities[p]) break;
    }
    
    if (p == 0)
    {
    	grid_delay = delay[0];
        grid_width = width[0];
    }
    else if (p >= 4)
    {
    	grid_delay = delay[3];
        grid_width = width[3];
    }
    else // interpolation
    {
    	grid_delay = GetInterpolationValue(dos_intensities[p - 1], dos_intensities[p], delay[p - 1], delay[p], trig_pulse_width);
    	grid_width = GetInterpolationValue(dos_intensities[p - 1], dos_intensities[p], width[p - 1], width[p], trig_pulse_width);
    }

    if (energy_level == HI)
    {
        grid_width_high_energy = grid_width;
        grid_delay_high_energy = grid_delay;
    }
    else
    {
        grid_width_low_energy = grid_width;
        grid_delay_low_energy = grid_delay;
    }

    for (p = 0; p < 6; p++)
    {
    	if (p == 0)
        { // grid delay or width
            temp = grid_width;
        }
        else if (p == 1)
        {
            temp = grid_delay;
        }
        else if (p == 2)    //Set the RF (target current) trigger delay to follow the gun stop delay
        {
            if (energy_level == HI)
            {
                RF_delay_high_energy = grid_width_high_energy - 9;
                temp = RF_delay_high_energy;
            }
            else
            {
                RF_delay_low_energy = grid_width_low_energy - 9;
                temp = RF_delay_low_energy;
            }

            //Set the RF pulse width delay with respect to the gun pulse stop delay
            //if (id == 2)
            //{
            //    if (Analog[18] > 196)
            //        Analog[18] = Analog[9] + (Analog[18] - 196);
            //    else if (Analog[18] < 196)
            //        Analog[18] = Analog[9] - (196 - Analog[18]);

            //    if (Analog[20] > 197)
            //        Analog[20] = Analog[13] + (Analog[20] - 197);
            //    else if (Analog[20] < 197)
            //        Analog[20] = Analog[13] - (197 - Analog[20]);
            //}
        }
        else
        {
            if (energy_level == HI)
                temp = Analog[ehi[p + 6]];
            else
                temp = Analog[elow[p + 6]];
        }

        for (q = 0; q < 8; q++)
        {
            PIN_SPI_CLK_OUT = 0;
            NOP()

            bittemp = temp & 0x80;
            temp = temp << 1;

            if (bittemp == 0x80)
            {
                PIN_SPI_DATA_OUT = 1;
                NOP()
            }
            else
            {
                PIN_SPI_DATA_OUT = 0;
                NOP()
            }

            PIN_SPI_CLK_OUT = 1;
            NOP()
        }

        if (p == 1)						//Latch Gun delay and width data into shift registers
        {
            PIN_LD_DELAY_GUN_OUT = 0;
            NOP()
            PIN_LD_DELAY_GUN_OUT = 1;
            NOP()
        }
        else if (p == 3)				//Latch PFN/RF delay data into shift registers
        {
            PIN_LD_DELAY_PFN_OUT = 0;
            NOP()
            PIN_LD_DELAY_PFN_OUT = 1;
            NOP()
        }
        else if (p == 5)				//Latch AFC/Dose delay data into shift registers
        {
            PIN_LD_DELAY_AFC_OUT = 0;
            NOP()
            PIN_LD_DELAY_AFC_OUT = 1;
            NOP()
        }
    }

    PIN_PW_CLR_CNT_OUT = !OLL_PW_CLR_CNT;			 // enable width count
    NOP()
    if (PIN_TRIG_INPUT != ILL_TRIG_ON)
    {
    	PIN_PW_HOLD_LOWRESET_OUT = OLL_PW_HOLD_LOWRESET;   // clear reset only when trig pulse is low
    	state_trig_stayed_on = 0;        
    }
    else
    	state_trig_stayed_on = 1;
        
    NOP()

}


void ReadTrigPulseWidth(void)
{
      unsigned int data;
      unsigned char i;

      if (pulsesON > 65000)
          pulsesON = 0;
      else
        pulsesON++;

      PIN_SPI_CLK_OUT  = 0;
      NOP()
      PIN_PW_SHIFT_OUT = !OLL_PW_SHIFT; // load the reg
      NOP()
      __delay32(1); // 100ns for 10M TCY
      PIN_PW_SHIFT_OUT = OLL_PW_SHIFT;  // enable shift
      NOP()
      __delay32(1); // 100ns for 10M TCY
      
      data = PIN_SPI_DATA_IN;      
      
      for (i = 0; i < 8; i++)
      {
      	PIN_SPI_CLK_OUT = 1;
        NOP()
        data <<= 1;
        data |= PIN_SPI_DATA_IN;
      	PIN_SPI_CLK_OUT = 0;
        NOP()
        __delay32(1); // 100ns for 10M TCY
      }   

      PIN_PW_SHIFT_OUT = !OLL_PW_SHIFT; // make load active when idle
      NOP()

      if (data & 0x0100)  // counter overflow
      {
        trig_pulse_width = 255;
      }
      else
      {
        trig_pulse_width = data & 0xff;
      }
      trig_pulse_width_unfiltered = trig_pulse_width;
      trig_pulse_width = FilterTrigger(trig_pulse_width);

      if (trig_pulse_width < 245)   //signify to pfn control board what mode to expect
          PIN_MODE_OUT = OLL_MODE_PORTAL;   //so it can use a different target
      else                                  //current setpoint for low energy
          PIN_MODE_OUT = OLL_MODE_GANTRY;

}

void FaultsAndLEDs(void)
{
    mainloopcounter++;

    if (mainloopcounter > 10000)
    {
        mainloopcounter = 0;
        heartbeat++;
        if (FlagPersonalityToPLCFailed || FlagAnalogsFromPLCFailed)
        {
            if (heartbeat > 10)
            {
                if (PIN_LED_STANDBY) 
                    PIN_LED_STANDBY = 0;
                else 
                    PIN_LED_STANDBY = 1;
                //PIN_LED_STANDBY = !PIN_LED_STANDBY;
                //NOP()
                if (FlagPersonalityToPLCFailed)
                    FlagPersonalityToPLCFailed = SendPersonalityToPLC(personality_id);
                //if (FlagAnalogsFromPLCFailed)
                //    FlagAnalogsFromPLCFailed = GetAnalogs();
                
                heartbeat = 0;
            }
        }
        else
        {
            if (PIN_LED_STANDBY)
                PIN_LED_STANDBY = 0;
            else
                PIN_LED_STANDBY = 1;
//            PIN_LED_STANDBY = !PIN_LED_STANDBY;
            NOP()
            heartbeat = 0;
        }
    }

    if (PIN_CPU_WARMUP_IN)
        PIN_LED_WARMUP = 1;
    else
        PIN_LED_WARMUP = 0;

    if (PIN_CPU_READY_IN)
        PIN_LED_HVON = 1;
    else
        PIN_LED_HVON = 0;

    if (PIN_CUSTOMER_XRAY_ON_IN)
        flagXrayON = 1;
    else
        flagXrayON = 0;

    if (PIN_CUSTOMER_XRAY_ON_IN)
        PIN_LED_XRAY_ON = 0;
    else
        PIN_LED_XRAY_ON = 1;

    if (PIN_LED_SUMFLT)
        PIN_CPU_SUMFLT_IN = 1;
    else
        PIN_CPU_SUMFLT_IN = 0;

//    PIN_LED_WARMUP = PIN_CPU_WARMUP_IN;
//    NOP()
//    PIN_LED_HVON = PIN_CPU_READY_IN;
//    NOP()
//    flagXrayON = PIN_CPU_XRAY_ENABLE_IN;
//    NOP()
//    PIN_LED_XRAY_ON = !PIN_CPU_XRAY_ENABLE_IN;
//    NOP()
//    PIN_CPU_SUMFLT_IN = PIN_LED_SUMFLT;
//    NOP()
}

unsigned int FilterTrigger(unsigned int param)
{
    unsigned int trigger_difference;
    int x;
    //For troubleshooting purposes.  Force a constant grid delay and width
    //grid_delay = 0x150;
    //grid_width = 0x50;
    /*if (energy_level == HI)
    {
        //Part of Hysteresis
        if (trigger_old_Hi > param)
            trigger_difference = trigger_old_Hi - param;
        else
            trigger_difference = param - trigger_old_Hi;
    }
    else
    {
        if (trigger_old_Low > param)
            trigger_difference = trigger_old_Low - param;
        else
            trigger_difference = param - trigger_old_Low;
    }*/

    //Establish Dose Levels to reduce jitter and provide consistent dose vs trigger width
    //Every bit represents 20ns pulse width change on the electron gun
    //Every bit also represents a 200ns pulse width change from the customer
    if (param > (DOSE_LEVELS - 1))
    {
        for (x = 0; x <= (param % DOSE_LEVELS); x++)
            param--;

        /*if ((param % DOSE_LEVELS) == 1)
            param--;
        else if ((param % DOSE_LEVELS) == 2)
            param -= 2;
        else if ((param % DOSE_LEVELS) == 3)
            param -= 3;
        else if ((param % DOSE_LEVELS) == 4)
            param -= 4;
        else if ((param % DOSE_LEVELS) == 5)
            param -= 5;
        else if ((param % DOSE_LEVELS) == 5)
            param -= 5;*/
    }
    else
        param = 0;

    //Ensure that at least 15 of the same width pulses in a row actually change the sampled width
    if (param != previousTriggerWidth)
    {
        triggerPulseCounter++;
        if (triggerPulseCounter < 15)
            param = previousTriggerWidth;
        else
            previousTriggerWidth = param;
    }
    else
    {
        triggerPulseCounter = 0;
    }

    //Hysteresis.  Reduce dithering of timing parameters
    /*if (energy_level == HI)
    {
        if (trigger_old_Hi != param)
        {
            if (trigger_difference < HYSTERESIS)
                param = trigger_old_Hi;
            else
                trigger_old_Hi = param;
        }
    }
    else
    {
        if (trigger_old_Low != param)
        {
            if (trigger_difference < HYSTERESIS)
                param = trigger_old_Low;
            else
                trigger_old_Low = param;
        }
    }*/
    return param;
}

/*
  The T2 interrupt adds an interrupt every 4.6ms
  This time is used to correct the dose overshoot
  that has the symptom of the magnetron pulse walking
  with respect to the gun driver pulse.  This occurs
  in the thyratron trigger board
*/
/*void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
  _T2IF = 0;
  TMR2 = 0;


}*/

//void __attribute__((interrupt, shadow, no_auto_psv)) _INT3Interrupt(void)
void __attribute__((interrupt(__save__(CORCON,SR)), no_auto_psv)) _INT3Interrupt(void)
{
    ClrWdt()

    if (PIN_TRIG_INPUT != ILL_TRIG_ON)
    {
        ReadTrigPulseWidth();
        //__delay32(150);         //15us delay
        ReadEnergyCommand();
    }
    else  // if pulse trig stays on, set to minimum dose
    {
        trig_pulse_width = 0;
        state_trig_stayed_on = 1;

    }
    ProgramShiftRegisters();

    _INT3IF = 0;		// Clear Interrupt flag
}


//void __attribute__((interrupt(__save__(CORCON,SR)), no_auto_psv)) _INT4Interrupt(void)
//{
//
//    _INT4IF = 0;		// Clear Interrupt flag
//}


void ReadEnergyCommand()
{
    if ((PIN_LOW_MODE_IN == HI) && (PIN_HIGH_MODE_IN == HI))
    {
        if (PIN_ENERGY_CMD_IN1 == HI)
        {
            NOP()
            PIN_AFC_TRIGGER_OK_OUT = OLL_AFC_TRIGGER_OK;    //Trigger the AFC in high energy only
            NOP()
            PIN_GUN_POLARITY_OUT = !OLL_GUN_POLARITY;
            NOP()
            PIN_ENERGY_CPU_OUT = !OLL_ENERGY_CPU;
            NOP()
            energy_level = HI;
        }
        else
        {
            NOP()
            PIN_AFC_TRIGGER_OK_OUT = !OLL_AFC_TRIGGER_OK;   //Do not trigger the AFC in low energy
            NOP()            
            PIN_GUN_POLARITY_OUT = !OLL_GUN_POLARITY;
            NOP()        
            PIN_ENERGY_CPU_OUT = OLL_ENERGY_CPU;
            NOP()
            energy_level = LOW;
        }
    }
    else
    {
        if (PIN_HIGH_MODE_IN == HI)
        {
            NOP()
            PIN_AFC_TRIGGER_OK_OUT = OLL_AFC_TRIGGER_OK;    //Trigger the AFC in single energy mode
            NOP()
            PIN_GUN_POLARITY_OUT = OLL_GUN_POLARITY;
            NOP()
            PIN_ENERGY_CPU_OUT = OLL_ENERGY_CPU;
            NOP()
            energy_level = LOW;
        }
        else
        {
            NOP()
            PIN_AFC_TRIGGER_OK_OUT = OLL_AFC_TRIGGER_OK;    //Trigger the AFC in single energy mode
            NOP()
            PIN_GUN_POLARITY_OUT = OLL_GUN_POLARITY;
            NOP()    
            PIN_ENERGY_CPU_OUT = !OLL_ENERGY_CPU;            
            NOP()  
            energy_level = HI;
        }
    }
}