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

#include "KMotionDef.h"
#include "ModBusSlave.h"

int CheckSum;	// From Checksum Library, CRC15.h, CRC16.cpp

#define HighByteLowByte(H,L)  (((H)<<8)+(L))

unsigned short MBRWRegisters[N_MB_RW_REGISTERS];
unsigned short MBROnlyRegisters[N_MB_RONLY_REGISTERS];

int	Slave = 1;  // default device address, <=247
int	Error = 0;

enum
{
	MBERROR_NONE = 0,
	// Modbus codes; reported with Modbus error packet
	MBERROR_ILLEGAL_FUNCTION = 1,
	MBERROR_ILLEGAL_DATA_ADDRESS = 2,	// used
	MBERROR_ILLEGAL_DATA_VALUE = 3,	// used
	MBERROR_SLAVE_DEVICE_FAILURE = 4,
	MBERROR_ACKNOWLEDGE = 5,
	MBERROR_SLAVE_DEVICE_BUSY = 6,
	MBERROR_NEGATIVE_ACKNOWLEDGE = 7,
	MBERROR_MEMORY_PARITY_ERROR = 8,
	// internal codes; No response to master
	INTERROR_WRONG_DEVICE = 9,
	INTERROR_CHECKSUM = 10,
} MBErrors;

typedef struct _MemRange
{
	int MBAddrStart;
	int MBLength;
	void *KFlopAddr;
} MemRange;

MemRange ReadWriteReg[]= // Modbus 4xxxx, function 0x03,0x06,0x10
{
	{0x0010,  0x01,  &VirtualBits},	// cmore micro range, 40017 discrete
	{0x0040,  0x02,  &VirtualBitsEx},	// cmore micro range, 40065,40066 discrete
	{0x0500,  0x40,  &MBRWRegisters[0x10]},	// cmore micro range, int16 range
		
	// expose all of the virtual bits as registers at their KFlop locations
	{0x0002,  N_VIRTUAL_BITS/16,  &VirtualBits},
	{0x0040,  N_VIRTUAL_BITS_EX/16,  &VirtualBitsEx},
	// and expose 256 ints that can be accessed in other threads
	{0x0100,  N_MB_RW_REGISTERS,  &MBRWRegisters},
	{0,0,0} // end flag
};
 
MemRange ReadOnlyReg[]= // Modbus 3xxxx, function 0x04
{
	// expose all of the virtual bits as registers at their KFlop locations
	{0x0002,  N_VIRTUAL_BITS/16,  &VirtualBits},
	{0x0040,  N_VIRTUAL_BITS_EX/16,  &VirtualBitsEx},
	// and expose 256 ints that can be accessed in other threads
	{0x0100,  N_MB_RONLY_REGISTERS,  &MBROnlyRegisters},
	{0,0,0} // end flag
};

MemRange ReadWriteCoils[]= // Modbus 0xxxx, function 0x01, 0x05, 0x0F
{
	// expose all of the virtual bits as coils at their KFlop locations
	{0x0020,  N_VIRTUAL_BITS,  &VirtualBits},
	{0x0400,  N_VIRTUAL_BITS_EX,  &VirtualBitsEx},
	{0,0,0} // end flag
};
 
MemRange ReadOnlyCoils[]= // Modbus 1xxxx, function 0x02
{
	// expose all of the virtual bits as coils at their native locations
	{0x0020,  N_VIRTUAL_BITS,  &VirtualBits},
	{0x0400,  N_VIRTUAL_BITS_EX,  &VirtualBitsEx},
	{0,0,0} // end flag
};
 

