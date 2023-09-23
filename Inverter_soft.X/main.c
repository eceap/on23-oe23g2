#include <xc.h>         /* Archivo de Encabezados General XC8 */
#include <stdint.h>     /* Para las definiciones de uint8_t por ej.*/
#include "system.h"     /* Funciones/Parametros System, como osc/peripheral config */
#include "user.h"       /* Funciones/Parametros User, como InitApp */
#include "uart.h"       /* Funciones/Parametros Uart */
#include "adc.h"        /* Funciones/parametros del ADC*/
#include "tick.h"       /* Funciones/Parametros Tick */
#include <stdio.h>      /* Archivo de Encabezados StdIO */

/*=======================[definiciones & Macros]=============================*/

#define Vmax 3828 // tension maxima en mV
#define Vmin 2470 // tension minima en mV

typedef enum {INICIAL, CORRECTO, FALLAS, DIAGNOSTICO} estadoMEF_t;

/*==================[definiciones de datos internos]=========================*/
estadoMEF_t estadoActual; // Variable de estado (global)
tick_t tInicio, tQB_CI, tQA_CI, tQB, tQA, tQB_CI_0, tQA_CI_0, tQB_0, tQA_0;
/*NOTA:
 *Los tiempos terminados en "_0" marcan el tiempo desde la ultima vez que 
 * el pin estuvo en estado logico bajo. Y los que no terminan en nada, 
 * exactamente lo contrario.
 */
//Mensajes de puerto serie
unsigned char MSJ_FallaFuse[] = {"FalloElFusible-"};
unsigned char MSJ_FallaCI[] = {"FalloElIntegrado-"};             
unsigned char MSJ_FallaRegulador[] = {"FalloElRegulador-"};     //Arrays de caracteres
unsigned char MSJ_FallaTrafPrim[] = {"FalloElTransformador1-"}; //Para puerto serie 
unsigned char MSJ_FallaTrafSecu[] = {"FalloElTransformador2-"};
unsigned char MSJ_FallaGeneral[] = {"FallaGeneral-"};
unsigned char MSJ_Normal[] = {"TodoCorrecto-"};
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void InicializarMEF(void);
void ActualizarMEF(void);
void apagarLedsFallas(void);
uint8_t diagnosticar(void);
/*==================[Definicion de Variables]================================*/

__bit FallaFuse, FallaCI, FallaRegulador, FallaTrafPrim, FallaTrafSecu, FallaGeneral; //Flags de fallas
/*==================[funcion principal]======================================*/
void main(void) {

    appInit(); /* Inicializa I/O y Periféricos de la aplicación */
    InicializarMEF();

    while (1) {
        ActualizarMEF();
        CLRWDT(); //Reinicia el WDT
    }
}
/*==================[definiciones de funciones internas]=====================*/
//Función Inicializa MEF

void InicializarMEF(void) {
    estadoActual = INICIAL; // Establece estado inicial 
    tInicio = tickRead(); // También inicia temporizacion 
    tQA = tInicio;
    tQA_0 = tInicio;
    tQB = tInicio;
    tQB_0 = tInicio;
    tQA_CI = tInicio;
    tQA_CI_0 = tInicio;
    tQB_CI = tInicio;
    tQB_CI_0 = tInicio;
}

//Función Actualizar MEF

void ActualizarMEF(void) {
    switch (estadoActual) {
        case INICIAL:
            PIN_LED_INICIAL = 1; //Prendo LED indicador de etapa de inicializacion
            if (!PIN_OPTO) {
                estadoActual = CORRECTO; //Se detecto tension a la salida del trafo, levantó correctamente el circuito
                PIN_LED_INICIAL = 0;
                tInicio = tickRead(); // guardo el tiempo de inicio 
            }
            if (((tickRead() - tInicio) > 5000) && PIN_OPTO) {
                estadoActual = FALLAS; // No se detectó tension en la salida del trafo pasados 5 segundos
                PIN_LED_INICIAL = 0;
            }
            break;

        case CORRECTO:
            apagarLedsFallas(); //Apago LEDs indicadores de fallas
            PIN_LED_NORMAL = 1; //Prendo LED indicador de estado de respuesta normal
            for (uint8_t i = 0; i < sizeof MSJ_Normal; i++) {
                uart2WriteByte(MSJ_Normal[i]);
            }
            if (diagnosticar() == 1) {
                estadoActual = FALLAS;
                PIN_BUZZER = 1;
            }
            break;

        case FALLAS:
            PIN_LED_NORMAL = 0; //Apago el LED indicador del estado CORRECTO
            PIN_LED_FALLA_GENERAL = 1; //Prendo LED indicador del estado Falla General
            if (FallaRegulador == 1) {
                for (uint8_t i = 0; i < sizeof MSJ_FallaRegulador; i++) {
                    uart2WriteByte(MSJ_FallaRegulador[i]);
                }
                for (uint8_t i = 0; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                }
                PIN_LED_FALLA_ZENER = 1;
            }
            if (FallaFuse == 1) { //El LED de esta falla se prende por Hardware
                for (uint8_t i = 0; i < sizeof MSJ_FallaFuse; i++) {
                    uart2WriteByte(MSJ_FallaFuse[i]);
                }
                for (uint8_t i = 0; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                } //Mando mensajes informando el error
            }
            if (FallaCI == 1) {
                for (uint8_t i = 0; i < sizeof MSJ_FallaCI; i++) {
                    uart2WriteByte(MSJ_FallaCI[i]);
                }
                for (uint8_t i = 0; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                } //Mando mensajes informando el error
                PIN_LED_FALLA_CI = 1; //Prendo LED correspondiente
            }
            if (FallaTrafPrim == 1) {
                for (uint8_t i = 0; i < sizeof MSJ_FallaTrafPrim; i++) {
                    uart2WriteByte(MSJ_FallaTrafPrim[i]);
                }
                for (uint8_t i = 0; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                } //Mando mensajes informando el error
                PIN_LED_FALLA_TRAFO = 1;
            }

            if (FallaTrafSecu == 1) {
                for (uint8_t i = 0; i < sizeof MSJ_FallaTrafSecu; i++) {
                    uart2WriteByte(MSJ_FallaTrafSecu[i]);
                }
                for (uint8_t i = 0; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                } //Mando mensajes informando el error
                PIN_LED_FALLA_TRAFO = 1;
            }
            if (diagnosticar() == 0) {
                estadoActual = CORRECTO;
                PIN_BUZZER = 0;
            }
            break;


        default:
            //Si algo modificó la variable estadoActual 
            // a un estado no válido llevo la MEF a un 
            // lugar seguro, por ejemplo, la reinicio:
            InicializarMEF();
    }
}
//Función para apagar leds

