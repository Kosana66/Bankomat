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
#include "adc.h"

_FOSC(CSW_ON_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal
//_FOSC(CSW_FSCM_OFF & XT);//deli sa 4
_FWDT(WDT_OFF);
_FGS(CODE_PROT_OFF);

unsigned int X, Y, x_vrednost, y_vrednost;
const unsigned int AD_Xmin =220;
const unsigned int AD_Xmax =3642;
const unsigned int AD_Ymin =520;
const unsigned int AD_Ymax =3450;

unsigned int broj,broj1,broj2, temp0, temp1, sirovi0, sirovi1, tacna_sifra; 

#define DRIVE_A PORTCbits.RC13
#define DRIVE_B PORTCbits.RC14


void Delay(unsigned int N)
{
	unsigned int i;
	for(i=0;i<N;i++);
}

void TouchPanel (void)
{
// vode horizontalni tranzistori
    DRIVE_A = 1;  
    DRIVE_B = 0;
    
    LATCbits.LATC13=1;
    LATCbits.LATC14=0;

    Delay(500); //cekamo jedno vreme da se odradi AD konverzija
				
    // ocitavamo x	
    x_vrednost = temp0;//temp0 je vrednost koji nam daje AD konvertor na BOTTOM pinu		

// vode vertikalni tranzistori

    DRIVE_A = 0;  
    DRIVE_B = 1;
    
    LATCbits.LATC13=0;
    LATCbits.LATC14=1;

    Delay(500); //cekamo jedno vreme da se odradi AD konverzija
	
    // ocitavamo y	
    y_vrednost = temp1;// temp1 je vrednost koji nam daje AD konvertor na LEFT pinu	
	
//Ako ?elimo da nam X i Y koordinate budu kao rezolucija ekrana 128x64 treba skalirati vrednosti x_vrednost i y_vrednost tako da budu u opsegu od 0-128 odnosno 0-64
//skaliranje x-koordinate
    X=(x_vrednost-161)*0.03629;
    // X= ((x_vrednost-AD_Xmin)/(AD_Xmax-AD_Xmin))*128;	
//vrednosti AD_Xmin i AD_Xmax su minimalne i maksimalne vrednosti koje daje AD konvertor za touch panel.
//Skaliranje y-koordinate
    Y=(y_vrednost-500)*0.020725;
    // Y= ((y_vrednost-AD_Ymin)/(AD_Ymax-AD_Ymin))*64;
}
	
void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{
    sirovi0=ADCBUF0;//0
    sirovi1=ADCBUF1;//1

    temp0=sirovi0;
    temp1=sirovi1;

    IFS0bits.ADIF = 0;
} 

// POTREBNO DA BI SE KALIBRISAO TOUCH SCREEN

void Write_GLCD(unsigned int data)
{
    unsigned char temp;
    temp=data/1000;
    Glcd_PutChar(temp+'0');
    data=data-temp*1000;
    temp=data/100;
    Glcd_PutChar(temp+'0');
    data=data-temp*100;
    temp=data/10;
    Glcd_PutChar(temp+'0');
    data=data-temp*10;
    Glcd_PutChar(data+'0');
}

enum STATE {START, TASTER2, TASTER3, TASTER4, KRAJ};
enum STATE stanje;

// SIFRA:
int password[4] = {9, 6, 3, 0};

int OcitajTaster()
{
    if((0<X)&&(X<14) && (0<Y)&&(Y<15))   return 1;
    if((15<X)&&(X<30) && (0<Y)&&(Y<15))   return 2;
    if((31<X)&&(X<46) && (0<Y)&&(Y<15))   return 3;
    if((0<X)&&(X<14) && (16<Y)&&(Y<32))   return 4;
    if((15<X)&&(X<30) && (16<Y)&&(Y<32))   return 5;
    if((31<X)&&(X<46) && (16<Y)&&(Y<32))   return 6;
    if((0<X)&&(X<14) && (33<Y)&&(Y<49))   return 7;
    if((15<X)&&(X<30) && (33<Y)&&(Y<49))   return 8;
    if((31<X)&&(X<46) && (33<Y)&&(Y<49))   return 9;
    if((15<X)&&(X<30) && (50<Y)&&(Y<66))   return 9;
    return 10;
}

int ProveraSifre()
{
    tacna_sifra=0;
    switch(stanje)
        {
            case START:
		if(OcitajTaster() == password[0])   stanje = TASTER2;
            /*    if(OcitajTaster() != 10 && OcitajTaster() !=password[0])    
                {
                    stanje = START;
                    Buzzer; 
                } */
            break;
					
            case TASTER2:
                if(OcitajTaster() == password[1])   stanje = TASTER3;
            /*    if(OcitajTaster() != 10 && OcitajTaster() !=password[1])    
                {
                    stanje = START;
                    Buzzer; 
                } */
            break;
                       
            case TASTER3:
                if(OcitajTaster() == password[2]) stanje = TASTER4;
            /*    if(OcitajTaster() != 10 && OcitajTaster() !=password[2])    
                {
                    stanje = START;
                    Buzzer; 
                } */
            break;
                    
            case TASTER4:
                if(OcitajTaster() == password[3]) stanje = KRAJ;
            /*    if(OcitajTaster() != 10 && OcitajTaster() !=password[3])    
                {
                    stanje = START;
                    Buzzer; 
                } */
            break;
            
            case KRAJ:
                tacna_sifra=1;
            break;
        }
    return tacna_sifra;
}


int ms, us;
int stanje1, pritisnut_taster1, stanje2, pritisnut_taster2;

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
    // otvaranje vrata pomocu servo motora ( -90 stepeni )
    LATBbits.LATB11 = 1;
    Delay_ms(2);
    LATBbits.LATB11 = 0;
    Delay_ms(18);
        
    while(!PORTCbits.RC15); // cekanje da se pritisne taster za zatvaranje vrata
        
    // zatvaranje vrata pomocu servo motora ( 90 stepeni )   
    LATBbits.LATB11 = 1;
    Delay_ms(1);
    LATBbits.LATB11 = 0;
    Delay_ms(19);
 }

// f-ja za buzzer
void Buzzer()
{
    for(int n=0; n<50; n++)
    {
        LATAbits.LATA11 = 1;
        Delay_us(300);
        LATAbits.LATA11 = 0;
        Delay_us(700);
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
    TRISCbits.TRISC15=1; // ulaz za taster kojim se bira zatvaraju vrata
     
    // za buzzer
    TRISAbits.TRISA11=0;// izlaz za pwm
    
    // za touch srceen
    ConfigureADCPins();
    ADCinit();
    ADCON1bits.ADON=1;
    TRISCbits.TRISC13=0;
    TRISCbits.TRISC14=0;
    stanje = START;
   
    // za tajmere
    Init_T1();
    Init_T2();
    
    while(1)
    {
    /*    DnevniScreensaver();
        while(!PirSenzor());
        GLCD_ClrScr();
        PocetniEkran(); */
        
       // Buzzer();
       // ServoMotor();
    }
    
    
    return (EXIT_SUCCESS);
}



