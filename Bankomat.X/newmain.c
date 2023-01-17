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
#include "uart.h"

_FOSC(CSW_FSCM_OFF & HS3_PLL4);//instruction takt je isti kao i kristal
//_FOSC(CSW_FSCM_OFF & XT);//deli sa 4
_FWDT(WDT_OFF);
_FGS(CODE_PROT_OFF);

#define DRIVE_A PORTCbits.RC13
#define DRIVE_B PORTCbits.RC14

unsigned int X, Y, x_vrednost, y_vrednost;
const unsigned int AD_Xmin =234;
const unsigned int AD_Xmax =3612;
const unsigned int AD_Ymin =310;
const unsigned int AD_Ymax =3580;

unsigned int sirovi0, sirovi1, sirovi2; 

int ms, us, pola_milisekunde;
int stanje11, pritisnut_taster1, stanje12, pritisnut_taster2;

// definisanje stanja za unos pina 
enum STATE1 {START, TASTER2, TASTER3, TASTER4, END};
enum STATE1 stanje1;

enum STATE2 {pocetak, s1, s2, s3, kraj};
enum STATE2 stanje2;

int tacna_sifra, taster;
// SIFRA:
int password[4] = {9, 6, 3, 0};

unsigned char tempRX;

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

/*************************************************************/
// PREKIDNE RUTINE
/*************************************************************/

// prekidna rutina za tajmer 1
void __attribute__ ((__interrupt__)) _T1Interrupt(void) // svakih 1ms
{
   // WriteStringUART1("TAJMER 1 PREKID \n");
    TMR1 = 0;
    ms++;
    IFS0bits.T1IF = 0;     
}

// prekidna rutina za tajmer 2
void __attribute__ ((__interrupt__)) _T2Interrupt(void) // svakih 1us
{
  //  WriteStringUART1("TAJMER 2 PREKID \n");
    TMR2 = 0;
    us++;
    IFS0bits.T2IF = 0;     
}

// prekidna rutina za tajmer 3
void __attribute__ ((__interrupt__)) _T3Interrupt(void) // svakih 0.1us
{
  //  WriteStringUART1("TAJMER 3 PREKID \n");
    TMR3 = 0;
    pola_milisekunde++;
    IFS0bits.T3IF = 0;     
}

// prekidna rutina za AD konverziju
void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{
  //  WriteStringUART1("ADC PREKID\n");
    sirovi0=ADCBUF0;//0
    sirovi1=ADCBUF1;//1
    sirovi2=ADCBUF2; // za mq3
 
    ADCON1bits.ADON=0;
    IFS0bits.ADIF = 0;
} 

// prekidna rutina za UART
void __attribute__((__interrupt__)) _U1RXInterrupt(void) 
{
    IFS0bits.U1RXIF = 0;
    tempRX=U1RXREG;
} 

/*************************************************************/
// FUNKCIJE ZA TAJMERE
/*************************************************************/
void Delay_ms (int vreme);
void Delay_us (int vreme);
void Delay_500us (int vreme);
void Delay(unsigned int N);

/*************************************************************/
// FUNKCIJE ZA GLCD
/*************************************************************/
void WriteNumberonGLCD( int data);
void PocetniEkran();
void DnevniScreensaver();
void NocniScreensaver();

/*************************************************************/
// FUNKCIJE ZA BUZZER
/*************************************************************/
void Buzzer();

/*************************************************************/
// FUNKCIJE ZA PIR SENZOR
/*************************************************************/
int PirSenzor();

/*************************************************************/
// FUNKCIJE ZA TOUCH PANEL
/*************************************************************/
void TouchPanel();
int OcitajTaster();
int ProveraSifre();

