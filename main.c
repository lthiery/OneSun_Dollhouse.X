/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (ECH, External Clock, High Power Mode (4-20 MHz): device clock supplied to CLKIN pin)
#pragma config WDTE = ON        // Watchdog Timer Enable (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#define _XTAL_FREQ 800000

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

void configureADC(void);
void selectADC(uint8_t channel);
void convertADC(void);
bool completeADC(void);
uint16_t getResultADC();
void InitNCO();
uint16_t avg(uint16_t * samples);

#define MPPT_STEP   0xF

#define NUM_ADC_SAMPLES         16  //ADC is 10-bit and we are using 16-bit variable to store

#define NUM_SAMPLES             8   //NUM_SAMPLES is 2^N
#define AVG_SAMPLES_SHIFT       3   //so avg samples with shift of N bits

#define VIN_CHANNEL             2
#define IIN_CHANNEL             6

#define VOUT_CHANNEL            
#define IOUT_CHANNEL            

uint16_t vin_samples[NUM_SAMPLES];
uint16_t iin_samples[NUM_SAMPLES];
uint8_t sample_pointer=0;
    
uint16_t curPV = 0;
uint16_t curPI = 0;
int32_t curFreq = 0x0;
/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();
    InitNCO();
    configureADC();
    
    RC5=0;
    CLRWDT();
    __delay_ms(1000);
    CLRWDT();
    __delay_ms(1000);

        
    while(1)
    {
        CLRWDT();
        
        curPV=0;
        curPI=0;
        for(uint8_t i=0; i<NUM_ADC_SAMPLES;i++){
            
            selectADC(VIN_CHANNEL);
            convertADC();
            while(!completeADC());
            curPV+=getResultADC();
            
            selectADC(IIN_CHANNEL);
            convertADC();
            while(!completeADC());
            curPI += getResultADC();
        }
 
        if((float) avg(&vin_samples)* avg(&iin_samples) > (float) curPV * curPI){
            RC5=0;
            if(curFreq-MPPT_STEP>0)         curFreq-=   MPPT_STEP;
            else                            curFreq=    0;

        }
        else{
            RC5=1;
            if(curFreq+MPPT_STEP<0xFFF)     curFreq+=   MPPT_STEP;
            else                            curFreq =   0xFFFF;
        }
        
        vin_samples[sample_pointer]=curPV;
        iin_samples[sample_pointer]=curPI;
        if(++sample_pointer==NUM_SAMPLES) sample_pointer = 0;

        NCO1INCH = curFreq>>8;
        NCO1INCL = curFreq&0xFF;
        __delay_ms(50);
    }
}

uint16_t avg(uint16_t * samples){
    uint32_t avg = 0;
    for(uint8_t i=0; i<NUM_SAMPLES; i++){
        avg += samples[i];
    }
    return avg>>AVG_SAMPLES_SHIFT;
}

void configureADC(void){
    ADCON1 = 0b01110000;
}

void selectADC(uint8_t channel){
    //make sure channel bits smaller than 7
    channel &=  0b111; 
    //clear channel selection bits (ie: ADCONX[7:2])
    ADCON0  &=  (2<<0b000);          
    //set channel selection bits;
    ADCON0  |=  (2<<channel);
}

void convertADC(){
    ADCON0 |=0b11;
}

bool completeADC(){
    return  !((ADCON0>>1)&0b1); //bit 1 is 0 when done
}

uint16_t getResultADC(){
    return (ADRESH<<2)|(ADRESL>>6);
}

void InitNCO(){
    APFCON |= 0b1;          //make RA4->NC0
 
    INTCON |= 0b11000000;   //enabled GIE and PIEI
    //PIE2 |= 0b100;
    NCO1CON = 0b11001110;   //
    NCO1CLK = 0b00;         //select internal clock
}