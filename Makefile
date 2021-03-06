# MPLAB IDE generated this makefile for use with GNU make.
# Project: PulseSync.mcp
# Date: Thu May 03 11:43:32 2012

AS = pic30-as.exe
CC = pic30-gcc.exe
LD = pic30-ld.exe
AR = pic30-ar.exe
HX = pic30-bin2hex.exe
RM = rm

PulseSync.hex : PulseSync.cof
	$(HX) "PulseSync.cof"

PulseSync.cof : main.o traps.o Initialize.o
	$(CC) -mcpu=30F6014A "main.o" "traps.o" "Initialize.o" "C:\Firmware\Linac\Pulse_Sync_Board\libp30F6014A-coff.a" -o"PulseSync.cof" -Wl,-Tp30F6014A.gld,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,--defsym=__ICD2RAM=1,-Map="PulseSync.map",--report-mem

main.o : ../../../program\ files/microchip/mplabc30/v3.25/support/generic/h/libpic30.h source/main.h source/Initialize.h ../../../program\ files/microchip/mplabc30/v3.25/include/math.h ../../../program\ files/microchip/mplabc30/v3.25/include/stddef.h ../../../program\ files/microchip/mplabc30/v3.25/include/stdlib.h ../../../program\ files/microchip/mplabc30/v3.25/support/generic/h/dsp.h ../../../program\ files/microchip/mplabc30/v3.25/support/dsPIC30F/h/p30f6014A.h source/main.c
	$(CC) -mcpu=30F6014A -x c -c "source\main.c" -o"main.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

traps.o : ../../../program\ files/microchip/mplabc30/v3.25/support/dsPIC30F/h/p30f6014A.h source/traps.c
	$(CC) -mcpu=30F6014A -x c -c "source\traps.c" -o"traps.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

Initialize.o : source/Initialize.h ../../../program\ files/microchip/mplabc30/v3.25/support/generic/h/libpic30.h source/main.h ../../../program\ files/microchip/mplabc30/v3.25/include/math.h ../../../program\ files/microchip/mplabc30/v3.25/include/stddef.h ../../../program\ files/microchip/mplabc30/v3.25/include/stdlib.h ../../../program\ files/microchip/mplabc30/v3.25/support/generic/h/dsp.h ../../../program\ files/microchip/mplabc30/v3.25/support/dsPIC30F/h/p30f6014A.h source/Initialize.c
	$(CC) -mcpu=30F6014A -x c -c "source\Initialize.c" -o"Initialize.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g -Wall

clean : 
	$(RM) "main.o" "traps.o" "Initialize.o" "PulseSync.cof" "PulseSync.hex"

