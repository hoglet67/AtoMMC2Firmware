#include <p18cxxx.h>

#include <stdio.h>
#include <string.h>
#include <delays.h>

#ifdef LINUX_BUILD
#include "../Shared/config.h"
#include "../Shared/general_io.h"
#else
#include "..\Shared\config.h"
#include "..\Shared\general_io.h"
#endif
#include "mmc2.h"
#include "device_io.h"
#include "pff.h"




#pragma udata sectorbuf
char sectorData[SECBUFFSIZE];
#pragma udata


#define FLAG_BLENABLED 0x80
#define FLAG_IRQENABLED 0x20


const rom far char* r = (const rom far char*)
#if defined(__18F4520)
"BOOTLD20.BIN";
#elif defined(__18F4523)
"BOOTLD23.BIN";
#elif defined(__18F4525)
"BOOTLD25.BIN";
#else
not valid as the processor define is set incorrectly
#endif



#pragma code _RESET_VECTOR = 0x001000
extern void _startup (void);        // See c018i.c in your C18 compiler dir
void _start(void)
{
   _asm goto _startup _endasm
}
#pragma code

#pragma code _HIIRQ_VECTOR = 0x1008
void PSP(void);
#pragma interrupt PSP
void PSP(void)
{
   REDLEDON();
}
#pragma code


static DIR dir;
static FILINFO fno;
static FATFS fs;


void redSignal(char code)
{
   int mark;
   long arm;

   for (mark = 0; mark < code; ++mark)
   {
      REDLEDON();
      for(arm = 0; arm < 40000; ++arm)
      {
      }
      REDLEDOFF();
      for(arm = 0; arm < 40000; ++arm)
      {
      }
   }

   for(arm = 0; arm < 200000; ++arm)
   {
   }
}


void greenSignal(char code)
{
   int mark;
   long arm;

   for (mark = 0; mark < code; ++mark)
   {
      GREENLEDON();
      for(arm = 0; arm < 40000; ++arm)
      {
      }
      GREENLEDOFF();
      for(arm = 0; arm < 40000; ++arm)
      {
      }
   }

   for(arm = 0; arm < 200000; ++arm)
   {
   }
}

void bail(int code)
{
   int i;

   GREENLEDOFF();

   for (i = 0; i < 25; ++i)
   {
      redSignal(code);
   }

   for (;;)
   {
   }
}



void WriteEEPROM(BYTE address, BYTE val)
{
   EEADR = address;
   EEDATA = val;
   EECON1bits.EEPGD = 0;
   EECON1bits.CFGS = 0;
   EECON1bits.WREN = 1;
   EECON2 = 0x55;
   EECON2 = 0xAA;
   EECON1bits.WR = 1;
   while(EECON1bits.WR)
   {
      _asm nop _endasm;
   }
   EECON1bits.WREN = 0;
}


BYTE ReadEEPROM(BYTE address)
{
   EEADR = address;
   EECON1bits.EEPGD = 0;
   EECON1bits.CFGS = 0;
   EECON1bits.RD = 1;
   return EEDATA;
}




void WriteDataToMemory(DWORD startaddr)
{
    BYTE i, j;
    WORD base = 0;
    WORD blockCount = 0;

    while (blockCount < 8)
    {
        TBLPTR = startaddr;

        // erase row of 64 bytes starting at TBLTPTR address
        EECON1bits.EEPGD = 1;
        EECON1bits.WREN = 1;
        EECON1bits.FREE = 1;
        EECON1bits.CFGS = 0;
        EECON2 = 0x55;
        EECON2 = 0xaa;
        EECON1bits.WR = 1;
        Nop();
        EECON1bits.WREN = 0;

        // each of the eight holding registers
        for(i = 0; i < 8; ++i)
        {
            TBLPTR = startaddr;
            for (j = 0; j < 8; ++j)
            {
                TABLAT = sectorData[base];      // put a char into the table latch register
                _asm
                TBLWTPOSTINC                    // write to holding register
                _endasm
                ++base;
            }
            TBLPTR -= 8;

            // after each 8-byte write to holding registers...
            EECON1bits.EEPGD = 1;               // ...write the holding registers to flash
            EECON1bits.WREN = 1;
            EECON1bits.CFGS = 0;
            EECON1bits.FREE = 0;
            EECON2 = 0x55;
            EECON2 = 0xaa;
            EECON1bits.WR = 1;
            Nop();
            EECON1bits.WREN = 0;

            startaddr += 8;
        }

        ++blockCount;
    }
}


void main(void)
{
   // they're static so they're heap based (check this ;)
   //
   static int i;
   static char received = 0;
   char res, configByte;
   short temp, blVersion;
   DWORD address;

   CLOCKINIT();

   // ensure all ADC channels, comparators are off
   //
   ADCON1 = 0b00001111;
   CMCON = 0x07;
   TRISA = 0b11101111;

   REDLEDOFF();
   GREENLEDOFF();

   LEDPINSOUT();

   configByte = ReadEEPROM(EE_SYSFLAGS);

   // get the bootloader's version bytes out of program flash
   //
   memcpypgm2ram((void*)(&sectorData[0]), (const rom far void *)13, 1);
   memcpypgm2ram((void*)(&sectorData[1]), (const rom far void *)15, 1);
   blVersion = sectorData[0] << 4 + (sectorData[1] & 0x0f);

   // reset config
   //
   WriteEEPROM(0xff,0xff);


   {
      int i;

      for (i = 0; i < MAX_MOUNT_TRIES; ++i)
      {
         if (pf_mount((BYTE*)sectorData, &fs) == 0)
         {
            break;
         }
      }
   }

   strcpypgm2ram((char*)sectorData, r);
   res = pf_open(&sectorData[0]);
   if (res)
   {
      bail(res);
   }

   REDLEDON();
   for(address = 0; address < 0x1000; )
   {
      GREENLEDON();
      pf_read((void*)sectorData, 512, (WORD*)&temp);
      WriteDataToMemory(address);
      address += 0x200;
      GREENLEDOFF();
   }
   REDLEDOFF();

   for(;;)
   {
      GREENLEDON();
      for(address = 0; address < 40000; ++address)
      {
      }
      GREENLEDOFF();
      for(address = 0; address < 40000; ++address )
      {
      }
   }
}