/*************************************************************/
// FUNKCIJE ZA SERVO MOTOR
/*************************************************************/
void OtvoriVrata();
void ZatvoriVrata();

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
     
    // za buzzer
    TRISAbits.TRISA11=0;// izlaz za pwm
    
    // za touch srceen
    ConfigureADCPins(); // ovim se konfigurisao i pin RB10 za MQ3 senzor
    ADCinit();
    ADCON1bits.ADON=0;
    TRISCbits.TRISC13=0;
    TRISCbits.TRISC14=0;
    stanje1 = START;
   
    // za tajmere
    Init_T1();
    Init_T2();
    Init_T3();
    ZatvoriVrata();
    // za uart
    InitUART1();
    tempRX=48;
    WriteStringUART1("init\n");
    
    stanje2=pocetak;
        
    while(1)
    {
        switch(stanje2)
        {
            case pocetak:
                //fali implementacija fotootpornika
                DnevniScreensaver();
                while(!PirSenzor()); 
            //      WriteStringUART1("pir senzor dobar\n");
                stanje2=s1;
                GLCD_ClrScr();
                PocetniEkran();
            //      WriteStringUART1("tastatura\n");
            break;
        
            case s1:
                TouchPanel();
                if(ProveraSifre()==1)
                {
                    GLCD_ClrScr();
                    GoToXY(35,4);
                    GLCD_Printf("TACAN PIN");
                    stanje2=s2;
                }
            break;
        
            case s2:
                ADCON1bits.ADON=1;
                Delay_ms(10);
            //    WriteUART1dec2string(sirovi2);
                if(sirovi2>1500)
                { 
                    WriteStringUART1("Pijani ste! \n");
                    stanje2=pocetak;
                }
                else
                    stanje2=s3;    
            break;
            
            case s3:
                WriteStringUART1("Izaberite:\n");
                WriteStringUART1("  1. UPLATA\n");
                WriteStringUART1("  2. ISPLATA\n\n");
                while(tempRX!=49 && tempRX!=50);
                if(tempRX==49)   
                {
                    GLCD_ClrScr();
                    GoToXY(35,4);
                    GLCD_Printf("UPLATA");
                    stanje2=kraj;
                }
                if(tempRX==50)  
                {
                    GLCD_ClrScr();
                    GoToXY(35,4);
                    GLCD_Printf("ISPLATA");
                    stanje2=kraj;
                }
                tempRX=48;
            break;
                
            case kraj:
                OtvoriVrata();
                Delay_ms(500);
                ZatvoriVrata();

                GLCD_ClrScr();
                GoToXY(25,3);
                GLCD_Printf("Hvala Vam sto");
                GoToXY(25,4);
                GLCD_Printf("koristite nas");
                GoToXY(40,5);
                GLCD_Printf("bankomat");
                Delay_ms(5000);
                GLCD_ClrScr();
                stanje2=pocetak;
            break;
        }
    }
    return (EXIT_SUCCESS);
}

/*************************************************************/
// DEFINICIJE FUNKCIJA
/*************************************************************/

// funkcija za kasnjenje u milisekundama
void Delay_ms (int vreme)
{
    ms = 0;
    T1CONbits.TON = 1;
    while(ms < vreme);
    T1CONbits.TON = 0;
}

// funkcija za kasnjenje u mikrosekundama
void Delay_us (int vreme)
{
    us = 0;
    T2CONbits.TON = 1;
    while(us < vreme);
    T2CONbits.TON = 0;
}

// funkcija za kasnjenje u 0.1 milisekundi
void Delay_500us (int vreme)
{
    pola_milisekunde = 0;
    T3CONbits.TON = 1;
    while(pola_milisekunde < vreme);
    T3CONbits.TON = 0;
}

// f-ja za kasnjenje
void Delay(unsigned int N)
{
    unsigned int i;
    for(i=0;i<N;i++);
}
	
// f-ja za ispis jednocifrenog broja
void WriteNumberonGLCD(int data)
{
    char temp;
    temp=data;
    Glcd_PutChar(data+'0');
}

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

// f-ja za buzzer
void Buzzer()
{
    unsigned int n;
    for(n=0; n<300; n++)
    {
        LATAbits.LATA11 = 1;
        Delay_us(100);
        LATAbits.LATA11 = 0;
        Delay_us(900);
        LATAbits.LATA11 = 1;
    }
}

//f-ja za detektovanje pokreta pomocu pir senzora
int PirSenzor()
{
    if(PORTBbits.RB12==1)
        return 1; 
    else
        return 0;
}

// f-ja za ocitavanje polozaja pritiska na touch panelu
void TouchPanel (void)
{
    
// vode horizontalni tranzistori
    DRIVE_A = 1;  
    DRIVE_B = 0;
  
    ADCON1bits.ADON=1;
    Delay_ms(10); //cekamo jedno vreme da se odradi AD konverzija
				
    // ocitavamo x	
    x_vrednost = sirovi0;//temp0 je vrednost koji nam daje AD konvertor na BOTTOM pinu		

    //skaliranje x-koordinate
    X=(x_vrednost-234)*0.03789;
    // X= ((x_vrednost-AD_Xmin)/(AD_Xmax-AD_Xmin))*128;	
//vrednosti AD_Xmin i AD_Xmax su minimalne i maksimalne vrednosti koje daje AD konvertor za touch panel.
    
   
// vode vertikalni tranzistori

    DRIVE_A = 0;  
    DRIVE_B = 1;

    ADCON1bits.ADON=1;
    Delay_ms(10); //cekamo jedno vreme da se odradi AD konverzija
	
    // ocitavamo y	
    y_vrednost = sirovi1;// temp1 je vrednost koji nam daje AD konvertor na LEFT pinu	

    //skaliranje y-koordinate
    Y=(y_vrednost-310)*0.01957;
    // Y= ((y_vrednost-AD_Ymin)/(AD_Ymax-AD_Ymin))*64;
  
    taster=OcitajTaster();
    WriteStringUART1("X");
    WriteCharUART1(' ');
    WriteUART1dec2string(X);
    WriteStringUART1("Y");
    WriteCharUART1(' ');
    WriteUART1dec2string(Y);
    WriteStringUART1("      ");
    WriteStringUART1("\n");
}

