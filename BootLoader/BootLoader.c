#include <p18cxxx.h>

#ifdef LINUX_BUILD
#include "../Shared/config.h"
#include "../Shared/general_io.h"
#else
#include "..\Shared\config.h"
#include "..\Shared\general_io.h"
#endif

#include "mmc.h"

#include "string.h"


const rom far char* r = (const rom far char*)
#if defined(__18F4520)
"ATOMMC20BIN";
#elif defined(__18F4523)
"ATOMMC23BIN";
#elif defined(__18F4525)
"ATOMMC25BIN";
#else
not valid as the processor define is set incorrectly
#endif


#define ENTERFIRMWARE() _asm goto 0x1000 _endasm

#define BS_55AA 510
#define SMBSIG 0x21424d53 // `SMB!`

#define  LD_WORD(ptr)      (WORD)(*(WORD*)(BYTE*)(ptr))
#define  LD_DWORD(ptr)     (DWORD)(*(DWORD*)(BYTE*)(ptr))
#define  ST_WORD(ptr,val)  *(WORD*)(BYTE*)(ptr)=(WORD)(val)
#define  ST_DWORD(ptr,val) *(DWORD*)(BYTE*)(ptr)=(DWORD)(val)

typedef enum {
   FR_OK = 0,        /* 0 */
   FR_DISK_ERR,      /* 1 */
   FR_NOT_READY,     /* 2 */
   FR_NO_FILE,       /* 3 */
   FR_NO_PATH,       /* 4 */
   FR_NOT_OPENED,    /* 5 */
   FR_NOT_ENABLED,      /* 6 */
   FR_NO_FILESYSTEM  /* 7 */
} FRESULT;


#define BS_FilSysType      54
#define BS_FilSysType32    82
#define MBR_Table       446
#define BPB_BytsPerSec     11
#define BPB_FATSz16        22
#define BPB_FATSz32        36
#define BPB_NumFATs        16
#define BPB_RsvdSecCnt     14
#define BPB_SecPerClus     13
#define BPB_RootEntCnt     17
#define BPB_TotSec32    32
#define BPB_TotSec16    19
#define BPB_RootClus    44
#define  DIR_FstClusHI     20
#define  DIR_FstClusLO     26
#define  DIR_FileSize      28

#define FS_FAT12  1
#define FS_FAT16  2
#define FS_FAT32  3


#pragma code low_vector=0x0008
void interrupt (void)
{
    _asm
    GOTO 0x1008

    // false instruction to double as bootloader version C0-01 F0-01
    MOVFF 0x02,0x09  // 0201
    _endasm
}
#pragma code


// jumped to if there's no firmware flashed
#pragma code bodger=0x1000
void bodger (void)
{
    Reset();
}
#pragma code



#pragma udata sectorbuf
#define GLOBUFFSIZE 512
static char globalData[GLOBUFFSIZE];
#pragma udata

BYTE CardType;

/* Definitions for MMC/SDC command */

#define  CMD0  (0x40+0)    /* GO_IDLE_STATE */
#define  CMD1  (0x40+1)    /* SEND_OP_COND (MMC) */
#define ACMD41 (0xC0+41)   /* SEND_OP_COND (SDC) */
#define  CMD8  (0x40+8)    /* SEND_IF_COND */
#define  CMD16 (0x40+16)   /* SET_BLOCKLEN */
#define  CMD17 (0x40+17)   /* READ_SINGLE_BLOCK */
#define  CMD24 (0x40+24)   /* WRITE_BLOCK */
#define  CMD55 (0x40+55)   /* APP_CMD */
#define  CMD58 (0x40+58)   /* READ_OCR */

BYTE XFER_SPI(BYTE output)
{
    char BitCount = 8;
    static char input;
    input = output;

    // SPI mode 0
    //
    // SCK idles low
    // Data output after falling edge of SCK
    // Data sampled before rising edge of SCK

    if (input&0x80)
        SPI_DOUT_PIN = 1;
    else
        SPI_DOUT_PIN = 0;
    Nop();    // Adjust for jitter
    Nop();
    do
    {
        if (SPI_DIN_PIN)
            STATUSbits.C = 1;
        else
            STATUSbits.C = 0;       // Set the carry bit according

        SPI_SCK_PIN = 1;
        _asm
        rlcf input,1,1
        _endasm

        Nop();    // Produces a 50% duty cycle clock
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();

        SPI_SCK_PIN = 0;

        if (input&0x80)
            SPI_DOUT_PIN = 1;
        else
            SPI_DOUT_PIN = 0;
    }
    while(--BitCount);

    return input;
}


