/*
 * File:        Adc.c
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

#include "main.h"
#include "Adc.h"


/*
 * Init Adc Function
 *
 *
 */
void Init_Adc( void )
{
	ADCON0 = 0;	        // Left justified, Vref=Vdd, an0, Adc stop
	ADCON0bits.ADFM = 1; 	// right justification xxxxxxDD DDDDDDDD
	ADCON1bits.ADCS = 1;    // 1=Fbus/8 for Adc clock = 125KHz
	//ANSEL = 0x03;         // alredy defined in init microcontroller
	ADCON0bits.ADON = ON;	// Power ON Adc module ( conversion NOT started until GO_nDONE was set)
	__delay_us(25);	        // steady time analog input ( charge stray capacitance )
}

/*
 * Read Adc Function for every analog input channels
 *
 *
 */
unsigned int Read_Adc( ANALOG_CH channel )
{
	unsigned int result = 0;

	ADCON0bits.CHS = channel; 	// select channel
	__delay_us(25);                 // Wait for correct switch Multiplexing
	ADCON0bits.GO_nDONE = ON;  	// Start conversion
	__delay_us(5);
	while (ADCON0bits.GO_nDONE == TRUE);  // Wait until data ready
	result =( ADRESH<<8 ) | ADRESL;
	return result;
}