// f-ja ocitavanja pritisnutog tastera
int OcitajTaster()
{
  //  WriteStringUART1("ocitavanje tastera\n");
    if((17<X)&&(X<31) && (0<Y)&&(Y<15))   return 0;
    if((32<X)&&(X<46) && (16<Y)&&(Y<31))  return 9;
    if((17<X)&&(X<31) && (16<Y)&&(Y<31))  return 8;
    if((2<X)&&(X<16) && (16<Y)&&(Y<31))   return 7;
    if((32<X)&&(X<46) && (32<Y)&&(Y<47))  return 6;
    if((17<X)&&(X<31) && (32<Y)&&(Y<47))  return 5;
    if((2<X)&&(X<16) && (32<Y)&&(Y<47))   return 4;
    if((32<X)&&(X<46) && (48<Y)&&(Y<64))  return 3;
    if((17<X)&&(X<31) && (48<Y)&&(Y<64))  return 2;
    if((2<X)&&(X<16) && (48<Y)&&(Y<64))   return 1;
    return 10;
}

// f-ja proveru tacnog pina
int ProveraSifre()
{
  //  WriteStringUART1("provera sifre\n");
    tacna_sifra=0;
    switch(stanje1)
        {
            case START:
                if(OcitajTaster() != 10)   
                {
                    GoToXY(70,5);
                    WriteNumberonGLCD(OcitajTaster());
                }
		if(OcitajTaster() == password[0])   stanje1 = TASTER2;
                if(OcitajTaster() != 10 && OcitajTaster() !=password[0])    
                {
                    stanje1 = START;
                    Buzzer(); 
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
            break;
					
            case TASTER2:
                if(OcitajTaster() != 10)   
                {
                    GoToXY(80,5);
                    WriteNumberonGLCD(OcitajTaster());
                }
                if(OcitajTaster() == password[1])   stanje1 = TASTER3;
                if(OcitajTaster() != 10 && OcitajTaster() !=password[1])    
                {
                    stanje1 = START;
                    Buzzer(); 
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
            break;
                       
            case TASTER3:
                if(OcitajTaster() != 10)   
                {
                    GoToXY(90,5);
                    WriteNumberonGLCD(OcitajTaster());
                }
                if(OcitajTaster() == password[2]) stanje1 = TASTER4;
                if(OcitajTaster() != 10 && OcitajTaster() !=password[2])    
                {
                    stanje1 = START;
                    Buzzer(); 
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
            break;
                    
            case TASTER4:
                if(OcitajTaster() != 10)   
                {
                    GoToXY(100,5);
                    WriteNumberonGLCD(OcitajTaster());
                }
                if(OcitajTaster() == password[3]) stanje1 = END;
                if(OcitajTaster() != 10 && OcitajTaster() !=password[3])    
                {
                    stanje1 = START;
                    Buzzer(); 
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
            break;
            
            case END:
                tacna_sifra = 1;
                stanje1 = START;
            break;
        }
    return tacna_sifra;
}

// f-ja za otvaranje vrata pomocu servo motora ( -90 stepeni )
void OtvoriVrata()
{
    unsigned int n;
    for(n=0; n<150; n++)
    {
        LATBbits.LATB11 = 1;
        Delay_ms(2);
        LATBbits.LATB11 = 0;
        Delay_ms(18);
        LATBbits.LATB11 = 1;
    }
}

// f-ja za zatvaranje vrata pomocu servo motora ( 90 stepeni )   
void ZatvoriVrata()
{
    unsigned int n;
    for(n=0; n<150; n++)
    {
        LATBbits.LATB11 = 1;
        Delay_ms(1);
        LATBbits.LATB11 = 0;
        Delay_ms(19);
        LATBbits.LATB11 = 1;
    }
}