//################## RangeConvert ###################
// Takes:   Modbus address and count, with memory range list
// Returns: 0=OK, or error code
//          Address of read/write first destination, and offset
// Effect:  translates Modbus ranges to internal memory ranges
int RangeConvert(int MBAdr,int MBCount,MemRange *memrng, void **KFAdr, int *offset)
{
	int i=0;
	char *rngstr;
	if (ReadWriteReg==memrng) rngstr="ReadWriteReg";
	if (ReadOnlyReg==memrng) rngstr="ReadOnlyReg";
	if (ReadWriteCoils==memrng) rngstr="ReadWriteCoils";
	if (ReadOnlyCoils==memrng) rngstr="ReadOnlyCoils";
	for (i=0;memrng[i].KFlopAddr;i++)
	{
		//d printf ("i=%d %04X:%04X -> %06X\n",i,memrng[i].MBAddrStart,memrng[i].MBLength,memrng[i].KFlopAddr); //debug
		if (MBAdr>=memrng[i].MBAddrStart && MBAdr+MBCount<=memrng[i].MBAddrStart+memrng[i].MBLength) // "-1" removed in second comparisons
		{
			*KFAdr=memrng[i].KFlopAddr;
			*offset=MBAdr-memrng[i].MBAddrStart;
			//d printf ("Range %s %04X:%04X -> %06X+%04X\n",rngstr,MBAdr&0xFFFF,MBCount&0xFFFF,*KFAdr,*offset); //debug
			return MBERROR_NONE;
		}
	}
	printf ("Unknown Range: %s %04X:%04X\n",rngstr,MBAdr,MBCount); //debug
	return MBERROR_ILLEGAL_DATA_ADDRESS;
}


//################## Process Data ###################
// Takes:   Data stream buffer from serial port, number of characters to read
// Returns: Nothing
// Effect:  Reads in and parses data

int Process_Data(unsigned char *Buffer, unsigned char Count)
{
	Count--;									// Convert Byte count to byte index;
	unsigned char Slave_Address = Buffer[0];	// Grab Slave Address out of buffer
	if(Slave_Address != 0) 						// Slave = 0 for Broadcast Msg
	{
		if(Slave != Slave_Address)  				// If Msg is not for this Slave
			return INTERROR_WRONG_DEVICE;
	}
	unsigned char Function = Buffer[1];									// Grab Function Code
	unsigned short CRC = (((Buffer[Count]<<8)&0xFF00)|(Buffer[Count-1]&0xFF));			// Transmitted CRC
	unsigned short Recalculated_CRC = CRC16(Buffer,Count-1);	// Computer CRC
	if(Recalculated_CRC != CRC)											// Compare CRC, if not equal, then...
		return INTERROR_CHECKSUM;

	//d printf("Packet %d\n",Function);    //debug

	Error = 0;
	if(Function == 1)													//Read Coils
	{
		Error = Read_Coils(Buffer,ReadWriteCoils);
	}
	if(Function == 2)													//Read D1screte Input
	{
		Error = Read_Coils(Buffer,ReadOnlyCoils);
	}
	if(Function == 3)													//Read Reg
	{
		Error = Read_Regs(Buffer,ReadWriteReg);
	}
	if(Function == 4)													//Read Input Reg
	{
		Error = Read_Regs(Buffer,ReadOnlyReg);
	}
	if(Function == 5)													//Write Single Coil
	{
		Error = Write_Single_Coil(Buffer,ReadWriteCoils);
	}
	if(Function == 6)													//Write Single Reg
	{
		Error = Write_Single_Reg(Buffer,ReadWriteReg);
	}
	if(Function == 7)													//Read Exception Status
	{
		Error = Read_Exception(Buffer);
	}
	if(Function == 15)													//Write Coils
	{
		Error = Write_Coils(Buffer,ReadWriteCoils);
	}
	if(Function == 16)													//Write Reg
	{
		Error = Write_Regs(Buffer,ReadWriteReg);
	}
	
	if (MBERROR_ILLEGAL_DATA_ADDRESS==Error||MBERROR_ILLEGAL_DATA_VALUE==Error)
	{
		Buffer[1]|=0x80;
		Buffer[2]=Error;
		Send_Response(Buffer,3);
	}
	
	Error = 0;															//We made it to the end, Set Error to 0
}

//################## Read Exception ###################
// Takes:   In Data Buffer
// Returns: 0=OK, or error code
// Effect:  Sets Reply Data and sends Response
int Read_Exception(unsigned char *Data_In)
{
	Data_In[2] = Error;
	Send_Response(Data_In,3);
	return 0;
}

//################## Send Response ###################
// Takes:   In Data Buffer, Length
// Returns: 0=OK, or error code
// Effect:  Sends Response over serial port
void Send_Response(unsigned char *Data_In,unsigned short Length)
{
	if(Data_In[0] == 0) 										// If Broadcast Msg, then no reply	
		return;
	if(Length > 0)												// If there is Data to be sent, then...
	{
		unsigned short MyCRC = CRC16(Data_In,Length);		// Calculate new CRC
		Data_In[Length++] = MyCRC & 0x00FF;							// Load lower byte into Buffer
		Data_In[Length++] = MyCRC >> 8;								// Load upper byte into Buffer
		int C;
		//d printf("\nSend ");
		for(C = 0; C < Length;C++)								// Write Data
		{
			RS232_PutChar(Data_In[C]);
			//d printf("%02x;",Data_In[C]&0xFF);
		}
		//d printf("\n");
	}
}

