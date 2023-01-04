/* 
 * File:   newmain.c
 * Author: kosana
 *
 * Created on 03.01.2023., 20.58
 */

#include<p30fxxxx.h>
#include <stdlib.h>
#include "glcd.h"
#include "tajmer.h"

_FOSC(CSW_ON_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal
//_FOSC(CSW_FSCM_OFF & XT);//deli sa 4
_FWDT(WDT_OFF);
_FGS(CODE_PROT_OFF);

int ms, us, stanje, pritisnut_taster;

// funkcija za kasnjenje u milisekundama
void Delay_ms (int vreme)
{
    ms = 0;
    while(ms < vreme);
}

// prekidna rutina za tajmer 1
void __attribute__ ((__interrupt__)) _T1Interrupt(void) // svakih 1ms
{
    TMR1 = 0;
    ms++;
    IFS0bits.T1IF = 0;     
}

// funkcija za kasnjenje u mikrosekundama
void Delay_us (int vreme)
{
    us = 0;
    while(us < vreme);
}

// prekidna rutina za tajmer 2
void __attribute__ ((__interrupt__)) _T2Interrupt(void) // svakih 1us
{
    TMR2 = 0;
    us++;
    IFS0bits.T2IF = 0;     
}

// slika tastature
const char tastatura [1024] = {
 0,  0,  0,254,  2,  2,  2,  2,  2,242,  2,  2,  2,  2,  2,254, 
   0,  0,254,  2,  2,  2,146,146,146,146,242,  2,  2,  2,254,  0, 
   0,254,  2,  2,  2,146,146,146,146,242,  2,  2,  2,254,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,127, 64, 64, 64, 64, 64, 79, 64, 64, 64, 64, 64,127, 
   0,  0,127, 64, 64, 64, 79, 72, 72, 72, 72, 64, 64, 64,127,  0, 
   0,127, 64, 64, 64, 72, 72, 72, 72, 79, 64, 64, 64,127,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,254,  2,  2,  2,242,130,130,130,242,  2,  2,  2,254, 
   0,  0,254,  2,  2,  2,242,146,146,146,146,  2,  2,  2,254,  0, 
   0,254,  2,  2,  2,242, 18, 18, 18, 18,  2,  2,  2,254,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,127, 64, 64, 64, 64, 64, 64, 64, 79, 64, 64, 64,127, 
   0,  0,127, 64, 64, 64, 72, 72, 72, 72, 79, 64, 64, 64,127,  0, 
   0,127, 64, 64, 64, 79, 73, 73, 73, 79, 64, 64, 64,127,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,254,  2,  2,  2, 18, 18,146,146,242,130,  2,  2,254, 
   0,  0,254,  2,  2,  2,242, 18, 18, 18,242,  2,  2,  2,254,  0, 
   0,254,  2,  2,  2,242,146,146,146,242,  2,  2,  2,254,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,127, 64, 64, 64, 64, 64, 64, 64, 79, 64, 64, 64,127, 
   0,  0,127, 64, 64, 64, 79, 73, 73, 73, 79, 64, 64, 64,127,  0, 
   0,127, 64, 64, 64, 72, 72, 72, 72, 79, 64, 64, 64,127,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,254,  2,  2,  2,242, 18, 18, 18,242,  2,  2,  2,254,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,127, 64, 64, 64, 79, 72, 72, 72, 79, 64, 64, 64,127,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 
};

// f-ja za prikaz pocetnog ekrana nakon detektovanja pokreta pir senzorom
void PocetniEkran()
{
        GLCD_DisplayPicture(tastatura);
        // prikaz stringa "Unesite pin: "
        GoToXY(64,1);
        GLCD_Printf("Unesite");
        GoToXY(75,2);
        GLCD_Printf("PIN:");
        GoToXY(60,5);
}

/*
        // sabloni za prikaz unetog broja
        Glcd_PutChar('1');
        GoToXY(75,5);
        Glcd_PutChar('2');
        GoToXY(90,5);
        Glcd_PutChar('3');
        GoToXY(105,5);
        Glcd_PutChar('4'); 
    */

// f-ja za prikaz dnevnog screensavera
void DnevniScreensaver()
{
        GoToXY(35,1);
        GLCD_Printf("Dobar dan!");
        GLCD_Rectangle(31,5,95,17);
        GoToXY(20,3);
        GLCD_Printf("Dobro dosli na ");
        GoToXY(40,4);
        GLCD_Printf("bankomat ");
        GoToXY(19,5);
        GLCD_Printf("Unicredit banke ");
    
}

// f-ja za prikaz nocnog screensavera
void NocniScreensaver()
{
    GoToXY(32,1);
        GLCD_Printf("Dobro vece!");
        GLCD_Rectangle(28,5,98,17);
        GoToXY(20,3);
        GLCD_Printf("Dobro dosli na ");
        GoToXY(40,4);
        GLCD_Printf("bankomat ");
        GoToXY(19,5);
        GLCD_Printf("Unicredit banke ");
    
}

//f-ja za detektovanje pokreta pomocu pir senzora
int PirSenzor()
{
    if(PORTBbits.RB12==1)
    {
        return 1; //Dioda D8 se aktivira kad je detektovan pokret
    }
    else
    {
        return 0;
    }
}
// f-ja za upravljanje servo motorom
void ServoMotor()
{  
    // softversko diferenciranje da bi se resio debouncing
    if(PORTDbits.RD8 || PORTDbits.RD9)
    {
        if(stanje<21)
            stanje++;
    }
    else
        stanje=0;

    if(stanje==20)
        pritisnut_taster=1;
        
    if(pritisnut_taster==1)//ako je taster pritisnut
    {
        // otvaranje vrata pomocu servo motora ( -90 stepeni )
        LATBbits.LATB11 = 1;
        Delay_ms(1);
        LATBbits.LATB11 = 0;
        Delay_ms(19);
        LATBbits.LATB11 = 1;
        
        // cekanje 10s da korisnik unese/izvadi novac
        // Delay_ms(10000);
    
        // zatvaranje vrata pomocu servo motora ( 90 stepeni )
        LATBbits.LATB11 = 1;
        Delay_ms(2);
        LATBbits.LATB11 = 0;
        Delay_ms(18); 
        LATBbits.LATB11 = 1;
        
        pritisnut_taster=0;
    }
 }

int main(int argc, char** argv) {

    //za glcd
    ConfigureLCDPins();
    GLCD_LcdInit();
    GLCD_ClrScr();
           
    //za pir senzor
    ADPCFGbits.PCFG12=1; // digitalni
    TRISBbits.TRISB12=1;// ulaz
    
    // za servo motor
    ADPCFGbits.PCFG11=1; // digitalni
    TRISBbits.TRISB11=0;// izlaz za pwm
    // za taster kojim se bira uplata/isplata
    TRISDbits.TRISD8=1; // ulaz  
    TRISDbits.TRISD9=1; // ulaz
    TRISAbits.TRISA11=0; // ulaz
     
    Init_T1();
    
    
   // Init_T2();
    while(1)
    {
   /*     DnevniScreensaver();
        while(!PirSenzor());
        GLCD_ClrScr();
        PocetniEkran();*/
        
        ServoMotor();
        
    }
    
    
    return (EXIT_SUCCESS);
}



