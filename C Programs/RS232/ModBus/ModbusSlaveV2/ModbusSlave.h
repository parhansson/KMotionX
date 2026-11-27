/*	ModBusSlave.c is a library for the Dynomotion KFlop board.
	It complies with ModBus protocol, customized for exchanging 
	information between Industrial controllers and the KFlop board.
	
	ModBusSlave.c is a port of the Arduino ModbusSlave by Tim W. Schilling.
	Therefore it is released under the GNU General Public License described below.
	
	Copyright (c) 2012 Tim W. Shilling (www.ShillingSystems.com)
	Arduino Modbus Slave is free software: you can redistribute it and/or modify it under the terms of the 
	GNU General Public License as published by the Free Software Foundation, either version 3 of the License, 
	or (at your option) any later version.

	Arduino Modbus Slave is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
	See the GNU General Public License for more details.

	To get a copy of the GNU General Public License see <http://www.gnu.org/licenses/>.
*/

#ifndef ModBusSlave_h
#define ModBusSlave_h

#define N_MB_RW_REGISTERS 	(0x100)
#define N_MB_RONLY_REGISTERS 	(0x100)

#define PERSIST_RWREG_BLOCK_ADR		80  // persist.UserData array index to pointer-to-ModbusRWRegister
#define PERSIST_RONLYREG_BLOCK_ADR		81  // persist.UserData array index to pointer-to-ModbusRWRegister

#define MBRWReg ((unsigned short *)persist.UserData[PERSIST_RWREG_BLOCK_ADR])
#define MBROnlyReg ((unsigned short *)persist.UserData[PERSIST_RONLYREG_BLOCK_ADR])

// Defines for IPG to access hardware locations in a readable fashion

// discrete
#define IPG_GAS_1_2				ReadBit(1041)		// 1/2
#define IPG_LASER_COM_ERROR		ReadBit(1045)
#define IPG_FOOTSW				ReadBit(1029)
#define IPG_GASOUT1				ReadBit(32)
#define IPG_PURGE_PB			ReadBit(1027)
#define IPG_GASOUT2				ReadBit(33)
#define IPG_RESET				ReadBit(1024)
#define IPG_MANUAL_AUTO			ReadBit(1038)
#define IPG_CW_PULSE			ReadBit(1039)

// int16
#define IPG_PULSE_TIME			MBRWReg[38]
#define IPG_PULSE_LASER_POWER	MBRWReg[40]
#define IPG_FOOT_REOSTAT		MBRWReg[37]
#define IPG_PULSE_PERIOD		MBRWReg[32]
#define IPG_LSR_ON_DLY			MBRWReg[41]
#define IPG_GAS_OFF_DLY			MBRWReg[42]
#define IPG_CW_PULSE_TIME		MBRWReg[36]
#define IPG_SHUTTER_ON_DELAY	MBRWReg[43]
#define IPG_SHUTTER_OFF_DELAY	MBRWReg[44]
#define IPG_NUMBER_OF_PULSES	MBRWReg[35]
#define IPG_CW_LASER_POWER		MBRWReg[39]
#define IPG_CW_PULSE_PERIOD		MBRWReg[37]
#define IPG_CURRENT_SCRN		MBRWReg[47]
#define IPG_RESET_DELAY			MBRWReg[49]

#endif
		