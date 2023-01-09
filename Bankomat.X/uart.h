/* 
 * File:   uart.h
 * Author: kosana
 *
 * Created on 08.01.2023., 23.04
 */

#include <p30fxxxx.h>

// f-ja za inicijalizaciju 
void InitUART1(void);

// f-ja za slanje karaktera
void WriteCharUART1(unsigned int data);

// f-ja za slanje stringa
void WriteStringUART1(register const char *str);

// f-ja za  slanje 4-cifrenog broja
void WriteUART1dec2string(unsigned int data);
