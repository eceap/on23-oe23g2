#include <xc.h>
#include "system.h"
#include "tick.h"

volatile tick_t tickCounter;    //Variable global que almacena el tick count

void tickInit(void) {
//Configura Timer0 para desbordar aproximadamente cada 1 mSeg
    T0CS = 0;       //Internal instruction cycle clock (FOSC/4)
    PSA = 0;        //Prescaler Assignment bit
                    // 1 = Prescaler is assigned to the WDT
                    // 0 = Prescaler is assigned to the Timer0 module
    T0PS0 = 1;
    T0PS1 = 0;
    T0PS2 = 0;
                            
    TMR0IE = 1;     //Habilita interrupción de TMR0
}

tick_t tickRead( void ) {
    tick_t aux;
    GIE = 0;                        //Inicia Zona critica
    aux = tickCounter;              //  Operaciones atomicas
    GIE = 1;                        //Finaliza la zona critica
    return aux;
}

void tickWrite( tick_t ticks ){
    GIE = 0;                        //Inicia Zona critica
    tickCounter = ticks;            //  Operaciones atomicas
    GIE = 1;     
}