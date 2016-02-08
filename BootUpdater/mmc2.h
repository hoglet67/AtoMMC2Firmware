
// contains all defines and externs for objects declared in main() scope or its like.


#define VSN_MAJ 1
#define VSN_MIN 4

#define SECBUFFSIZE 512
#define GLOBUFFSIZE 256

#define MAX_MOUNT_TRIES 15

#define EE_SYSFLAGS 0xff
#define EE_PORTBVALU 0xfe
#define EE_PORTBTRIS 0xfe

typedef void (*WORKERFN)(void);

extern void comDirectoryOpen(void);
extern void comDirectoryRead(void);
extern void comFileOpen(void);
extern void comFileGetInfo(void);
extern void comFileRead(void);
extern void comFileWrite(void);
extern void comFileClose(void);
extern void comOpenSDDOSImg(void);
extern void comReadSDDOSSect(void);
extern void comWriteSDDOSSect(void);
extern void comValidateSDDOSDrives(void);
extern void comSerialiseSDDOSDrives(void);
extern void comUnmountSDDOSImg(void);
extern void comGetSDDOSImgNames(void);
extern void comExecuteArbitrary(void);



#define DIR_OPEN comDirectoryOpen
#define DIR_READ comDirectoryRead
#define FILE_OPEN comFileOpen
#define FILE_GETINFO comFileGetInfo
#define FILE_READ comFileRead
#define FILE_WRITE comFileWrite
#define FILE_CLOSE comFileClose
#define SDDOS_OPENIMG comOpenSDDOSImg
#define SDDOS_READ comReadSDDOSSect
#define SDDOS_WRITE comWriteSDDOSSect
#define SDDOS_VALIDATE comValidateSDDOSDrives
#define SDDOS_SERIALISE comSerialiseSDDOSDrives
#define SDDOS_UNMOUNT comUnmountSDDOSImg
#define SDDOS_IMGNAME comGetSDDOSImgNames

#define STARCOMMAND comExecuteArbitrary


#ifdef BUILD_FOR_EMULATOR
extern unsigned char LATD;
extern unsigned char PORTD;
extern unsigned char PORTA;
extern unsigned char WASWRITE;
#endif

extern unsigned char configByte;
extern unsigned char CardType;
extern unsigned char blVersion;
extern unsigned char portBVal;

extern unsigned char sectorData[SECBUFFSIZE];
extern unsigned char globalData[GLOBUFFSIZE];

extern void WriteEEPROM(unsigned char address, unsigned char val);
extern unsigned char ReadEEPROM(unsigned char address);

extern WORKERFN workers[];
extern unsigned char CardType;

extern void initprocessor(void);
extern void process(void);


typedef struct
{
   unsigned long baseSector;
   char filename[13];
   unsigned char attribs;
}
imgInfo;