//################## Write_Single_Reg ###################
// Takes:   In Data Buffer
// Returns: 0=OK, or error code
// Effect:  Sets Reply Data and Register values, sends Response for Write Single Reg.
int Write_Single_Reg(unsigned char *Data_In, MemRange *memrng)  // Function code 1
{
	int MBAdr = HighByteLowByte(Data_In[2],Data_In[3]);

	printf ("WriteSingleReg(%02X) %04X\n",Data_In[1],MBAdr&0xFFFF); //debug

	unsigned short *MemAdr;
	int offset;
	int rslt=RangeConvert(MBAdr,1,memrng,&MemAdr,&offset);
	if (rslt)
		return rslt;

	MemAdr[offset] = HighByteLowByte(Data_In[4],Data_In[5]);
	Send_Response(Data_In,6);
	return 0;
}

//################## Write_Single_Coil ###################
// Takes:   In Data Buffer
// Returns: 0=OK, or error code
// Effect:  Writes single bit in Registers.  Sets Reply Data and sends Response
int Write_Single_Coil(unsigned char *Data_In, MemRange *memrng)  // Function code 1
{
	int MBAdr = HighByteLowByte(Data_In[2],Data_In[3]);
	int MBValue = HighByteLowByte(Data_In[4],Data_In[5]);

	printf ("WriteSingleCoil(%02X) %04X\n",Data_In[1],MBAdr&0xFFFF); //debug

	unsigned short *MemAdr;
	int offset;
	int rslt=RangeConvert(MBAdr,1,memrng,&MemAdr,&offset);
	if (rslt)
		return rslt;

	unsigned short Write_Address = MBAdr+offset/16;
	unsigned char Write_Bit = offset&0x0F;

	if (MBValue!=0xFF00 && MBValue!=0x0000)
	{
		printf("MBERROR_ILLEGAL_DATA_VALUE: WriteSingleCoil had bad value (not 0xFF00 or 0x0000)\n"); //debug
 		return MBERROR_ILLEGAL_DATA_VALUE;
 	}
	
	if(MBValue)
		MemAdr[Write_Address] |= (1<<Write_Bit);
	else
		MemAdr[Write_Address] &= ~(1<<Write_Bit);
	Send_Response(Data_In,6);
	return 0;
}

//################## Write_Regs ###################
// Takes:   In Data Buffer
// Returns: 0=OK, or error code
// Effect:  Writes Registers.  Sets Reply Data and sends Response
void Write_Regs(unsigned char *Data_In, MemRange *memrng)  // Function code 16 (0x10)
{
	int MBAdr = HighByteLowByte(Data_In[2],Data_In[3]);
	int MBCnt = HighByteLowByte(Data_In[4],Data_In[5]);
	unsigned char Byte_Count = Data_In[6];

	unsigned short *MemAdr;
	int offset;
	int rslt=RangeConvert(MBAdr,MBCnt,memrng,&MemAdr,&offset);
	if (rslt)
		return rslt;
	if (MBCnt*2!=Byte_Count)
	{
		printf("MBERROR_ILLEGAL_DATA_VALUE: Write_Regs byte count != 2*length\n"); //debug
		return MBERROR_ILLEGAL_DATA_VALUE;
	}
	if (MBCnt>125)
	{
		printf("MBERROR_ILLEGAL_DATA_VALUE: Write_Regs byte count != 2*length\n"); //debug
		return MBERROR_ILLEGAL_DATA_VALUE;
	}
	
	printf ("Write_Regs(%02X) %04X:%04X -> %04X+%04X\n",Data_In[1],MBAdr&0xFFFF,MBCnt&0xFFFF,MemAdr,offset); //debug

	unsigned short Read_Byte = 7;  // First byte of input Data_In
	int C;
	for(C = 0; C < Byte_Count;C+=2)
	{
		MemAdr[offset] = (Data_In[Read_Byte]<<8)+Data_In[Read_Byte+1];
		offset+=1;
		Read_Byte+=2;
	}
	Send_Response(Data_In,6);
	return 0;
}

