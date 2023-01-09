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

_FOSC(CSW_ON_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal
//_FOSC(CSW_FSCM_OFF & XT);//deli sa 4
_FWDT(WDT_OFF);
_FGS(CODE_PROT_OFF);

#define DRIVE_A PORTCbits.RC13
#define DRIVE_B PORTCbits.RC14

unsigned int X, Y, x_vrednost, y_vrednost;
const unsigned int AD_Xmin =233;
const unsigned int AD_Xmax =3626;
const unsigned int AD_Ymin =310;
const unsigned int AD_Ymax =3515;

unsigned int temp0, temp1, temp2, sirovi0, sirovi1, sirovi2; 

int ms, us;
int stanje1, pritisnut_taster1, stanje2, pritisnut_taster2;

// definisanje stanja za unos pina 
enum STATE {START, TASTER2, TASTER3, TASTER4, KRAJ};
enum STATE stanje;

int tacna_sifra;
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
    TMR1 = 0;
    ms++;
    IFS0bits.T1IF = 0;     
}

// prekidna rutina za tajmer 2
void __attribute__ ((__interrupt__)) _T2Interrupt(void) // svakih 1us
{
    TMR2 = 0;
    us++;
    IFS0bits.T2IF = 0;     
}

// prekidna rutina za AD konverziju
void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{
    sirovi0=ADCBUF0;//0
    sirovi1=ADCBUF1;//1
    sirovi2=ADCBUF2;
             
    temp0=sirovi0;
    temp1=sirovi1;
    temp2=sirovi2;
    
    IFS0bits.ADIF = 0;
} 

// prekidna rutina za UART
void __attribute__((__interrupt__)) _U1RXInterrupt(void) 
{
    IFS0bits.U1RXIF = 0;
    tempRX = 0;
    tempRX=U1RXREG;
} 

/*************************************************************/
// FUNKCIJE ZA TAJMERE
/*************************************************************/
void Delay_ms (int vreme);
void Delay_us (int vreme);
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
    TRISCbits.TRISC15=1; // ulaz za taster kojim se bira zatvaraju vrata
     
    // za buzzer
    TRISAbits.TRISA11=0;// izlaz za pwm
    
    // za touch srceen
    ConfigureADCPins(); // ovim se konfigurisao i pin RB10 za MQ3 senzor
    ADCinit();
    ADCON1bits.ADON=1;
    TRISCbits.TRISC13=0;
    TRISCbits.TRISC14=0;
    stanje = START;
   
    // za tajmere
    Init_T1();
    Init_T2();
    
    // za uart
    InitUART1();
    tempRX=0;
    
    DnevniScreensaver();
    while(!PirSenzor()); 
    GLCD_ClrScr();
    PocetniEkran();
    
    while(1)
    {
       
        TouchPanel();
        if(ProveraSifre()==1)
        {
            GLCD_ClrScr();
            GoToXY(35,4);
            GLCD_Printf("TACAN PIN");
            while(tempRX==0);
            GoToXY(35,5);
            WriteNumberonGLCD(tempRX);
            WriteStringUART1("Izaberite:\n");
            WriteStringUART1("1.UPLATA\n");
            WriteStringUART1("2.ISPLATA\n\n");
            while(tempRX!=1 && tempRX!=2);
            if(tempRX==1)   
            {
                WriteStringUART1("---> Korisnik je pritisnuo uplatu\n");
                GLCD_ClrScr();
                GoToXY(35,4);
                GLCD_Printf("UPLATA");
            }
            if(tempRX==2)  
            {
                WriteStringUART1("---> Korisnik je pritisnuo isplatu\n");
                GLCD_ClrScr();
                GoToXY(35,4);
                GLCD_Printf("ISPLATA");
            }
            tempRX=0;
            OtvoriVrata();
            Delay_ms(10000);
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
            PocetniEkran();
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
    while(ms < vreme);
}

// funkcija za kasnjenje u mikrosekundama
void Delay_us (int vreme)
{
    us = 0;
    while(us < vreme);
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
    for(int n=0; n<300; n++)
    {
        LATAbits.LATA11 = 1;
        Delay_us(100);
        LATAbits.LATA11 = 0;
        Delay_us(900);
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
    X=(x_vrednost-233)*0.03772;
    // X= ((x_vrednost-AD_Xmin)/(AD_Xmax-AD_Xmin))*128;	
//vrednosti AD_Xmin i AD_Xmax su minimalne i maksimalne vrednosti koje daje AD konvertor za touch panel.
//Skaliranje y-koordinate
    Y=(y_vrednost-310)*0.01997;
    // Y= ((y_vrednost-AD_Ymin)/(AD_Ymax-AD_Ymin))*64;
}

// f-ja ocitavanja pritisnutog tastera
int OcitajTaster()
{
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
    tacna_sifra=0;
    switch(stanje)
        {
            case START:
                if(OcitajTaster() != 10)   
                {
                    GoToXY(70,5);
                    WriteNumberonGLCD(OcitajTaster());
                }
		if(OcitajTaster() == password[0])   stanje = TASTER2;
                if(OcitajTaster() != 10 && OcitajTaster() !=password[0])    
                {
                    stanje = START;
                    Buzzer(); 
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
                Delay_ms(30);
            break;
					
            case TASTER2:
                if(OcitajTaster() != 10)   
                {
                    GoToXY(80,5);
                    WriteNumberonGLCD(OcitajTaster());
                }
                if(OcitajTaster() == password[1])   stanje = TASTER3;
                if(OcitajTaster() != 10 && OcitajTaster() !=password[1])    
                {
                    stanje = START;
                    Buzzer(); 
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
                Delay_ms(30);
            break;
                       
            case TASTER3:
                if(OcitajTaster() != 10)   
                {
                    GoToXY(90,5);
                    WriteNumberonGLCD(OcitajTaster());
                }
                if(OcitajTaster() == password[2]) stanje = TASTER4;
                if(OcitajTaster() != 10 && OcitajTaster() !=password[2])    
                {
                    stanje = START;
                    Buzzer(); 
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
                Delay_ms(30);
            break;
                    
            case TASTER4:
                if(OcitajTaster() != 10)   
                {
                    GoToXY(100,5);
                    WriteNumberonGLCD(OcitajTaster());
                }
                if(OcitajTaster() == password[3]) stanje = KRAJ;
                if(OcitajTaster() != 10 && OcitajTaster() !=password[3])    
                {
                    stanje = START;
                    Buzzer(); 
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
                Delay_ms(30);
            break;
            
            case KRAJ:
                tacna_sifra = 1;
                stanje = START;
            break;
        }
    return tacna_sifra;
}

// f-ja za otvaranje vrata pomocu servo motora ( -90 stepeni )
void OtvoriVrata()
{
    LATBbits.LATB11 = 1;
    Delay_ms(2);
    LATBbits.LATB11 = 0;
    Delay_ms(18);
}

// f-ja za zatvaranje vrata pomocu servo motora ( 90 stepeni )   
void ZatvoriVrata()
{
    LATBbits.LATB11 = 1;
    Delay_ms(1);
    LATBbits.LATB11 = 0;
    Delay_ms(19);
}
