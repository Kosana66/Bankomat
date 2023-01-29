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

_FOSC(CSW_FSCM_OFF & HS3_PLL4);
_FWDT(WDT_OFF);
_FGS(CODE_PROT_OFF);

#define DRIVE_A LATCbits.LATC13
#define DRIVE_B LATCbits.LATC14

unsigned int X, Y, x_vrednost, y_vrednost;
const unsigned int AD_Xmin =220;
const unsigned int AD_Xmax =3642;
const unsigned int AD_Ymin =520;
const unsigned int AD_Ymax =3450;

unsigned int sirovi0, sirovi1, sirovi2, sirovi3; 
int ms, desetina_milisekunde;
unsigned char tempRX;

// definisanje stanja za unos pina 
enum STATE1 {START, TASTER2, TASTER3, TASTER4, END};
enum STATE1 stanje1;
// definisanje stanja za beskonacnu petlju
enum STATE2 {pocetak, s1, s2, s3, kraj};
enum STATE2 stanje2;

int tacna_sifra, taster;
// SIFRA:
int password[4] = {9, 6, 3, 0};

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

// prekidna rutina za tajmer 3
void __attribute__ ((__interrupt__)) _T3Interrupt(void) // svakih 0.1us
{
    TMR3 = 0;
    desetina_milisekunde++;
    IFS0bits.T3IF = 0;     
}