//################## Write_Coils ###################
// Takes:   In Data Buffer
// Returns: 0=OK, or error code
// Effect:  Writes multiple bits in Registers.  Sets Reply Data and sends Response
int Write_Coils(unsigned char *Data_In, MemRange *memrng)  // Function code 1
{
	int MBAdr = HighByteLowByte(Data_In[2],Data_In[3]);
	int MBCnt = HighByteLowByte(Data_In[4],Data_In[5]);
	unsigned char Byte_Count = Data_In[6];

	unsigned short *MemAdr;
	int offset;
	int rslt=RangeConvert(MBAdr,MBCnt,memrng,&MemAdr,&offset);
	if (rslt)
		return rslt;
	if (MBCnt>246)
		return MBERROR_ILLEGAL_DATA_VALUE;

	printf ("Write_Coils(%02X) %04X:%04X\n",Data_In[1],MBAdr&0xFFFF,MBCnt&0xFFFF); //debug

	unsigned short Write_Address = MBAdr+offset/16;
	unsigned char Write_Bit = offset&0x0F;

	unsigned short Read_Byte = 7; // First byte of input Data_In
	unsigned char Read_Bit = 0;
	int C;
	for(C = 0; C < MBCnt;C++)
	{
		if((Data_In[Read_Byte]&(1<<Read_Bit))>0)		// If set Bit is a 1, then, set corresponding bit in register
		{
			MemAdr[Write_Address] |= (1<<Write_Bit);
		}
		else											// If set bit is a 0, clear the bit in the register
		{
			MemAdr[Write_Address] &= ~(1<<Write_Bit);
		}
		Read_Bit++;										// Increment the Read Bit
		Write_Bit++;									// Increment the write bit	
		if(Read_Bit>=8)									// Reads are from bytes of data, so increment every 8th bit
		{
			Read_Bit = 0;
			Read_Byte++;
		}
		if(Write_Bit >= 16)								// Write are to 16bit registers, so increment every 16th bit.	
		{
			Write_Bit = 0;
			Write_Address++;
		}
	}
	Send_Response(Data_In,6);
	return 0;
}

//################## Read Reg ###################
// Takes:   In Data Buffer
// Returns: 0=OK, or error code
// Effect:  Replies to query or returns error code
int Read_Regs(unsigned char *Data_In, MemRange *memrng)  // Function code 0x03,0x04
{
	int MBAdr = HighByteLowByte(Data_In[2],Data_In[3]);
	int MBCnt = HighByteLowByte(Data_In[4],Data_In[5]);

	unsigned short *MemAdr;
	int offset;
	int rslt = RangeConvert(MBAdr,MBCnt,memrng,&MemAdr,&offset);
	if (rslt)
		return rslt;
	if (MBCnt>125)
		return MBERROR_ILLEGAL_DATA_VALUE;

	//d printf ("Read_Regs(%02X) %04X:%04X -> %06X+%04X\n",Data_In[1],MBAdr&0xFFFF,MBCnt&0xFFFF,MemAdr,offset); //debug

	Data_In[2] = MBCnt * 2;

	unsigned short Item = 3;
	int Count;
	for(Count = 0; Count < MBCnt; Count++)
	{
		Data_In[Item++] = (MemAdr[offset+Count]>>8)&0xFF;
		Data_In[Item++] = MemAdr[offset+Count]&0xFF;
	}
	Send_Response(Data_In,Item);
	return 0;
}

