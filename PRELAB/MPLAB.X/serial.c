/* 
 * File:   serial.c
 * Author: pablo
 *
 * Created on 13 de abril de 2023, 5:46 p.m.
 */

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF          // Code Protection bit (Program memory code protection is enabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


/**********LIBRERIAS**********/

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pic16f887.h>
#include "oscilador.h"

/**********DEFINIR CONSTANTES**********/

#define _XTAL_FREQ 8000000

/**********VARIABLES GLOBALES**********/

/**********PROTOTIPOS**********/
void setup(void);
void initUART(void);

/**********INTERRUPCIONES**********/

/**********CÓDIGO PRINCIPAL**********/


/*
 * 
 */
void main(void) 
{
    setup();
    setupOSC();
    initUART();
    while(1)   //loop principal
    { 
        PORTB ++;
        if (TXSTAbits.TRMT==1){
            TXREG = PORTB+33;
        }
        if (PIR1bits.RCIF ==1){
            PORTD = RCREG;
            RCIF = 0;
        }
        __delay_ms(500);
 
    }
}



/**********FUNCIONES**********/

void initUART(void){
    /*
     * 12.1.1.6 Asynchronous Transmission Set-up:
    1. Initialize the SPBRGH, SPBRG register pair and
    the BRGH and BRG16 bits to achieve the desired
    baud rate (see Section 12.3 ?EUSART Baud
    Rate Generator (BRG)?).
    2. Enable the asynchronous serial port by clearing
    the SYNC bit and setting the SPEN bit.
    3. If 9-bit transmission is desired, set the TX9 control bit. A set ninth data bit will indicate that the 8
    Least Significant data bits are an address when
    the receiver is set for address detection.
    4. Enable the transmission by setting the TXEN
    control bit. This will cause the TXIF interrupt bit
    to be set.
    5. If interrupts are desired, set the TXIE interrupt
    enable bit. An interrupt will occur immediately
    provided that the GIE and PEIE bits of the
    INTCON register are also set.
    6. If 9-bit transmission is selected, the ninth bit
    should be loaded into the TX9D data bit.
    7. Load 8-bit data into the TXREG register. This
    will start the transmission.
     */
    
    //paso1
    SPBRG = 12;
    SPBRGH = 0;
    BRGH = 0;
    BRG16 = 0;
    
    //paso2
    SYNC = 0;
    SPEN = 1; //Habilitación del modulo UART
    
    //paso3, son los 9 bits.
    
    
    //paso4
    TXEN = 1;
    TXIF = 0;
    // Asynchronous Reception Set-up
    CREN = 1;   //habilita la recepcion
    RCIF = 0;   //limpia bandera de interrupcion de datos recibidos
    
    //Paso 5> Interrupciones
    
    //Paso 6> cargar el 9no bit.
    
    //Paso7> cargar los 8 bits
    
    
    
}

void setup(void)
{
    //configuracion de entradas y salidas
    ANSELH = 0; //Configuracion Analogico/Digital
    ANSEL = 0;

    TRISB = 0; //Configuración Input/Output
    TRISD = 0;   
    
    PORTB = 0; //Inicialización en 0
    PORTD = 0;
   
    return;
}
    