void apagarLedsFallas(void) {
    PIN_LED_FALLA_CI = 0; //
    PIN_LED_INICIAL = 0; //
    PIN_LED_FALLA_TRAFO = 0; //Apago LEDs indicadores de fallas
    PIN_LED_FALLA_ZENER = 0; //
    PIN_LED_FALLA_GENERAL = 0; //
}
//Función de diagnostico de fallas

uint8_t diagnosticar(void) {
    uint8_t Fallas = 0;
    uint16_t medicion = adcRead_mV(IAN15);
    if (medicion < Vmin || medicion > Vmax) {
        FallaRegulador = 1;
        Fallas = 1;
    }
    if (PIN_FUSIBLE == 1) {
        FallaFuse = 1;
        Fallas = 1;
    }

    if (PIN_QA_CI == 1) {
        if ((tickRead() - tQA_CI > 40 && PIN_QA_CI != 0) || (PIN_QA_CI == 1 && PIN_QB_CI == 1)) {
            FallaCI = 1;
            Fallas = 1;
            tQA_CI = tickRead();
        } else if ((tickRead() - tQA_CI < 40)) {
            tQA_CI_0 = tickRead();
        }
    } else if (PIN_QA_CI == 0) {
        if ((tickRead() - tQA_CI_0 > 40 && PIN_QA_CI != 1) || (PIN_QA_CI == 0 && PIN_QB_CI == 0)) {
            FallaCI = 1;
            Fallas = 1;
            tQA_CI_0 = tickRead();
        } else if (tickRead() - tQA_CI_0 < 40) {
            tQA_CI = tickRead();
        }
    }
    if (PIN_QB_CI == 1) {
        if ((tickRead() - tQB_CI > 40 && PIN_QB_CI != 0) || (PIN_QA_CI == 1 && PIN_QB_CI == 1)) {
            FallaCI = 1;
            Fallas = 1;
            tQB_CI = tickRead();
        } else if (tickRead() - tQB_CI < 40) {
            tQB_CI_0 = tickRead();
        }
    } else if (PIN_QB_CI == 0) {
        if ((tickRead() - tQB_CI_0 > 40 && PIN_QB_CI != 1) || (PIN_QA_CI == 0 && PIN_QB_CI == 0)) {
            FallaCI = 1;
            Fallas = 1;
            tQB_CI_0 = tickRead();
        } else if (tickRead() - tQB_CI_0 < 40) {
            tQB_CI = tickRead();
        }
    }

    if (PIN_QA == 1) {
        if ((tickRead() - tQA > 40 && PIN_QA != 0) || (PIN_QA == 1 && PIN_QB == 1)) {
            FallaTrafPrim = 1;
            Fallas = 1;
            tQA = tickRead();
        } else if (tickRead() - tQA < 40) {
            tQA_0 = tickRead();
        }
    } else if (PIN_QA == 0) {
        if ((tickRead() - tQA_0 > 40 && PIN_QA != 1) || (PIN_QA == 0 && PIN_QB == 0)) {
            FallaTrafPrim = 1;
            Fallas = 1;
            tQA_0 = tickRead();
        } else if (tickRead() - tQA_0 < 40) {
            tQA = tickRead();
        }
    }
    if (PIN_QB == 1) {
        if ((tickRead() - tQB > 40 && PIN_QB != 0) || (PIN_QA == 0 && PIN_QB == 0)) {
            FallaTrafPrim = 1;
            Fallas = 1;
            tQB = tickRead();
        } else if (tickRead() - tQB < 40) {
            tQB_0 = tickRead();
        }
    } else if (PIN_QB == 0) {
        if ((tickRead() - tQB_0 > 40 && PIN_QB != 1) || (PIN_QA == 0 && PIN_QB == 0)) {
            FallaTrafPrim = 1;
            Fallas = 1;
            tQB_0 = tickRead();
        } else if (tickRead() - tQB_0 < 40) {
            tQB = tickRead();
        }
    }

    if (PIN_OPTO == 1) {
        FallaTrafSecu = 1;
        Fallas = 1;
    }
    if (PIN_TEC1 == 0) {
        PIN_BUZZER = 0;
    }
    return (Fallas);
}