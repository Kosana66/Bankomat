/* 
 * File:   tajmer.c
 * Author: kosana
 *
 * Created on 03.01.2023., 23.49
 */

#include <p30fxxxx.h>
#include "tajmer.h"

#define TMR1_period 3000 
//  Fosc = 3.33MHz
// 1/Fosc = 0.33us !!!, 0.33us * 3000 = 1ms  

#define TMR2_period 3 
//  Fosc = 3.33MHz
// 1/Fosc = 0.33us !!!, 0.33us * 3 = 1us 

#define TMR3_period 300
//  Fosc = 3.33MHz
// 1/Fosc = 0.33us !!!, 0.33us * 300 = 0.1ms

void Init_T1(void)
{
	TMR1 = 0;
	PR1 = TMR1_period;
	
	T1CONbits.TCS = 0; // 0 = Internal clock (FOSC/4)
	//IPC1bits.T1IP = 3 // T12 interrupt priority (0-7)
	//SRbits.IPL = 3; // CPU interrupt priority is 3(11)
	IFS0bits.T1IF = 0; // clear interrupt flag
	IEC0bits.T1IE = 1; // enable interrupt

//	T1CONbits.TON = 1; // T1 on 
}

void Init_T2(void)
{
	TMR2 = 0;
	PR2 = TMR2_period;
	
	T2CONbits.TCS = 0; // 0 = Internal clock (FOSC/4)
	//IPC1bits.T1IP = 3 // T12 interrupt priority (0-7)
	//SRbits.IPL = 3; // CPU interrupt priority is 3(11)
	IFS0bits.T2IF = 0; // clear interrupt flag
	IEC0bits.T2IE = 1; // enable interrupt

//	T2CONbits.TON = 1; // T1 on 
}

void Init_T3(void)
{
	TMR3 = 0;
	PR3 = TMR3_period;
	
	T3CONbits.TCS = 0; // 0 = Internal clock (FOSC/4)
	//IPC1bits.T1IP = 3 // T12 interrupt priority (0-7)
	//SRbits.IPL = 3; // CPU interrupt priority is 3(11)
	IFS0bits.T3IF = 0; // clear interrupt flag
	IEC0bits.T3IE = 1; // enable interrupt

//	T2CONbits.TON = 1; // T1 on 
}