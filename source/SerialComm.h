#ifndef __SERIAL_A34405_H
#define __SERIAL_A34405_H

#include "ETM_BUFFER_BYTE_64.h"
#include <uart.h>
#include "main.h"

// Command List
#define CMD_READ_RAM_VALUE                              0x10
#define CMD_READ_TRIGGER_WIDTH_UNFILTERED               0x1F
#define CMD_READ_TRIGGER_WIDTH                          0x20
#define CMD_READ_GRID_HIGH_WIDTH                        0x21
#define CMD_READ_GRID_HIGH_DELAY                        0x22
#define CMD_READ_GRID_LOW_WIDTH                         0x23
#define CMD_READ_GRID_LOW_DELAY                         0x24
#define CMD_PULSES_ON                                   0x30
#define CMD_READ_RF_DELAY_HIGH_ENERGY                   0x40
#define CMD_READ_RF_DELAY_LOW_ENERGY                    0x41
#define CMD_DATA_LOGGING                                0x56
#define CMD_SET_ANALOG                                  0x60
#define CMD_READ_ANALOG                                 0x61


// RAM Locations
#define RAM_READ_STATE                                  0x01



#define COMMAND_BUFFER_EMPTY  0x00
#define COMMAND_BUFFER_FULL   0x02

#define COMMAND_LENGTH        9
#define SYNC_BYTE_1           0xF1
#define SYNC_BYTE_2           0xF2
#define SYNC_BYTE_3_RECEIVE   0xF3
#define SYNC_BYTE_3_SEND      0xF4




struct CommandStringStruct {
  unsigned char command_byte;
  unsigned char register_byte;
  unsigned char data_high_byte;
  unsigned char data_low_byte;
  unsigned char data_state;
};

extern struct CommandStringStruct command_string;
extern BUFFERBYTE64 uart2_input_buffer;
extern BUFFERBYTE64 uart2_output_buffer;
extern unsigned int trig_pulse_width_unfiltered;
extern unsigned int trig_pulse_width;
extern unsigned char grid_width;
extern unsigned char grid_width_high_energy;
extern unsigned char grid_width_low_energy;
extern unsigned char grid_delay;
extern unsigned char grid_delay_high_energy;
extern unsigned char grid_delay_low_energy;
extern unsigned int pulsesON;
extern unsigned char RF_delay;
extern unsigned char RF_delay_high_energy;
extern unsigned char RF_delay_low_energy;
extern unsigned char Analog[33];

void DoSerialCommand(void);

void SendLoggingDataToUart();


/*
  SIGNAL       USB-COM422-PLUS (MALE)  <-------->  A30956-000 (J1 FEMALE)   SIGNAL
  TXD-               1                                   9                  RXD-
  TXD+               2                                   2                  RXD+
  RXD+               3                                   4                  TXD+
  RXD-               4                                   6                  TXD-
  GND                5                                   5                  GND
*/


#endif
