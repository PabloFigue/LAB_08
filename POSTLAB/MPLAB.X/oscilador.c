/**********LIBRERIAS**********/

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pic16f887.h>
#include "oscilador.h"

void setupOSC(void){
    OSCCON = 0x75;
    return;
}
