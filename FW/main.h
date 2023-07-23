/*
 * File:        main.h
 * Autor:       Enzo Carollo
 *
 * Rev:         1.0
 * Date:        16/2/2013
 *
 * The software is owned by the author,
 * and is protected under applicable copyright laws. All rights are
 * reserved. Any use in violation of the foregoing restrictions may
 * subject the user to criminal sanctions under applicable laws, as
 * well as to civil liability for the breach of the terms and
 * conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE AUTHOR SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENT.
 */

#define _XTAL_FREQ 4000000

#include <htc.h>


/*
 * Definitions
 */

#define RELE                  PORTAbits.RA2
#define TRIS_RELE             TRISAbits.TRISA2
#define RELE_ON               PORTAbits.RA2 = ON; NOP()
#define RELE_OFF              PORTAbits.RA2 = OFF; NOP()

#define TRIS_CONTINUOUS_PIN     TRISAbits.TRISA5
#define TRIS_FIRST_PULSE_PIN    TRISAbits.TRISA4

#define CONTINUOUS_PIN          PORTAbits.RA5
#define FIRST_PULSE_PIN         PORTAbits.RA4

#define SECONDS_MASK            0x01
#define MINUTES_MASK            0x02
#define HOURS_MASK              0x04

#define CONVERSION_NUMBER       173u

#define INPUT_MASK              (~PORTC) & 0x3F
#define PULSE_INPUT_MASK        ((~PORTC) & 0x38) >> 3
#define PAUSE_INPUT_MASK        (~PORTC) & 0x07

/*
 * Project configurations:
 *
 */
#define OFF                     0
#define ON                      1

#define OUT                     0
#define IN                      1

#define FALSE                   0
#define TRUE                    1

#define FOREVER                 1

#define ENABLE                  1
#define DISABLE                 0

#define DONE                    1
#define RUNNING                 0

#define SW_OPEN                 1
#define SW_CLOSED               0

#define START_EEPROM_ADDR       0




/*
 * Types
 *
 */

enum
{
	FIRST_PAUSE =0,
	FIRST_PULSE
};


/*
 * Values and Flags internal struct
 */
typedef struct
{
    unsigned char Seconds :6;               // 0-60
    unsigned char Seconds_Flag :1;
    unsigned char Minutes :6;
    unsigned char Minutes_Flag :1;
    unsigned char Hours :6;
    unsigned char Hours_Flag :1;
} TIMER_TIME;



/*
 * Timer struct
 */
typedef union
{
    unsigned char  Timer_Byte[6];           // Full timer Array
    struct
    {
        TIMER_TIME Pulse;
        TIMER_TIME Pause;
        unsigned char Continuos_Flag :1;    // loop mode
        unsigned char Mode :1;              // first pulse
        unsigned char Value_Updated :1;     // for reload counters
        unsigned char Reset_Flags :1;       // for first timer update
        unsigned Unused_Flag :2;            //
    };
} TIMER_TYPE;



/*
 *
 * Functions Prototypes
 *
 */

void Init(void);
void Init_Timer (void);
void interrupt isr(void);
void Update_Timer(void);
void Calc_Seconds(void);
void Save_Timer(void);
