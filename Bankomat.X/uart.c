/* 
 * File:   uart.c
 * Author: kosana
 *
 * Created on 08.01.2023., 23.05
 */

#include <p30fxxxx.h>
#include "uart.h"

void InitUART1(void)
{
    U1BRG=0x0015;// baudrate = 9600 jer je F=3.33MHz U1BRG=21
    U1MODEbits.ALTIO=0;// alternativni pinovi su zauzeti
    IEC0bits.U1RXIE=1;//omogucavamo rx1 interupt
    U1STA&=0xfffc; // zadnja 2 bita se setuju na 0 (bit za signalizaciju podataka u baferu i bit za gresku)
    U1MODEbits.UARTEN=1;//ukljucujemo ovaj modul
    U1STAbits.UTXEN=1;//ukljucujemo predaju
}

void WriteCharUART1(unsigned int data)
{
    while(!U1STAbits.TRMT); // cekanje da registar za predaju bude slobodan
    if(U1MODEbits.PDSEL == 3) // uslov da su podaci 8-bitni
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}

void WriteStringUART1(register const char *str)
{
    while((*str)!=0)
    {
        WriteCharUART1(*str);
        if(*str == 13) WriteCharUART1(10);
        if(*str == 10) WriteCharUART1(13);
        str++;
    }
}

void WriteUART1dec2string(unsigned int data)
{
    unsigned char temp;
    temp=data/1000;
    WriteCharUART1(temp+'0');
    data=data-temp*1000;
    temp=data/100;
    WriteCharUART1(temp+'0');
    data=data-temp*100;
    temp=data/10;
    WriteCharUART1(temp+'0');
    data=data-temp*10;
    WriteCharUART1(data+'0');
}

