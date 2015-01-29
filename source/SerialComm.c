#include <p30fxxxx.h>
#include "SerialComm.h"
#include "Main.h"

/*
  Serial Commands

  A single command is stored in command_string
  If there is a valid command stored in command_string, then the command_string.data_state = COMMAND_BUFFER_FULL
  If there is NOT a volid command stored in command_string, then command_string.data_state = COMMAND_BUFFER_EMPTY


  When a byte is received on the UART it is transfered to the "uart1_input_buffer" by the UART receive interrupt - the input buffer is a circular buffer that is 64 Bytes deep
  (see buffer64.h for more infor on the buffer)

  Every time through the command loop (200us to 1ms) DoSerialCommand() will be called
  If the command_string is empty, then the input buffer is searched for a valid command (the oldest valid command will be moved to command_string)

  If a command was found OR the command_string was already full, then the command is executed.

  Assume an average execution cycle of 1mS and 9 bytes per command.  A command rate of 72 K Baund can be sustained. (57.6 K Baud Standard will work)

  Assume an average execution cycle of 500uS and 9 bytes per command, A command rate of 144 K Baud can be sustained (115.2 K Baud Standard should be safe)

*/

void LookForCommand(void);
void ExecuteCommand(void);
unsigned char CheckCRC(unsigned int crc);
unsigned int MakeCRC(unsigned char command_byte, unsigned char register_byte, unsigned int data_word);
unsigned int ReadFromRam(unsigned int ram_location);
void SendCommand(unsigned char command_byte, unsigned char register_byte, unsigned int data_word);

struct CommandStringStruct command_string;
BUFFERBYTE64 uart2_input_buffer;
BUFFERBYTE64 uart2_output_buffer;

unsigned int *ram_pointer;

unsigned char data_logging_to_uart = 0;

void DoSerialCommand(void) {
  /*
     Look for a command and execute it.
  */
    if (U2STAbits.TRMT == 1)  //Last transmit completed
        COMM_DRIVER_ENABLE_PIN = 0;  //Enable receive driver and disable transmit driver


  if (command_string.data_state != COMMAND_BUFFER_FULL) {
    LookForCommand();
  }

  if (command_string.data_state == COMMAND_BUFFER_FULL) {
      COMM_DRIVER_ENABLE_PIN = 1;
      ExecuteCommand();
  }

}



void LookForCommand(void) {
  unsigned char read_byte;
  unsigned int crc;
  /*
    If the state is "waitng for command" then it looks for a command in the buffer, if the state is "executing command" it does nothing

    To look for a command in the buffer.
    1) See if there are enough bytes in the buffer to contain a command.
    2) If there are look for command sync
       2b) If there are less bytes in the buffer than it takes to make a command, exit
    3) If command Syncs, check the checksum ^ increment the read_position as each byte is read
       3b) If command does not sync, increment the the read positon and return to step 1
    4) If the checksum checks out, move the command data into the command data structure
    4b) If the checksum fails, return to step 1
  */

  while ((command_string.data_state == COMMAND_BUFFER_EMPTY) && (BufferByte64BytesInBuffer(&uart2_input_buffer) >= COMMAND_LENGTH)) {
    // Look for a command
    read_byte = BufferByte64ReadByte(&uart2_input_buffer);
    if (read_byte == SYNC_BYTE_1) {
      read_byte = BufferByte64ReadByte(&uart2_input_buffer);
      if (read_byte == SYNC_BYTE_2) {
	read_byte = BufferByte64ReadByte(&uart2_input_buffer);
	if (read_byte == SYNC_BYTE_3_RECEIVE) {
	  // All of the sync bytes matched, this should be a valid command
	  command_string.command_byte   = BufferByte64ReadByte(&uart2_input_buffer);
	  command_string.data_high_byte = BufferByte64ReadByte(&uart2_input_buffer);
	  command_string.data_low_byte  = BufferByte64ReadByte(&uart2_input_buffer);
	  command_string.register_byte  = BufferByte64ReadByte(&uart2_input_buffer);
	  crc                           = BufferByte64ReadByte(&uart2_input_buffer);
	  crc                           = (crc << 8) + BufferByte64ReadByte(&uart2_input_buffer);
	  if (CheckCRC(crc)) {
	    command_string.data_state = COMMAND_BUFFER_FULL;
	  }
	}
      }
    }
  }
}

void SendLoggingDataToUart() {
  unsigned char byte;
  if (data_logging_to_uart) {
    //BufferByte64WriteByte(&uart2_output_buffer, 0xFE);
    //BufferByte64WriteByte(&uart1_output_buffer, (afc_motor.current_position >> 8));
    //BufferByte64WriteByte(&uart1_output_buffer, (afc_motor.current_position & 0x00FF));
    //BufferByte64WriteByte(&uart1_output_buffer, (afc_motor.target_position >> 8));
    //BufferByte64WriteByte(&uart1_output_buffer, (afc_motor.target_position & 0x00FF));

    //BufferByte64WriteByte(&uart1_output_buffer, afc_data.frequency_error_filtered);
    //BufferByte64WriteByte(&uart1_output_buffer, (afc_data.pulses_on >> 8));
    //BufferByte64WriteByte(&uart1_output_buffer, (afc_data.pulses_on & 0x00FF));
    if ((!U2STAbits.UTXBF) && (BufferByte64IsNotEmpty(&uart2_output_buffer))) {
      /*
	There is at least one byte available for writing in the outputbuffer and the transmit buffer is not full.
	Move a byte from the output buffer into the transmit buffer
	All subsequent bytes will be moved from the output buffer to the transmit buffer by the U1 TX Interrupt
      */
      U2TXREG = BufferByte64ReadByte(&uart2_output_buffer);
    }
  }
}

