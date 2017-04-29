/*
 * File:   main.c
 * Author: jun1ro1
 * Processor: PIC10F322
 * Created on August 12, 2016, 3:08 PM
 */

// PIC10F322 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG
#pragma config FOSC = INTOSC    // Oscillator Selection bits (INTOSC oscillator: CLKIN function disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config WDTE = SWDTEN    // Watchdog Timer Enable (WDT controlled by the SWDTEN bit in the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)
#pragma config LPBOR = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)

// Clock Frequency 1MHz in Hz for __delay_ms() function
#define _XTAL_FREQ  1000000

// define registers
#define RA0_LED     RA0
#define RA1_PTR     RA1

/*
Register    I/O     Purpose
RA0         Output  LED Power
RA1         Output  Photo Transister Power
RA2         Input   Photo Transister Voltage
 */

void main(void) {

    // Clock frequency 1MHz
    OSCCONbits.IRCF = 0b011;

    ANSELA = 0x04; // RA2:Analog, Others:Digital
    TRISA = 0x04; // RA2:Input,  Others:Output

    RA0_LED = 0; // LED on
    __delay_ms(1000); //LED on for a test
    RA0_LED = 1; // LED off

    WDTCON = (0b01011 << 1) | 0x01; // WDT interval 2s in 1MHz and WDT on

    while (1) {
        CLRWDT(); // clear WDT
        SLEEP(); // wait for WDT timer interval = 2[sec]
        NOP();

        RA1_PTR = 1; // photo transistor power on
        __delay_ms(10); // wait for the photo transistor response time

        ADCONbits.ADON = 1; // enable ADC
        ADCONbits.ADCS = 0b000; // ADC Clock Period = Fosc/2 = 2.0 usec
        ADCONbits.CHS  = 0b010; // AN2 selected

        ADCONbits.GO_nDONE = 1; // start AD conversion
        while (ADCONbits.GO_nDONE);
        int val = ADRES;
        ADCONbits.ADON = 0; // disable ADC

        RA1_PTR = 0; // photo transistor power off

        if (val < 10) {
            // when digital input is low (photo transistor is off)
            for(int i = 0; i < 16; i++) {
                RA0_LED = 0; // LED on  for 15[msec]]
                __delay_ms(15);
                RA0_LED = 1; // LED off for 15[msec]
                __delay_ms(15);
            }
        }
        else {
            __delay_ms(480); // wait for 30 x 16 = 480[msec]
        }

        RA0_LED = 1; // LED off
    }

    return;
}