/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

void release_spi(void)
{
   DESELECT();
   XFER_SPI(0xff);
}


/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static BYTE send_cmd (BYTE cmd, DWORD arg)
{
   BYTE n, res;

   union
   {
      BYTE b[4];
      DWORD d;
   }
   argbroke;
   argbroke.d = arg;

    if (cmd & 0x80)
    {
        /* ACMD<n> is the command sequense of CMD55-CMD<n> */
        cmd &= 0x7F;

        res = send_cmd(CMD55, 0);
        if (res > 1)
        {
            return res;
        }
   }

   /* Select the card */

   DESELECT();
   XFER_SPI(0xff);
   SELECT();
   XFER_SPI(0xff);

   /* Send a command packet */

   XFER_SPI(cmd);                /* Start + Command index */
   XFER_SPI(argbroke.b[3]);         /* Argument[31..24] */
   XFER_SPI(argbroke.b[2]);         /* Argument[23..16] */
   XFER_SPI(argbroke.b[1]);         /* Argument[15..8] */
   XFER_SPI(argbroke.b[0]);         /* Argument[7..0] */
   n = 0x01;                     /* Dummy CRC + Stop */
   if (cmd == CMD0) n = 0x95;       /* Valid CRC for CMD0(0) */
   if (cmd == CMD8) n = 0x87;       /* Valid CRC for CMD8(0x1AA) */
   XFER_SPI(n);

    /* Wait for a valid response in timeout of 10 attempts */

   n = 10;
   do
    {
      res = XFER_SPI(0xff);
   }
   while ((res & 0x80) && --n);

   return res;
}


void SinkXFER(void)
{
    XFER_SPI(0xff);
}

BYTE disk_initialise(void)
{
   WORD tmr;
   BYTE n, cmd, ty, ocr[4];

   GREENLEDON();

    /* init SPI */
   SPI_CS_TRIS = 0;
   DESELECT();

   SPI_DIN_TRIS = 1;

   SPI_DOUT_TRIS = 0;
   SPI_DOUT_PIN = 1;

   SPI_SCK_TRIS = 0;
   SPI_SCK_PIN = 0;

    /* send dummy clocks */
    /* at least 80 - enough to fluch through any unfinished cmd sequence */
   for (n = 0; n < 10; ++n)
   {
        SinkXFER();
   }

    /* initialise card & determine its capabilities */
   ty = 0;

    /* Enter Idle state */
    if (send_cmd(CMD0, 0) == 1)
    {
      if (send_cmd(CMD8, 0x1AA) == 1)
        {
            /* it's an SDv2 */
            /* Get trailing return value of R7 resp */
          SinkXFER();
          SinkXFER();
          ocr[2] = XFER_SPI(0xff);
          ocr[3] = XFER_SPI(0xff);

            if (ocr[2] == 0x01 && ocr[3] == 0xAA)
            {
                /* The card can work at vdd range of 2.7-3.6V */
                /* Wait for leaving idle state (ACMD41 with HCS bit) */
                for (tmr = 0x3000; tmr && send_cmd(ACMD41, 1UL << 30); --tmr);

                if (tmr && send_cmd(CMD58, 0) == 0)
                {
                    /* Check CCS bit in the OCR */
                ocr[0] = XFER_SPI(0xff);
                SinkXFER(); // code size reduction
                SinkXFER();
                SinkXFER();

                    /* SDv2 (HC or SC) */
               ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
            }
         }
      }
      else
        {
            /* SDv1 or MMCv3 */
            if (send_cmd(ACMD41, 0) <= 1)
            {
                /* SDv1 */
                ty = CT_SD1;
                cmd = ACMD41;
            }
         else
         {
            /* MMCv3 */
            ty = CT_MMC;
            cmd = CMD1;
         }

         /* Wait for leaving idle state */
         for (tmr = 0x7000; tmr && send_cmd(cmd, 0); tmr--);

            /* Set R/W block length to 512 */
            if (!tmr || send_cmd(CMD16, 512) != 0)
            {
            ty = 0;
            }
      }
   }

   release_spi();

   GREENLEDOFF();

   return ty;
}