void SendCommand(unsigned char command_byte, unsigned char register_byte, unsigned int data_word) {
  unsigned int crc;
  if (!data_logging_to_uart) {
    crc = MakeCRC(command_byte, register_byte, data_word);
    BufferByte64WriteByte(&uart2_output_buffer, SYNC_BYTE_1);
    BufferByte64WriteByte(&uart2_output_buffer, SYNC_BYTE_2);
    BufferByte64WriteByte(&uart2_output_buffer, SYNC_BYTE_3_SEND);
    BufferByte64WriteByte(&uart2_output_buffer, command_byte);
    BufferByte64WriteByte(&uart2_output_buffer, (data_word >> 8));
    BufferByte64WriteByte(&uart2_output_buffer, (data_word & 0x00FF));
    BufferByte64WriteByte(&uart2_output_buffer, register_byte);
    BufferByte64WriteByte(&uart2_output_buffer, (crc >> 8));
    BufferByte64WriteByte(&uart2_output_buffer, (crc & 0x00FF));

    if ((!U2STAbits.UTXBF) && (BufferByte64IsNotEmpty(&uart2_output_buffer))) {
      /*
	There is at least one byte available for writing in the outputbuffer and the transmit buffer is not full.
	Move a byte from the output buffer into the transmit buffer
	All subsequent bytes will be moved from the output buffer to the transmit buffer by the U1 TX Interrupt
      */
      U2TXREG = BufferByte64ReadByte(&uart2_output_buffer);
    }
  }
}


void ExecuteCommand(void) {
  unsigned int data_word;
  unsigned int return_data_word;
  unsigned int return_command_byte;

  data_word = command_string.data_high_byte;
  data_word = data_word << 8;
  data_word = data_word + command_string.data_low_byte;

  return_data_word = data_word;
  return_command_byte = command_string.command_byte;
  switch (command_string.command_byte)
    {

    case CMD_READ_TRIGGER_WIDTH_UNFILTERED:
      return_data_word = trig_pulse_width_unfiltered;
      break;

    case CMD_READ_TRIGGER_WIDTH:
        return_data_word = trig_pulse_width;
        break;

    case CMD_READ_GRID_HIGH_WIDTH:
        return_data_word = grid_width_high_energy;
        break;

    case CMD_READ_GRID_HIGH_DELAY:
        return_data_word = grid_delay_high_energy;
        break;

    case CMD_READ_GRID_LOW_WIDTH:
        return_data_word = grid_width_low_energy;
        break;

    case CMD_READ_GRID_LOW_DELAY:
        return_data_word = grid_delay_low_energy;
        break;

    case CMD_PULSES_ON:
        return_data_word = pulsesON;
        break;

    case CMD_READ_RF_DELAY_HIGH_ENERGY:
        return_data_word = RF_delay_high_energy;
        break;

    case CMD_READ_RF_DELAY_LOW_ENERGY:
        return_data_word = RF_delay_low_energy;
        break;

      case CMD_SET_ANALOG:
          Analog[command_string.register_byte] = command_string.data_low_byte;
          return_data_word = (int)command_string.data_low_byte;
          break;

      case CMD_READ_ANALOG:
          return_data_word = (int)Analog[command_string.register_byte];
          break;

      default:
          return_data_word = 23;

    }

  // Echo the command that was recieved back to the controller
  SendCommand(return_command_byte, command_string.register_byte, return_data_word);
  command_string.data_state = COMMAND_BUFFER_EMPTY;
}



unsigned int ReadFromRam(unsigned int ram_location) {
  unsigned int data_return;

  switch (ram_location)
    {

    case RAM_READ_STATE:
      data_return = 69;
      break;




    }

  return data_return;
}


unsigned int MakeCRC(unsigned char command_byte, unsigned char register_byte, unsigned int data_word) {
  unsigned int crc;
  crc = SYNC_BYTE_1 + SYNC_BYTE_2 + SYNC_BYTE_3_SEND;
  crc += command_byte + register_byte;
  crc += (data_word >> 8);
  crc += (data_word & 0x00FF);

  return crc;
  // DPAKRER Make real CRC
}


unsigned char CheckCRC(unsigned int crc) {
  unsigned int crcCheck;
  // At the moment the CRC is just a checksum
  crcCheck = SYNC_BYTE_1 + SYNC_BYTE_2 + SYNC_BYTE_3_RECEIVE;
  crcCheck += command_string.command_byte + command_string.register_byte;
  crcCheck += command_string.data_high_byte + command_string.data_low_byte;
  if (crcCheck == crc) {
    return 1;
  } else {
    return 0;
  }
  // DPARKER make Real CRC

}



//void _ISRNOPSV _U1RXInterrupt(void) {
void __attribute__((interrupt(__save__(CORCON,SR)),no_auto_psv)) _U2RXInterrupt(void) {
  _U2RXIF = 0;
  while (U2STAbits.URXDA) {
    BufferByte64WriteByte(&uart2_input_buffer, U2RXREG);
  }
}



//void _ISRNOPSV _U1TXInterrupt(void) {
void __attribute__((interrupt(__save__(CORCON,SR)),no_auto_psv)) _U2TXInterrupt(void) {
  _U2TXIF = 0;
  while ((!U2STAbits.UTXBF) && (BufferByte64BytesInBuffer(&uart2_output_buffer))) {
    /*
      There is at least one byte available for writing in the outputbuffer and the transmit buffer is not full.
      Move a byte from the output buffer into the transmit buffer
    */
    U2TXREG = BufferByte64ReadByte(&uart2_output_buffer);
  }
}


