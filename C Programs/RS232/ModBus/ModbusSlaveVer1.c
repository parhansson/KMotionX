#include "KMotionDef.h"

// RS232 Buffered input/output example to echo out any received character


int CheckSum;	// From Checksum Library, CRC15.h, CRC16.cpp

//################## ModBusSlave ###################
// Takes:   Slave Address, Pointer to Registers and Number of Available Registers
// Returns: Nothing
// Effect:  Initializes Library

int	Slave = 1;
unsigned short coilAry[2000];
unsigned short *Coils = coilAry;
int	Error = 0;
int	Coil_Count = 2000;

ModBusSlave(unsigned char _Slave, unsigned short * _Coils, unsigned short _Coil_Count)
{
	Slave = _Slave;
	Coils = _Coils;
	Error = 0;
	Coil_Count = _Coil_Count;
}

//################## Process Data ###################
// Takes:   Data stream buffer from serial port, number of characters to read
// Returns: Nothing
// Effect:  Reads in and parses data

void Process_Data(unsigned char *Buffer, unsigned char Count)
{
    Count--;									// Convert Byte count to byte index;
    unsigned char Slave_Address = Buffer[0];	// Grab Slave Address out of buffer
	if(Slave_Address > 247)						// If Address greater than 247 => Invalid
	{
		Error=1;
		return;
	}
    if(Slave_Address != 0) 						// Slave = 0 for Broadcast Msg
    {
      if(Slave != Slave_Address)  				// If Msg is not for this Slave
      {
        return;
      }
    }
    unsigned char Function = Buffer[1];									// Grab Function Code
    unsigned short CRC = ((Buffer[Count]<<8)|Buffer[Count-1]);			// Transmitted CRC
    unsigned short Recalculated_CRC = CRC16(Buffer,Count-1);	// Computer CRC
    if(Recalculated_CRC != CRC)											// Compare CRC, if not equal, then...
    {
		Error = 2;
      return;
    }

printf("Packet %d\n",Function);    
    
    if(Function == 1)													//Read Coils
    {
      Read_Coils(Buffer);
    }
    if(Function == 2)													//Read Descrete Input
    {
      Read_Coils(Buffer);
    }
    if(Function == 3)													//Read Reg
    {
      Read_Reg(Buffer);
    }
    if(Function == 4)													//Read Input Reg
    {
      Read_Reg(Buffer);
    }
    if(Function == 5)													//Write Single Coil
    {
      Write_Single_Coil(Buffer);
    }
    if(Function == 6)													//Write Single Reg
    {
      Write_Single_Reg(Buffer);
    }
	if(Function == 7)													//Read Exception Status
	{
		Read_Exception(Buffer);
	}
    if(Function == 15)													//Write Coils
    {
      Write_Coils(Buffer);
    }
    if(Function == 16)													//Write Reg
    {
      Write_Reg(Buffer);
    }
	Error = 0;															//We made it to the end, Set Error to 0
}