DRESULT disk_read_sector(DWORD lba)
{
   DRESULT res;
   BYTE rc;
   WORD bc;

   GREENLEDON();

   if (!(CardType & CT_BLOCK))
   {
      /* Convert to byte address if needed */
        //lba *= 512;
        lba <<= 9;
   }

   res = RES_ERROR;

    /* READ_SINGLE_BLOCK */
   if (send_cmd(CMD17, lba) == 0)
    {
      /* Wait for data packet with timeout of 100s of ms */
      bc = 30000;
      do
      {
         rc = XFER_SPI(0xff);
      }
      while (rc == 0xFF && --bc);

        if (rc == 0xFE)
        {
            BYTE* data = &globalData[0];

            /* A data packet arrived */
            for(bc = 0; bc <512; ++bc)
            {
                data[bc] = XFER_SPI(0xff);
            }

            /* skip CRC */
         SinkXFER(); // less code than 'XFER_SPI(0xff);'
         SinkXFER();

         res = RES_OK;
      }
   }

   release_spi();

   GREENLEDOFF();

   return res;
}




/*-----------------------------------------------------------------------*/
/* Load boot record and check if it is an FAT boot record                */
/*-----------------------------------------------------------------------*/

static
BYTE check_fs (   /* 0:The FAT boot record, 1:Valid boot record but not an FAT, 2:Not a boot record, 3:Error */
   DWORD sect  /* Sector# (lba) to check if it is an FAT boot record or not */
)
{
   /* Load boot record */
   if (disk_read_sector(sect) != RES_OK)
      return 3;

   /* Check record signature (always placed at offset 510 even if the sector size is >512) */
   if (LD_WORD(&globalData[BS_55AA]) != 0xAA55)
      return 2;

   /* Check "FAT" string */
   if ((LD_DWORD(&globalData[BS_FilSysType]) & 0xFFFFFF) == 0x544146)
      return 0;

   if ((LD_DWORD(&globalData[BS_FilSysType32]) & 0xFFFFFF) == 0x544146)
      return 0;

   return 1;
}







/*-----------------------------------------------------------------------*/
/* Make sure that the file system is valid                               */
/*-----------------------------------------------------------------------*/

BYTE fmt;
BYTE csize;
DWORD fatbase;
DWORD dirbase;
DWORD database;
WORD n_rootdir;

/* FR_OK(0): successful, !=0: any error occurred */
FRESULT mount(void)
{
   BYTE *tbl;
   DSTATUS stat;
   DWORD bsect, fsize, tsect, mclst;

   /* The logical drive must be mounted. Following code attempts to mount the volume */

   bsect = 0;

   /* Search FAT partition on the drive */
   fmt = check_fs(bsect);
   if (fmt == 1)
   {
      /* Not an FAT boot record, it may be partitioned */
      /* Check a partition listed in top of the partition table */
      tbl = &globalData[MBR_Table];
      if (tbl[4])
      {
         bsect = LD_DWORD(&tbl[8]);
         fmt = check_fs(bsect);
      }
   }
   if (fmt == 3) return FR_DISK_ERR;
   if (fmt || LD_WORD(globalData+BPB_BytsPerSec) != 512) return FR_NO_FILESYSTEM;

   /* Number of sectors per FAT */
   fsize = LD_WORD(globalData+BPB_FATSz16);
   if (!fsize)
   {
      fsize = LD_DWORD(globalData+BPB_FATSz32);
   }

   fsize *= globalData[BPB_NumFATs];                  /* (Number of sectors in FAT area) */
   fatbase = bsect + LD_WORD(globalData+BPB_RsvdSecCnt); /* FAT start sector (lba) */
   csize = globalData[BPB_SecPerClus];                /* Number of sectors per cluster */
   n_rootdir = LD_WORD(globalData+BPB_RootEntCnt);       /* Number of root directory entries */
    n_rootdir >>= 4;  // /= (512/32);                       /* Number of entries per sector */

   tsect = LD_WORD(globalData+BPB_TotSec16);          /* Number of sectors on the volume */
   if (!tsect)
   {
      tsect = LD_DWORD(globalData+BPB_TotSec32);
   }

   /* Last cluster# + 1 (Number of clusters + 2) */
   mclst = (tsect - LD_WORD(globalData+BPB_RsvdSecCnt) - fsize - n_rootdir) / csize + 2;

   /* Determine the FAT sub type */
   fmt = FS_FAT12;
   if (mclst > 0xFF7) fmt = FS_FAT16;                 /* Number of clusters > 0xFF5 */
   if (mclst > 0xFFF7) fmt = FS_FAT32;                /* Number of clusters > 0xFFF5 */

   if (fmt == FS_FAT32)
      dirbase = LD_DWORD(globalData+BPB_RootClus);    /* Root directory start cluster */
   else
      dirbase = fatbase + fsize;                   /* Root directory start sector (lba) */

   database = fatbase + fsize + n_rootdir;                   /* Data start sector (lba) */

   return FR_OK;
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
                TABLAT = globalData[base];      // put a char into the table latch register
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


unsigned short updateCRC(unsigned char data, unsigned short crc)
{
    crc  = (unsigned char)(crc >> 8) | (crc << 8);
    crc ^= data;
    crc ^= (unsigned char)(crc & 0xff) >> 4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4) << 1;

    return crc;
}


