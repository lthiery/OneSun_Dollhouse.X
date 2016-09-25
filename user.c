/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/



#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */




void InitApp(void)
{
    
    TRISA = ~(~TRISC | 0b010000);   //set outputs for PORTA
    TRISC = ~(~TRISC | 0b110000);   //set outputs for PORTC
    
    RC4 = 1; //drive AMP_VDD
    
    TRISA   |=      0b110;  //configure port A inputs
    ANSELA  |=      0b110;  //set analog inputs on port A
    
    TRISC   |=      0b1100;  //configure port C inputs
    ANSELC  |=      0b1100;  //set analog inputs on port C
    

    /* Setup analog functionality and port direction */

    /* Initialize peripherals */

    /* Enable interrupts */
}

