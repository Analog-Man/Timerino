/*
 * File:        main.c
 * Autor:       Enzo Carollo
 *
 * Rev:         1.0
 * Date:        16/2/2013
 * email:       enzcar2@tin.it
 *
 * Compiler:    Microchip HI-Tech Pic C compiler v.9.82 30-Day Demo mode
 *              Optimizations Level=9
 *
 * The software is owned by the author, only for educational usage.
 * For Commercial use please contact the Author.
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

#include    <htc.h>
#include    "main.h"
#include    "Timer.h"
#include    "Adc.h"

/*
 *  Watchdog Timer Enable bit   = OFF,
 *  RA3/MCLR pin function is    = MCLR,
 *  Brown-out Detect Enable bit = ON.
 *
 */
__CONFIG( FOSC_INTRCIO & WDTE_OFF & MCLRE_ON & BOREN_ON );

/*
 *   Public Variables
 */

 unsigned char          Global_Counter;
 unsigned short long    Pulse_Time_Elapsed;
 unsigned short long    Pause_Time_Elapsed;
 unsigned char          Input_Status;
 TIMER_TYPE             Timer_Status; // Make primary timer struct.
 bit                    EE_Update;    // eeprom update flag


/*
 * main
 *
 */
void main(void)
{
    Init();                         // Call initializations
    Global_Counter = 0;             // Reset Global Base Time Conter
    Init_Timer();                   //

    Update_Timer();                 // First Update & load
    Calc_Seconds();                   //

    //setup flags
    if (FIRST_PULSE_PIN == SW_CLOSED)
      {
        Timer_Status.Mode = FIRST_PULSE;
        RELE_ON;
        Pulse_Time_Elapsed--;
      }
    else
      {
        Timer_Status.Mode = FIRST_PAUSE;
        RELE_OFF;
        Pause_Time_Elapsed--;

      }

    if (CONTINUOUS_PIN == SW_CLOSED)
        Timer_Status.Continuos_Flag = TRUE;
    else
        Timer_Status.Continuos_Flag = FALSE;

    while (FOREVER)
    {
        if (Global_Counter == 2)
        {
            if (Timer_Status.Mode == FIRST_PULSE)
            {
                if (Pulse_Time_Elapsed > 0)
                {
                    RELE_ON;
                    Pulse_Time_Elapsed--;
                }
                else if (Pause_Time_Elapsed > 0)
                {
                    RELE_OFF;
                    Pause_Time_Elapsed--;
                }
                if (Pause_Time_Elapsed == 0)    //both 0
                {
                    if (Timer_Status.Continuos_Flag == TRUE)     // update for next cycle
                    {
                        Timer_Status.Value_Updated = TRUE;       // Reload if continuos Mode
                    }
                }
                Global_Counter =0;
            }
            else // FIRST_PAUSE
            {
                if (Pause_Time_Elapsed > 0)
                {
                    RELE_OFF;
                    Pause_Time_Elapsed--;
                }
                else if (Pulse_Time_Elapsed > 0)
                {
                    Pulse_Time_Elapsed--;
                    RELE_ON;
                }
                else
                  {
                    RELE_OFF;                                  // switch off output at the end of single loop
                  }
                if (Pulse_Time_Elapsed == 0)
                {    //both 0
                    if (Timer_Status.Continuos_Flag == TRUE)    // update for next cycle
                    {
                        Timer_Status.Value_Updated = TRUE;      // Reload if continuos Mode
                    }
                }
                Global_Counter = 0;
            }
        }
        Update_Timer();                                         // Update Timer
        Calc_Seconds();                                           //
    } // end while forever
}

/*
 * Init microcontroller pheripheral
 *
 *
 */
void Init(void)
{

    CMCON = 0x07;       // Analog comparator disabled, all pin I/O
    ANSEL = 0x03;       // all digital I/O pins except RA0 and RA1  ( Pause an Pulse respectively)
    TRISA = ~0x04;      // RA2 output and others are digital input
    TRISC = 0xFF;       // All portc digital input
    PORTA = 0x00;       // clear portA
    OPTION_REGbits.nRAPU = 0; // ENABLE all PULL-UP IN RA PORT
    WPUA = 0x30;        // RB4 and RB5 with pull up internal enabled

    Init_Timer1();
    Init_Adc();
    TRIS_RELE = OUT;
    RELE_OFF;
    Input_Status = INPUT_MASK; // Filter pins
    EE_Update = FALSE;
}

/*
 * Update Timer
 *
 */

void Init_Timer(void)
{
    unsigned char i;
    for( i=0; i<6; i++)
    {
        Timer_Status.Timer_Byte[i]  = eeprom_read(START_EEPROM_ADDR+i); //load entire Timer structure from eeprom
    }
    Timer_Status.Value_Updated = TRUE;
    Timer_Status.Reset_Flags = FALSE;
}

/*
 * ISR Service routine
 *
 */

void interrupt
isr(void)
{
    if (PIR1bits.TMR1IF == 1)   // timer1 overflow interrupt
    {
        TMR1L = 0xdb;
        TMR1H = 0x0b; 		// Reload prescaler with 62500
        Global_Counter++;
        PIR1bits.TMR1IF = 0;    // clear the interrupt flag, enable next loop
    }
}

/*
 * Update if s m h closed
 */