void main(void)
{
    short i;
    BYTE configFlags;

    // start the clocks
    CLOCKINIT();

    // ensure all ADC channels, comparators are off
    ADCON1 = 0b00001111;
    CMCON = 0x07;

    TRISA = 0b11011111;

	// enable pullups on port B
	INTCON2bits.RBPU=0;

	// Make Port B an input. 
	TRISB = 0xFF;

    // lights out
    REDLEDOFF();
    GREENLEDOFF();
    LEDPINSOUT();

    EEADR = 0xff;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1;
    configFlags = EEDATA;

    if (configFlags & 0x20)
    {
      ASSERTIRQ();
    }

    // enter the bootloader only if config byte has bit 7 set.

    // now it might be possible to lock things up if we get here without any firmware loaded
    // and the bootloader flag is clear. This would be bad. It is extremely unlikely however
    // as long as any bootloader-updater ensures the eeprom is correctly configured (reset)...

	// if high bit of port b is 0, then enter the bootloader, this is a hardware way of forcing 
	// the bootload, even if the config register bit is cleared. 
    if ((configFlags & 0x80) || ((PORTB & 0x80)==0))
    {
        CardType = disk_initialise();

        if (CardType == 0)
        {
            ENTERFIRMWARE();
        }

        if(mount() != 0)
        {
            ENTERFIRMWARE();
        }

        // scan the 1st 'n_rootdir' directory entries for the firmware file.
        // must be multiple of 32, or 1 sector's worth of entries
        //
        while(n_rootdir)
        {
            disk_read_sector(dirbase);
            ++dirbase;

            i = 0;
            while(i<512)
            {
                int result = memcmppgm2ram(&globalData[i], r, 11);
                if (result == 0)
                {
                    WORD crc2;
                    DWORD address;
                    WORD crc = 0xffff;
                    BYTE x;
                    BYTE rom* progPtr = (BYTE rom*)0x1000;

                    // found it - get start cluster
                    // then translate into absolute sector
                    //
                    DWORD size, offset = ((DWORD)LD_WORD(globalData+i+DIR_FstClusHI) << 16) | LD_WORD(globalData+i+DIR_FstClusLO);
                    offset -= 2;
                    offset = offset * csize + database;

                    /* get file size. adjust for header*/
                    size = LD_DWORD(globalData+i+DIR_FileSize);
                    size -= 512;

                    // read 1st sector. contains magic string & crc
                    //
                    disk_read_sector(offset++);
                    address = LD_DWORD(globalData);
                    crc2 = LD_WORD(globalData+4);

                    // crc the 1st 8k of currently flashed program
                    //
                    for (i = 0; i < 0x2000; ++i)
                    {
                        x = progPtr[i];
                        crc = updateCRC(x, crc);
                    }

                    // if not a valid file or the same as already flashed ...
                    if (address != SMBSIG || crc == crc2)
                    {
                        ENTERFIRMWARE();
                    }

                    REDLEDON();
                    for (address = 0x1000; address < 0x1000 + size; address += 512)
                    {
                        GREENLEDON();
                        disk_read_sector(offset);
                        ++offset;
                        GREENLEDOFF();

                        WriteDataToMemory(address);
                    }
                    REDLEDOFF();

                    ENTERFIRMWARE();
                }

                i += 32;
                if (n_rootdir)
                {
                    --n_rootdir;
                }
            }
        }
    }

    ENTERFIRMWARE();
}
