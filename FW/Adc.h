/*
 * File:        Adc.h
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
#include <htc.h>




/*
 *
 * Channels ADC inputs
 *
 */
typedef enum
{
	AN0 =0,
	AN1,
	AN2,
	AN3,
	AN4,
	AN5,
	AN6,
	AN7
} ANALOG_CH;



/*
 *
 * Functions Prototypes
 *
 */
void Init_Adc( void );
unsigned int Read_Adc( ANALOG_CH channel );