void Update_Timer(void)
{
    unsigned char temp;
    unsigned int conversion_value;
    conversion_value = ((Read_Adc(AN0) * 10 + CONVERSION_NUMBER) / CONVERSION_NUMBER); // Pause conversion

    temp = INPUT_MASK;
    if((Input_Status != temp))                   // detect any change of inputs
    {
        Input_Status = temp;
        EE_Update = TRUE;                        // Enable eeprom storage
        Timer_Status.Value_Updated = TRUE;
        if( Timer_Status.Reset_Flags == FALSE)   // clear flags at first change after power on
        {
            Timer_Status.Pause.Seconds_Flag = FALSE;
            Timer_Status.Pause.Minutes_Flag = FALSE;
            Timer_Status.Pause.Hours_Flag   = FALSE;
            Timer_Status.Pulse.Seconds_Flag = FALSE;
            Timer_Status.Pulse.Minutes_Flag = FALSE;
            Timer_Status.Pulse.Hours_Flag   = FALSE;
            Timer_Status.Reset_Flags = TRUE;
        }
    }
    else
    {
        EE_Update = FALSE;
    }

    temp = PAUSE_INPUT_MASK;            // mask first correct bits
    switch (temp)
    {
        case SECONDS_MASK:              //for RC0
        Timer_Status.Pause.Seconds = (unsigned char) conversion_value; // numerical values 0-60
        Timer_Status.Pause.Seconds_Flag = TRUE;
        Timer_Status.Value_Updated = TRUE;
        break;

        case MINUTES_MASK:              //for RC1
        Timer_Status.Pause.Minutes = (unsigned char) conversion_value; // numerical values 0-60
        Timer_Status.Pause.Minutes_Flag = TRUE;
        Timer_Status.Value_Updated = TRUE;
        break;

        case HOURS_MASK:                //for RC2, increasing port pin
        Timer_Status.Pause.Hours = (unsigned char) conversion_value;   // numerical values 0-60
        Timer_Status.Pause.Hours_Flag = TRUE;
        Timer_Status.Value_Updated = TRUE;
        break;

        default:
        break;
    }
    conversion_value = ((Read_Adc(AN1) * 10 + CONVERSION_NUMBER) / CONVERSION_NUMBER);  // Pulse conversion

    temp = PULSE_INPUT_MASK;
    switch (temp)
    {
        case HOURS_MASK:                //for RC5 this mask becomes for seconds
        Timer_Status.Pulse.Seconds = (unsigned char) conversion_value; // numerical values 0-60
        Timer_Status.Pulse.Seconds_Flag = TRUE;
        Timer_Status.Value_Updated = TRUE;
        break;

        case MINUTES_MASK:              //for RC4
        Timer_Status.Pulse.Minutes = (unsigned char) conversion_value; // numerical values 0-60
        Timer_Status.Pulse.Minutes_Flag = TRUE;
        Timer_Status.Value_Updated = TRUE;
        break;

        case SECONDS_MASK:              //for RC3, decreasing port pin. Same as for Seconds, this mask becomes for Hours
        Timer_Status.Pulse.Hours = (unsigned char) conversion_value;  // numerical values 0-60
        Timer_Status.Pulse.Hours_Flag = TRUE;
        Timer_Status.Value_Updated = TRUE;
        break;

        default:
        break;
    }
}

/*
 * Fonction for Convert and load Timer Counters
 *
 */
void Calc_Seconds(void)  // load!!
{
    unsigned char temp;
    temp=0;
    if ((Timer_Status.Value_Updated == TRUE))
    {
        Pulse_Time_Elapsed = 1;   // default 1 sec
        Pause_Time_Elapsed = 1;
        // pulse
        if (Timer_Status.Pulse.Seconds_Flag == TRUE)
        {
            Pulse_Time_Elapsed += Timer_Status.Pulse.Seconds;
            temp++;
        }


        if (Timer_Status.Pulse.Minutes_Flag == TRUE)
        {
            Pulse_Time_Elapsed += (Timer_Status.Pulse.Minutes * 60);
            temp++;
        }


        if (Timer_Status.Pulse.Hours_Flag == TRUE)
        {
            Pulse_Time_Elapsed  += (Timer_Status.Pulse.Hours * 3600);
            temp++;
        }
        if(temp >0)
        {
            Pulse_Time_Elapsed--;
        }

        temp=0;
        //pause
        if (Timer_Status.Pause.Seconds_Flag == TRUE)
        {
            Pause_Time_Elapsed += Timer_Status.Pause.Seconds;
            temp++;
        }


        if (Timer_Status.Pause.Minutes_Flag == TRUE)
        {
            Pause_Time_Elapsed += (Timer_Status.Pause.Minutes * 60);
            temp++;
        }

         if (Timer_Status.Pause.Hours_Flag == TRUE)
        {
            Pause_Time_Elapsed += (Timer_Status.Pause.Hours * 3600);
            temp++;
        }
         if(temp >0)
        {
            Pause_Time_Elapsed--;
        }
        Timer_Status.Value_Updated = FALSE;     //Counters Reloaded
    }
    Save_Timer();                               //save
}


/*
 *      Function for store Timer Data in eeprom
 */
void Save_Timer(void)
{
    unsigned char i;
    for( i=0; i<6; i++)
    {
        if(EE_Update == TRUE)   // Save only if change occured
        {
            eeprom_write((START_EEPROM_ADDR+i),Timer_Status.Timer_Byte[i] );    // Set Update flags
            __delay_us(1);                                                      // Delay after byte saving
        }
    }
}

