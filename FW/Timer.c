/*
 * File:        Timer.c
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


/*
 *
 * Functions
 *
 */
void Init_Timer1(void)
{

	TMR1L  = 0xdb;
	TMR1H  = 0x0b; 	    // Load prescaler with 62500
	T1CON  = 0x31;		// Timer 1 On with prescale internal clock by 8 -> 125KHz
	PIE1   = 0x01;		// Enable Timer 1 Interrupt;
	PIR1   = 0x00;		// Interrupts Flag register cleared
	INTCON = 0xc0;		// Enable global and pheripheral interrupts
}








