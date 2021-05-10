/*
 * File:   newmain.c
 * Author: mustafacan
 *
 * Created on 09 May?s 2021 Pazar, 15:38
 */


#include <xc.h>
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (Power-up Timer is disabled)
#pragma config CP = OFF         // Code Protection bit (Code protection disabled)

#define _XTAL_FREQ 4000000   
/* prescaler clock freq Xt_f / 4  so 4MHz/4 = 1MHz
 * prescaler rate is equal to 1:256 so timer ticks speed is 1Mhz/256 == 1us/256
 * 1/256us is pretty enought for this project
 */

int count;
int distanceSensor1;
int parkingMode = 0;
int speedIsSlow = 0;
void __interrupt() ISR(){
    if(T0IF){ // 8-bit timer interrputs every 1us
        count += 1;
        T0IF = 0; 
        TMR0 = 0;
    }
    if(RBIF){ // At least one of the RB7:RB4 pins changed state (must be cleared in software)
        int countPrev = count;
        if(RB4 == 1){ // Grove Ultrasonik Mesafe Sensörü V2.0 devreye girince
            distanceSensor1 = 0;
        }
        if(RB4 == 0){ // sensor veri almayı kesince
            distanceSensor1 = count - countPrev;
            count = 0;
            TMR0 = 0;
        }
    }
}

void main(){
    TRISA = 0x11 // ---10001 RA4 set as clock input RA0 set as start parking button input
    PORTA = 0x00; // default 0 for cleaning
    PORTB = 0x00; // default 0 for cleaning 
    
    T0CS = 0; // Internal instruction cycle clock (CLKOUT)
    T0SE = 0; // Increment on low-to-high transition on RA4/T0CKI pin 
    PSA = 0; // Prescaler is assigned to the Timer0 module
    PS2 = 1; //PS2-PS1-PS0 == 111 prescaler rate is 1:256 
    PS1 = 1;
    PS0 = 1;
    RBIE = 1; // Enables the RB port change interrupt
    TMR0 = 0; //timer is set as 0
    
    while(1){
        if(!parkingMode){ // if Driving mode is active
            if(RA0 && (speedIsSlow)){ // Enable parking mode
            __delay_ms(1000);
                GIE  = 1; // Enables all unmasked interrupts
                T0IE = 1; // Enables the TMR0 interrupt
                parkingMode = 1;
            __delay_ms(1000);
            }
        }
        if(parkingMode && RA0){
            __delay_ms(1000);
            GIE  = 0; // Enables all unmasked interrupts
            T0IE = 0; // Enables the TMR0 interrupt
            TMR0 = 0; //timer is set as 0
            parkingMode = 0;
            __delay_ms(1000);
            }
        if(parkingMode && !RA0){
            
        }
    // mesafe bilgisi burada kullanılacak    
    }
}
