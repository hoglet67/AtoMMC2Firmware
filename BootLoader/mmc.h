#ifndef _MMC

#include "integer.h"

#define SPI_CS_PIN PORTCbits.RC2
#define SPI_CS_TRIS TRISCbits.TRISC2

#define SPI_DIN_PIN PORTCbits.RC5
#define SPI_DIN_TRIS TRISCbits.TRISC5

#define SPI_DOUT_PIN PORTCbits.RC4
#define SPI_DOUT_TRIS TRISCbits.TRISC4

#define SPI_SCK_PIN   PORTCbits.RC3
#define SPI_SCK_TRIS  TRISCbits.TRISC3

#define SELECT() SPI_CS_PIN = 0 // MMC CS = L
#define DESELECT() SPI_CS_PIN = 1 // MMC CS = H
#define MMC_SEL()  SPI_CS_PIN == 0 // MMC CS status (true:selected)

/* Status of Disk Functions */
typedef BYTE DSTATUS;

/* Results of Disk Functions */
typedef enum {
 RES_OK = 0, /* 0: Function succeeded */
 RES_ERROR, /* 1: Disk error */
 RES_NOTRDY, /* 2: Not ready */
 RES_PARERR /* 3: Invalid parameter */
} DRESULT;

#define STA_NOINIT 0x01 /* Drive not initialized */
#define STA_NODISK 0x02 /* No medium in the drive */

/* Card type flags (CardType) */
#define CT_MMC 0x01 /* MMC ver 3 */
#define CT_SD1 0x02 /* SD ver 1 */
#define CT_SD2 0x04 /* SD ver 2 */
#define CT_SDC (CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK 0x08 /* Block addressing */

#define _MMC
#endif

