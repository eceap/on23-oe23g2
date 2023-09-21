#include <xc.h>         /* Archivo de Encabezados General XC8 */
#include <stdint.h>     /* Para las definiciones de uint8_t por ej.*/
#include "system.h"     /* Funciones/Parametros System, como osc/peripheral config */
#include "user.h"       /* Funciones/Parametros User, como InitApp */
#include "uart.h"       /* Funciones/Parametros Uart */
#include "adc.h"        /* Funciones/parametros del ADC*/
#include "tick.h"       /* Funciones/Parametros Tick */
#include <stdio.h>      /* Archivo de Encabezados StdIO */

/*==================[definiciones y macros]==================================*/
// Nuevo tipo de datos enumerado llamado estadoMEF_t
typedef enum {INICIAL, CORRECTO, FALLAS, DIAGNOSTICO} estadoMEF_t;

/*==================[definiciones de datos internos]=========================*/
estadoMEF_t  estadoActual; // Variable de estado (global)
tick_t tInicio; 
/*Mensajes de puerto serie*/
unsigned char MSJ_FallaFuse[] = {"FalloElFusible-"};  
unsigned char MSJ_FallaCI[] = {"FalloElIntegrado-"};         //Arrays de caracteres 
unsigned char MSJ_FallaRegulador[] = {"FalloElRegulador-"};  //Para puerto serie 
unsigned char MSJ_FallaGeneral[] = {"FallaGeneral-"};
unsigned char MSJ_Normal[] = {"TodoCorrecto-"};
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void InicializarMEF(void);
void ActualizarMEF(void);

/*==================[funcion principal]======================================*/
uint8_t FallaFusible;
__bit FallaFuse, FallaCI, FallaRegulador, FallaGeneral; //Flags de fallas


void main(void) {
    /* TODO: Agregar el Cod1iogo de la Aplicación aquí */
    appInit(); /* Inicializa I/O y Periféricos de la aplicación */
    InicializarMEF();
    
    while (1) {
        ActualizarMEF();
        CLRWDT(); //Reinicia el WDT
    }
}
/*==================[definiciones de funciones internas]=====================*/
// Función Inicializa MEF
void InicializarMEF(void) {
    estadoActual = INICIAL; // Establece estado inicial 
    tInicio = tickRead();   // También inicia temporizacion    
}

// Función Actualizar MEF

void ActualizarMEF(void) {
    switch (estadoActual) {
        case INICIAL:
            PIN_LED_INICIAL = 1; //Prendo LED indicador de etapa de inicializacion
            if (PIN_OPTO == 0) {
                estadoActual = DIAGNOSTICO;
                PIN_LED_INICIAL = 0; // apago el led para indicar que se termino el estado inicial
            }
            if (tickRead() - tInicio > 5000 && PIN_OPTO == 1) {
                estadoActual = FALLAS;
            }
            break;
        case CORRECTO:
            PIN_LED_INICIAL = 0; //Apago LED indicador de etapa de inicializacion
            PIN_LED_NORMAL = 1; //Prendo LED indicador de estado de respuesta normal


            break;
        case FALLAS:
            PIN_LED_INICIAL = 0; //Apago LED indicador de etapa de inicializacion
            PIN_LED_FALLA_GENERAL = 1; //Prendo LED indicador de falla general
            
            if (FallaFusible ==1){
                
            }
            
            if (FallaFuse == 1) {
                for (uint8_t i; i < sizeof MSJ_FallaFuse; i++) {
                    uart2WriteByte(MSJ_FallaFuse[i]);
                }
            }
            break;
        case DIAGNOSTICO:
           


                break;


        default:
            //Si algo modificó la variable estadoActual 
            // a un estado no válido llevo la MEF a un 
            // lugar seguro, por ejemplo, la reinicio:
            InicializarMEF();
    }
}