//################## Read Coils ###################
// Takes:   In Data Buffer
// Returns: 0 or error code
// Effect:  Reads a bit at a time, composing 8 bit replies.  Sets Reply Data and sends Response
int Read_Coils(unsigned char *Data_In, MemRange *memrng)  // Function code 0x01,0x02
{
	int MBAdr = HighByteLowByte(Data_In[2],Data_In[3]);
	int MBCnt = HighByteLowByte(Data_In[4],Data_In[5]);
	
	unsigned short *MemAdr;
	int offset;
	int rslt = RangeConvert(MBAdr,MBCnt,memrng,&MemAdr,&offset);
	if (rslt)
		return rslt;
	
	printf ("Read_Coils(%02X) %04X:%04X -> %06X+%04X\n",Data_In[1],MBAdr&0xFFFF,MBCnt&0xFFFF,MemAdr,offset); //debug

	//Read data
	unsigned char Byte_Count = 0;
	unsigned char Sub_Bit_Count = 0;
	unsigned char Working_Byte = 0;

	unsigned short Item = 3;
	int Bit;
	for(Bit = 0; Bit < MBCnt; Bit++)
	{
		Working_Byte = Working_Byte | ((MemAdr[offset/16]>>(offset&0x0F))<<Sub_Bit_Count);
		Sub_Bit_Count++;
		if(Sub_Bit_Count >=8)
		{
			Data_In[Item] = Working_Byte;
			Working_Byte = 0;
			Sub_Bit_Count=0;
			Byte_Count++;
			Item++;
		}
	}
	//If not a full byte of info
	if(Sub_Bit_Count != 0)
	{
		Data_In[Item] = Working_Byte;
		Working_Byte = 0;
		Sub_Bit_Count=0;
		Byte_Count++;
		Item++;
	}
	Data_In[2] = Byte_Count; 
	Send_Response(Data_In,Item);
	return 0;
};



static unsigned char auchCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
} ;

static char auchCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
} ;


unsigned short CRC16(unsigned char *puchMsg,unsigned short usDataLen)
{
	unsigned char uchCRCHi = 0xff;
	unsigned char uchCRCLo = 0xff;
	unsigned int uIndex;
	while(usDataLen--)
	{
		uIndex = uchCRCLo ^ *puchMsg++;
		uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
		uchCRCHi = auchCRCLo[uIndex];
	}
	return (uchCRCHi<<8|uchCRCLo);
}


double LastInTime;
double EndOfPacketWait;
char packetBuild[256]; // max length of modbus frame
int packetSize=0;

//################## ModBusSlave ###################
// Takes:   Slave Address, Pointer to Registers and Number of Available Registers
// Returns: Nothing
// Effect:  Initializes Library
void ModbusSlave_Init()
{
	printf("\nModbus Slave Init\n");
	EnableRS232Cmds(RS232_BAUD_38400);
	DoRS232Cmds = FALSE;  // turn off processing RS232 input as commands
	LastInTime=Time_sec();
	EndOfPacketWait=3.5*10/38400; // wait 3.5 characters after a packet
	packetSize=0;

	int c;
	for (c=0;c<N_MB_RW_REGISTERS;c++)
		MBRWRegisters[c]=0;
	for (c=0;c<N_MB_RONLY_REGISTERS;c++)
		MBROnlyRegisters[c]=0;
		
	// make the register static arrays available to the other threads
	persist.UserData[PERSIST_RWREG_BLOCK_ADR]=(int)MBRWRegisters;
	//d printf("persist.UserData[%d]<=%08X\n",PERSIST_RWREG_BLOCK_ADR,MBRWRegisters); //debug
	persist.UserData[PERSIST_RONLYREG_BLOCK_ADR]=(int)MBROnlyRegisters;
	//d printf("persist.UserData[%d]<=%08X\n",PERSIST_RONLYREG_BLOCK_ADR,MBROnlyRegisters);  //debug
	
	if (0)	// memory location reference integrity test
	{
		MBRWRegisters[0]=0xDEAD;
		MBROnlyRegisters[0]=0xBEEF;
		if (MBRWRegisters[0]!=MBRWReg[0])
		{
			printf("%04X should equal %04X\n",MBRWRegisters[0],MBRWReg[0]);
		}
	}
}

void ModbusSlave_Monitor()
{
	char c;

	if (pRS232RecIn != pRS232RecOut) // data in buffer
	{
		LastInTime=Time_sec();
		c=RS232_GetChar();
		if (packetSize<255)
			packetBuild[packetSize++]=c;
		//d printf("%02x,",c&0xFF); //debug
	}
	else
	{
		if (LastInTime+EndOfPacketWait<Time_sec() && packetSize)
		{
			Process_Data(packetBuild,packetSize);
			//d printf("Error=%i, %f\n",Error,LastInTime+EndOfPacketWait-Time_sec());	//debug
			packetSize=0;	// ready for next packet
			//d printf("\n",c);	//debug
		}
	}
}

main()
{
	ModbusSlave_Init();

	printf("Start Modbus\n");
	while (1)
	{
		ModbusSlave_Monitor();
	}
	printf("End Modbus\n");
}