//################## Read Exception ###################
// Takes:   In Data Buffer
// Returns: Nothing
// Effect:  Sets Reply Data and sends Response
void Read_Exception(unsigned char *Data_In)
{
	Data_In[2] = Error;
	Send_Response(Data_In,3);
	Error = 0;
}
//################## Send Response ###################
// Takes:   In Data Buffer, Length
// Returns: Nothing
// Effect:  Sends Response over serial port
void Send_Response(unsigned char *Data_In,unsigned short Length)
{
	if(Data_In[0] == 0) 										// If Broadcast Msg, then no reply	
		return;
  if(Length > 0)												// If there is Data to be sent, then...
  {
    unsigned short MyCRC = CRC16(Data_In,Length);		// Calculate new CRC
    Data_In[Length++] = MyCRC & 0x00ff;							// Load lower byte into Buffer
    Data_In[Length++] = MyCRC >> 8;								// Load upper byte into Buffer
    int C;
    for(C = 0; C < Length;C++)								// Write Data
    {
      RS232_PutChar(Data_In[C]);
    }
  }
}
//################## Write_Single_Reg ###################
// Takes:   In Data Buffer
// Returns: Nothing
// Effect:  Sets Reply Data and Register values, sends Response for Write Single Reg.
void Write_Single_Reg(unsigned char *Data_In)  // Function code 1
{
  unsigned short Addr_Hi = Data_In[2];
  unsigned short Addr_Lo = Data_In[3];
  unsigned short Value_Hi = Data_In[4];
  unsigned short Value_Lo = Data_In[5];

  unsigned short Address = (Addr_Lo + (Addr_Hi<<8));
  	if(Address >= Coil_Count)								// Invalid Address;
	{
		Error = 3;
		return;
	}
  Coils[Address] = (Value_Hi<<8) + Value_Lo;
  Send_Response(Data_In,6);
}
//################## Write_Single_Coil ###################
// Takes:   In Data Buffer
// Returns: Nothing
// Effect:  Writes single bit in Registers.  Sets Reply Data and sends Response
void Write_Single_Coil(unsigned char *Data_In)  // Function code 1
{
  unsigned short Addr_Hi = Data_In[2];
  unsigned short Addr_Lo = Data_In[3];
  unsigned short Value_Hi = Data_In[4];
  unsigned short Value_Lo = Data_In[5];
  unsigned short Address = Addr_Lo + (Addr_Hi<<8);  
  unsigned short Write_Address = Address/16;
  unsigned char Write_Bit = Address&0x000F;
    	if(Address >= Coil_Count*16)			// Invalid Address;
	{
		Error = 3;
		return;
	}
    if(Value_Hi>0 | Value_Lo > 0)  				// Real Protocol requires 0xFF00 = On and 0x0000 = Off, Custom, using anything other than 0 => ON
    {
      Coils[Write_Address] |= (1<<Write_Bit);
    }
    else
    {
      Coils[Write_Address] &= ~(1<<Write_Bit);
    }
  Send_Response(Data_In,6);
}
//################## Write_Reg ###################
// Takes:   In Data Buffer
// Returns: Nothing
// Effect:  Writes Register in Registers.  Sets Reply Data and sends Response
void Write_Reg(unsigned char *Data_In)  // Function code 1
{
  unsigned short Addr_Hi = Data_In[2];
  unsigned short Addr_Lo = Data_In[3];
  unsigned short Cnt_Hi = Data_In[4];
  unsigned short Cnt_Lo = Data_In[5];
  unsigned char Byte_Count = Data_In[6];
  unsigned short Address = (Addr_Lo + (Addr_Hi<<8));
    	if(Address >= Coil_Count)	// Invalid Address;
	{
		Error = 3;
		return;
	}
  unsigned short Read_Byte = 7;  // First entry in input Data_In
  int C;
  for(C = 0; C < Byte_Count;C+=2)
  {
	  Coils[Address] = (Data_In[Read_Byte]<<8)+Data_In[Read_Byte+1];
      Address+=1;
      Read_Byte+=2;
  }
  Send_Response(Data_In,6);
}
//################## Write_Coils ###################
// Takes:   In Data Buffer
// Returns: Nothing
// Effect:  Writes multiple bits in Registers.  Sets Reply Data and sends Response
void Write_Coils(unsigned char *Data_In)  // Function code 1
{
  unsigned short Addr_Hi = Data_In[2];
  unsigned short Addr_Lo = Data_In[3];
  unsigned short Cnt_Hi = Data_In[4];
  unsigned short Cnt_Lo = Data_In[5];
  unsigned char Byte_Count = Data_In[6];
  
  unsigned short Address = Addr_Lo + (Addr_Hi<<8);
  unsigned short Write_Bit_Count = Cnt_Lo + (Cnt_Hi<<8);
  	if(Address >= Coil_Count*16)	// Invalid Address;
	{
		Error = 3;
		return;
	}
  
  unsigned short Write_Address = Address/16;
  unsigned char Write_Bit = Address&0x000F;
  
  unsigned short Read_Byte = 7; // First entry in input Data_In
  unsigned char Read_Bit = 0;
  int C;
  for(C = 0; C < Write_Bit_Count;C++)
  {
    if((Data_In[Read_Byte]&(1<<Read_Bit))>0)		// If set Bit is a 1, then, set corrisponding bit in register
    {
      Coils[Write_Address] |= (1<<Write_Bit);
    }
    else											// If set bit is a 0, clear the bit in the register
    {
      Coils[Write_Address] &= ~(1<<Write_Bit);
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
}
//################## Read Reg ###################
// Takes:   In Data Buffer
// Returns: Nothing
// Effect:  Reads bytes at a time and composes 16 bit replies.  Sets Reply Data and sends Response
void Read_Reg(unsigned char *Data_In)  // Function code 1
{
  unsigned short Addr_Hi = Data_In[2];
  unsigned short Addr_Lo = Data_In[3];
  unsigned short Cnt_Hi = Data_In[4];
  unsigned short Cnt_Lo = Data_In[5];
  //Read date
  unsigned short Byte_Count = Cnt_Lo + (Cnt_Hi<<8);
  Data_In[2] = Byte_Count * 2;
  
  unsigned short Address = (Addr_Lo + (Addr_Hi<<8));
    	if(Address >= Coil_Count)	// Invalid Address;
	{
		Error = 3;
		return;
	}
  unsigned short Item = 3;
  int Count;
  for(Count = 0; Count < Byte_Count; Count++)
  {
    Data_In[Item+1] = Coils[Address]&0xFF;
    Data_In[Item] = (Coils[Address]>>8)&0xFF;
	Address++;
    Item+=2;
  }
  Send_Response(Data_In,Item);
}

//################## Read Coils ###################
// Takes:   In Data Buffer
// Returns: Nothing
// Effect:  Reads a bit at a time, composing 8 bit replies.  Sets Reply Data and sends Response
void Read_Coils(unsigned char *Data_In)  // Function code 1
{
  unsigned short Addr_Hi = Data_In[2];
  unsigned short Addr_Lo = Data_In[3];
  unsigned short Cnt_Hi = Data_In[4];
  unsigned short Cnt_Lo = Data_In[5];
  //Read date
  unsigned short Bit_Count = Cnt_Lo + (Cnt_Hi<<8);
  unsigned char Byte_Count = 0;
  unsigned char Sub_Bit_Count = 0;
  unsigned char Working_Byte = 0;
  unsigned short Address = Addr_Lo + (Addr_Hi<<8);
    	if(Address >= Coil_Count*16)	// Invalid Address;
	{
		Error = 3;
		return;
	}
  unsigned short Item = 3;
  int Bit;
  for(Bit = 0; Bit < Bit_Count; Bit++)
  {
    Working_Byte = Working_Byte | ((Coils[Address/16]>>(Address&0x000f))<<Sub_Bit_Count);
    Address++;
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
char packetBuild[256]; // max length of modbus frame
int packetSize=0;

main()
{
	char c;
	
	EnableRS232Cmds(RS232_BAUD_38400);
	
	DoRS232Cmds = FALSE;  // turn off processing RS232 input as commands
	
	printf("Waiting\n");
	LastInTime=Time_sec();
	packetSize=0;
	while (1)
	{
		if (pRS232RecIn == pRS232RecOut) // wait for data in buffer
		{
			if (LastInTime+0.1<Time_sec() && packetSize)	//! should be 3.5 chars time
			{
				Process_Data(packetBuild,packetSize);
				printf("Error=%i,%f",Error,LastInTime+0.1-Time_sec());
				packetSize=0;	// ready for next packet
				printf("\n",c);
			}
		}
		else
		{
			LastInTime=Time_sec();
			c=RS232_GetChar();
			packetBuild[packetSize++]=c;
			//RS232_PutChar(c);
			printf("%02x,",c);
		}
	}
	printf("Done Waiting\n");
}