// prekidna rutina za AD konverziju
void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{
    sirovi0=ADCBUF1;//0
    sirovi1=ADCBUF2;//1
    sirovi2=ADCBUF3; // za mq3
    sirovi3=ADCBUF0; // za fotootpornik
 
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
void Delay_100us (int vreme);

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

    // inicijalizacija za glcd
    ConfigureLCDPins();
    GLCD_LcdInit();
    GLCD_ClrScr();
           
    // inicijalizacija za pir senzor
    ADPCFGbits.PCFG12=1; // digitalni
    TRISBbits.TRISB12=1;// ulaz
    
    // inicijalizacija za servo motor
    ADPCFGbits.PCFG11=1; // digitalni
    TRISBbits.TRISB11=0;// izlaz za pwm
     
    // inicijalizacija za buzzer
    TRISAbits.TRISA11=0;// izlaz za pwm
    
    // inicijalizacija za touch screen
    ConfigureADCPins(); // ovim se konfigurisao pin RB10 za MQ3 senzor i RB6 za fotootpornik
    ADCinit();
    ADCON1bits.ADON=0;
    TRISCbits.TRISC13=0;
    TRISCbits.TRISC14=0;
    stanje1 = START;
   
    // inicijalizacija za tajmere
    Init_T1();
    Init_T3();
    ZatvoriVrata();
    
    // inicijalizacija za uart
    InitUART1();
    tempRX=48;
    WriteStringUART1("Init\n");
    
    stanje2=pocetak;
        
    while(1)
    {
        switch(stanje2)
        {
            case pocetak:
                ADCON1bits.ADON=1;
                Delay_ms(10);
                if(sirovi3>2000)
                    NocniScreensaver();
                else 
                    DnevniScreensaver();
                
                while(!PORTBbits.RB12); 
                stanje2=s1;
                GLCD_ClrScr();
                PocetniEkran();
            break;
        
            case s1:
                TouchPanel();
                while(ProveraSifre()==0)
                    TouchPanel();
                
                GLCD_ClrScr();
                GoToXY(35,3);
                GLCD_Printf("TACAN PIN");
                Delay_ms(1000);
                stanje2=s2;
            break;
        
            case s2:
                ADCON1bits.ADON=1;
                Delay_ms(10);
                if(sirovi2>1500)
                { 
                    WriteStringUART1("Pijani ste ! \n");
                    WriteStringUART1("Dovidjenja ! \n");
                    stanje2=pocetak;
                }
                else
                {
                    WriteStringUART1("Niste pijani ! \n");
                    stanje2=s3;    
                }
            break;
            
            case s3:
                WriteStringUART1("Izaberite:\n");
                WriteStringUART1("  1. UPLATA\n");
                WriteStringUART1("  2. ISPLATA\n\n");
                while(tempRX!=49 && tempRX!=50);
                if(tempRX==49)   
                {
                    GLCD_ClrScr();
                    GoToXY(45,3);
                    GLCD_Printf("UPLATA");
                    stanje2=kraj;
                }
                if(tempRX==50)  
                {
                    GLCD_ClrScr();
                    GoToXY(45,3);
                    GLCD_Printf("ISPLATA");
                    stanje2=kraj;
                }
                tempRX=48;
            break;
                
            case kraj:
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

// funkcija za kasnjenje od 0.1 milisekundi
void Delay_100us (int vreme)
{
    desetina_milisekunde = 0;
    T3CONbits.TON = 1;
    while(desetina_milisekunde < vreme);
    T3CONbits.TON = 0;
}
	
// f-ja za ispis jednocifrenog broja na GLCD
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
    for(n=0; n<150; n++)
    {
        LATAbits.LATA11 = 1;
        Delay_100us(1);
        LATAbits.LATA11 = 0;
        Delay_100us(9);
        LATAbits.LATA11 = 1;
    }
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
    X=(x_vrednost-220)*0.037405;
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
    Y=(y_vrednost-520)*0.02184;
    // Y= ((y_vrednost-AD_Ymin)/(AD_Ymax-AD_Ymin))*64;
  
    taster=OcitajTaster();
}

// f-ja ocitavanja pritisnutog tastera
int OcitajTaster()
{
    if((14<X)&&(X<25) && (0<Y)&&(Y<14))   return 0;
    if((26<X)&&(X<38) && (15<Y)&&(Y<29))  return 9;
    if((14<X)&&(X<25) && (15<Y)&&(Y<29))  return 8;
    if((2<X)&&(X<13) && (15<Y)&&(Y<29))   return 7;
    if((26<X)&&(X<38) && (30<Y)&&(Y<43))  return 6;
    if((14<X)&&(X<25) && (30<Y)&&(Y<43))  return 5;
    if((2<X)&&(X<13) && (30<Y)&&(Y<43))   return 4;
    if((26<X)&&(X<38) && (44<Y)&&(Y<58))  return 3;
    if((14<X)&&(X<25) && (44<Y)&&(Y<58))  return 2;
    if((2<X)&&(X<13) && (44<Y)&&(Y<58))   return 1;
    return 10;
}

// f-ja proveru tacnog pina
int ProveraSifre()
{
    tacna_sifra=0;
    switch(stanje1)
        {
            case START:
                if(taster != 10)   
                {
                    GoToXY(70,5);
                    WriteNumberonGLCD(taster);
                }
		if(taster == password[0])   
                {
                    stanje1 = TASTER2;
                    Delay_ms(500);
                }
                if(taster != 10 && taster !=password[0])    
                {
                    stanje1 = START;
                    Buzzer(); 
                    Delay_ms(1000);
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
            break;
					
            case TASTER2:
                if(taster != 10)   
                {
                    GoToXY(80,5);
                    WriteNumberonGLCD(taster);
                }
                if(taster == password[1])   
                {
                    stanje1 = TASTER3;
                    Delay_ms(500);
                }
                if(taster != 10 && taster !=password[1])    
                {
                    stanje1 = START;
                    Buzzer(); 
                    Delay_ms(1000);
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
            break;
                       
            case TASTER3:
                if(taster != 10)   
                {
                    GoToXY(90,5);
                    WriteNumberonGLCD(taster);
                }
                if(taster == password[2]) 
                {
                    stanje1 = TASTER4;
                    Delay_ms(500);
                }
                if(taster != 10 && taster !=password[2])    
                {
                    stanje1 = START;
                    Buzzer(); 
                    Delay_ms(1000);
                    GLCD_ClrScr();
                    PocetniEkran(); 
                } 
            break;
                    
            case TASTER4:
                if(taster != 10)   
                {
                    GoToXY(100,5);
                    WriteNumberonGLCD(taster);
                }
                if(taster == password[3]) 
                {
                    stanje1 = END;
                    Delay_ms(500);
                }
                if(taster != 10 && taster !=password[3])    
                {
                    stanje1 = START;
                    Buzzer(); 
                    Delay_ms(1000);
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
        Delay_100us(27);
        LATBbits.LATB11 = 0;
        Delay_100us(173);
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
        Delay_100us(6);
        LATBbits.LATB11 = 0;
        Delay_100us(194);
        LATBbits.LATB11 = 1;
    }
}
