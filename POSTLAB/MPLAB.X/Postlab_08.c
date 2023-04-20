/* 
 * File:   Postlab_08.c
 * Author: pablo
 *
 * Created on 19 de abril de 2023, 7:41 p.m.
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
#include <string.h>

/**********DEFINIR CONSTANTES**********/

#define _tmr0_value 61
#define _XTAL_FREQ 8000000

/**********VARIABLES GLOBALES**********/

int valPOT;
int status;
unsigned int numero;

/**********PROTOTIPOS**********/
void setup(void);
void initUART(void);
void cadena(char txt[]);
void enviar_numero_decimal(unsigned int numero);

/**********INTERRUPCIONES**********/

void __interrupt () isr(void)
{
    if(T0IF)    //tmr0
    { 
        TMR0 = _tmr0_value;
        INTCONbits.T0IF = 0;
    }
    if (ADIF)   //Interrupcion del ADC
    { 
        numero = ADRESH;
        PIR1bits.ADIF = 0;
    }
    return;
}

/**********C�DIGO PRINCIPAL**********/
/*
 * 
 */
void main(void) 
{
    setup();
    initUART();
    while(1)   //loop principal
    { 
         cadena("\r\n");
        cadena("\r\n\nEscoja una de las siguientes funciones:\r\n");
        cadena("1 - leer Potenciometro\r\n");
        cadena("2 - Enviar ASCII al PORTB\r\n");
        status =1;
        while (status == 1){    //While que se ejecuta hasta que se presione 1 o 2 que es ASCII representa 49 o 50.
            if (RCREG == 49){
                status = 0;
            }
            else if (RCREG == 50){
                status = 0;
            }   
        }
            if (RCREG == 49){ 
                ADCON0bits.GO = 1;      //Iniciar a convertir
                while (ADIF == 0){;}    //Esperar hasta que haya terminado de convertir
                __delay_us(2);
                enviar_numero_decimal(numero);  //Conversion del numero binario a cadena de caracteres.
                RCIF = 0;       //Se limpia la bandera de recepcion de datos
                RCREG =0;       //Se limpia el valor recibido.
            } 
            else if(RCREG == 50){
                PORTB = 0;  //Se limpia el puerto para cargar el nuevo ASCII
                RCIF = 0;   //Se limpia la bandera que se activo al elegir la opcion del menu
                cadena("Ingrese el valor del ASCII:  ");    //Mostrar texto hasta que se ingrese un nuevo dato 
                while (RCIF == 0){;} 
                TXREG = RCREG;  //Mostrar en la terminal virtual cual fue el valor recibido
                PORTB = RCREG;  //Mostrar el valor ASCII en binario en el puertoB
                RCREG =0;       //Se limpia el valor recibido.
                RCIF = 0;       //Se limpia la banderaa de recepcion de datos.
            }
            __delay_ms(100);
        }
    }   


/**********FUNCIONES**********/
void enviar_numero_decimal(unsigned int numero) {
    char buffer[6]; // Buffer para almacenar la cadena formateada
    sprintf(buffer, "%u", numero); // Formatear el n�mero como una cadena de caracteres
    for (int i = 0; i < 6; i++) {
        TXREG = buffer[i]; // Copiar cada car�cter de la cadena en el registro TXREG
        __delay_ms(4); // Esperar a que se complete la transmisi�n
    }
}

void cadena(char txt[])
{
    int i;
    for(i = 0; i < strlen(txt); i++) {  // For del tama�o de la cadena de caracteres
        TXREG = txt[i];     //Copiar cada caracter de la cadena en el registrto TXREG
        __delay_ms(5);      // Esperar a que se complete la transmisi�n
    }
}

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
    SPBRG = 12;     //SPBRGH:SPBRG = (8MHz/9600)/64 -1 = 12 ---> Real 9615.38
    SPBRGH = 0;     //%error = (9615.38-9600)/9600*100 = 0.16%
    BRGH = 0;
    BRG16 = 0;
    
    //paso2
    SYNC = 0;   //Modo asincronico habilitado
    SPEN = 1;   //Habilitaci�n del modulo UART
    
    //paso3, habilitacion de los 9 bits.
    
    
    //paso4 Habilitacion de la transmicion
    TXEN = 1; //Habilitar transmision & TXIF = 1
    TXIF = 0; //Apagar la bandera TX
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

    TRISB = 0; //Configuraci�n Input/Output
    TRISD = 0;   
    
    PORTB = 0; //Inicializaci�n en 0
    PORTD = 0;
    PORTA = 0;
    
    OSCCONbits.IRCF = 0b111;    //8MHz
    OSCCONbits.SCS = 1;         //prescaler al TMR0
    
    //configuraci�n tmr0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b111;
    TMR0 = _tmr0_value;
    
    //Configuraci�n de las interrupciones
    INTCONbits.T0IF = 0; // Limpiar la bandera de interrupcion del TMR0
    INTCONbits.T0IE = 1; // Habilitacion de la interrupcion del TMR0
    PIR1bits.ADIF = 0;   // limpiar la bandera de interrupcion del ADC
    PIE1bits.ADIE = 1;   // habilitar interrupciones de ADC
    INTCONbits.GIE = 1;  // Usualmente la global se enciende de [ultimo]
   // INTCONbits.PEIE = 1; // habilitar interrupciones perifericas
    
    //Configuracion ADC
    ANSEL = 0b00000001;
    TRISA = 0b00000001;
    ADCON0bits.ADCS = 0b10;     //Conversion Clock
    ADCON0bits.CHS = 0b0000;    //Inicializaci�n en el Canal 0
    __delay_ms(2);
    ADCON1bits.ADFM = 0;    //justificado a la izquierda
    ADCON1bits.VCFG0 = 0;   //Vdd
    ADCON1bits.VCFG1 = 0;   //Vss
    ADCON0bits.ADON = 1;    //ADC enable
    ADIF =  0;
    
    return;
}
    