#ifndef _IO

#ifndef BUILD_FOR_EMULATOR

#define LEDPINSOUT() TRISCbits.TRISC0 = 0; TRISCbits.TRISC1 = 0;

#define REDLEDON() PORTCbits.RC0 = 1;
#define REDLEDOFF() PORTCbits.RC0 = 0;
#define GREENLEDON() PORTCbits.RC1 = 1;
#define GREENLEDOFF() PORTCbits.RC1 = 0;

#define ASSERTIRQ()  PORTCbits.RC6 = 0; TRISCbits.TRISC6 = 0;
#define RELEASEIRQ() TRISCbits.TRISC6 = 1;

#define ACTIVITYSTROBE(x) LATAbits.LATA5 = x;

#define WASWRITE TRISEbits.IBF

extern void redSignal(unsigned char);

#else

// it's for an emulator!

#define LEDPINSOUT()

#define REDLEDON()
#define REDLEDOFF()
#define GREENLEDON()
#define GREENLEDOFF()

#define ASSERTIRQ()
#define RELEASEIRQ()

#define ACTIVITYSTROBE(x)

#define STKPTR 0

#define redSignal(x)

#endif

#define _IO
#endif
