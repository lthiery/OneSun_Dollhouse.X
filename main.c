/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

// CONFIG1
#pragma config FOSC = INTOSC       // Oscillator Selection Bits (ECH, External Clock, High Power Mode (4-20 MHz): device clock supplied to CLKIN pin)
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

/* i.e. uint8_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void selectADC_IntOsc(){
    ADCON1 |= (0b111 <<4);
}

void selectADC_5VREF(){
    ADCON1 &= 0b11111000; //clear first 3 bits
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
    return (ADCON0>>1)&0b1;
}

uint16_t getResultADC(){
    return (ADRESH<<2)|(ADRESL>>6);
}

void main(void)
{
    /* Configure the oscillator for the device */
    //ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    while(1)
    {
        //LED_status=~LED_status;
        //RC5 = LED_status; //Makes 5th bit of PORTC at Logic High
 
        
        //read RA2 / AN2
        
        selectADC_IntOsc();
        selectADC_5VREF();
        selectADC(2);
        
        convertADC();
     
        while(!completeADC);
 
        if(getResultADC()>220){
            RC5=1;
        }
        else{
            RC5=0;
        }
         
        __delay_ms(500);
        
         
         
         
    }
}

