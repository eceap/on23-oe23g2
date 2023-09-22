#include <xc.h>         /* Archivo de Encabezados General XC8 */
#include <stdint.h>     /* Para las definiciones de uint8_t por ej.*/
#include "system.h"     /* Funciones/Parametros System, como osc/peripheral config */
#include "user.h"       /* Funciones/Parametros User, como InitApp */
#include "uart.h"       /* Funciones/Parametros Uart */
#include "adc.h"        /* Funciones/parametros del ADC*/
#include "tick.h"       /* Funciones/Parametros Tick */
#include <stdio.h>      /* Archivo de Encabezados StdIO */

/*=======================[definiciones & Macros]=============================*/

#define BUZZER(a) (a==1?PORTAbits.RA1=0:PORTAbits.RA1=1)

typedef enum {INICIAL, CORRECTO, FALLAS, DIAGNOSTICO} estadoMEF_t;

/*==================[definiciones de datos internos]=========================*/
estadoMEF_t estadoActual; // Variable de estado (global)
tick_t tInicio;
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
uint16_t diagnosticar(void);
void apagarLedsFallas(void);

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
}

//Función Actualizar MEF

void ActualizarMEF(void) {
    switch (estadoActual) {
        case INICIAL:
            PIN_LED_INICIAL = 1; //Prendo LED indicador de etapa de inicializacion
            if (!PIN_OPTO) {
                estadoActual = CORRECTO; //Se detecto tension a la salida del trafo, levantó correctamente el circuito
                PIN_LED_INICIAL = 0;
            }
            if ( ((tickRead() - tInicio )> 5000) && PIN_OPTO ) {
                estadoActual = FALLAS; // No se detectó tension en la salida del trafo pasados 5 segundos
                PIN_LED_INICIAL = 0;
            }
            break;
            
        case CORRECTO:
            apagarLedsFallas();         //Apago LEDs indicadores de fallas
            PIN_LED_NORMAL = 1;         //Prendo LED indicador de estado de respuesta normal
            for (uint8_t i; i < sizeof MSJ_Normal; i++) {
                 uart2WriteByte(MSJ_Normal[i]);
            }  
            if(diagnosticar()== 1){
                estadoActual = FALLAS
                BUZZER(1);
            }
            break;
            
        case FALLAS:
            PIN_LED_NORMAL = 0;                 //Apago el LED indicador del estado CORRECTO
            PIN_LED_FALLA_GENERAL = 1;          //Prendo LED indicador del estado Falla General
            if (FallaRegulador == 1){
                for (uint8_t i; i < sizeof MSJ_FallaRegulador; i++) {
                    uart2WriteByte(MSJ_FallaRegulador[i]);
                }   
                for (uint8_t i; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                }
            PIN_LED_FALLA_ZENER = 1;
            }         
            if (FallaFuse == 1){    //El LED de esta falla se prende por Hardware
                for (uint8_t i; i < sizeof MSJ_FallaFuse; i++) {
                    uart2WriteByte(MSJ_FallaFuse[i]);
                }                                                       
                for (uint8_t i; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                }                   //Mando mensajes informando el error
            }               
            if (FallaCI == 1){
                for (uint8_t i; i < sizeof MSJ_FallaCI; i++) {
                    uart2WriteByte(MSJ_FallaCI[i]);
                }                   
                for (uint8_t i; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                }                   //Mando mensajes informando el error
            PIN_LED_FALLA_CI = 1;   //Prendo LED correspondiente
            }        
            if(FallaTrafPrim == 1){
                for (uint8_t i; i < sizeof MSJ_FallaTrafPrim; i++) {
                    uart2WriteByte(MSJ_FallaTrafPrim[i]);
                }
                for (uint8_t i; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                }                   //Mando mensajes informando el error
            PIN_LED_FALLA_TRAFO = 1;
            }
            
            if(FallaTrafSecu == 1){
                for (uint8_t i; i < sizeof MSJ_FallaTrafSecu; i++) {
                    uart2WriteByte(MSJ_FallaTrafSecu[i]);
                }
                for (uint8_t i; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                }                   //Mando mensajes informando el error
            PIN_LED_FALLA_TRAFO = 1;
            }
            if (diagnosticar() == 0){
                estadoActual = CORRECTO;
                BUZZER(0);
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
void apagarLedsFallas (void){
            PIN_LED_FALLA_CI = 0;       //
            PIN_LED_INICIAL = 0;        //
            PIN_LED_FALLA_TRAFO = 0;    //Apago LEDs indicadores de fallas
            PIN_LED_FALLA_ZENER = 0;    //
            PIN_LED_FALLA_GENERAL = 0;  //
}
//Función de diagnostico de fallas
uint16_t diagnosticar(void) {
    if (adcRead(PIN_ZENNER) - 2 < 733) {
        FallaRegulador = 1;
        return (1);
    }
    if (PIN_FUSIBLE == 1) {
        FallaFuse = 1;
        return (1);
    }

    if (PIN_QA_CI == 1) {
        if ((tickRead() - tInicio > 11 && !PIN_QA_CI == 0) || (PIN_QA_CI == 1 && PIN_QB_CI == 1)) {
            FallaCI = 1;
            return (1);
        }
    }
    if (PIN_QB_CI == 1) {
        if (tickRead() - tInicio > 11 && !PIN_QB_CI == 0) {
            FallaCI = 1;
            return (1);
        }
    }

    if (PIN_QA == 1) {
        if (tickRead() - tInicio > 11 && !PIN_QA == 0) {
                FallaTrafPrim = 1;
                return (1);
            }
        }
    if (PIN_QB == 1) {
        if (tickRead() - tInicio > 11 && !PIN_QB == 0) {
            FallaTrafPrim = 1;
            return (1);
        }
    }
    
    if (!PIN_OPTO == 0) {
        FallaTrafSecu = 1;
        return (1);
    }
    if  (!PIN_TEC1 == 1){BUZZER(0);}
    return (0);
}
