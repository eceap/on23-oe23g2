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

typedef enum {
    INICIAL, CORRECTO, FALLAS, DIAGNOSTICO
} estadoMEF_t;

/*==================[definiciones de datos internos]=========================*/
estadoMEF_t estadoActual; // Variable de estado (global)
tick_t tInicio;
//Mensajes de puerto serie
unsigned char MSJ_FallaFuse[] = {"FalloElFusible-"};
unsigned char MSJ_FallaCI[] = {"FalloElIntegrado-"}; //Arrays de caracteres 
unsigned char MSJ_FallaRegulador[] = {"FalloElRegulador-"}; //Para puerto serie 
unsigned char MSJ_FallaTrafPrim[] = {"FalloElTransformador1-"};
unsigned char MSJ_FallaTrafSecu[] = {"FalloElTransformador2-"};
unsigned char MSJ_FallaGeneral[] = {"FallaGeneral-"};
unsigned char MSJ_Normal[] = {"TodoCorrecto-"};
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void InicializarMEF(void);
void ActualizarMEF(void);

/*==================[funcion principal]======================================*/
uint8_t Fallas;
__bit FallaFuse, FallaCI, FallaRegulador, FallaTrafPrim, FallaTrafSecu, FallaGeneral; //Flags de fallas

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
    tInicio = tickRead(); // También inicia temporizacion    
}

// Función Actualizar MEF

void ActualizarMEF(void) {
    switch (estadoActual) {
        case INICIAL:
            PIN_LED_INICIAL = 1; //Prendo LED indicador de etapa de inicializacion
            if (!PIN_OPTO == 1) {
                estadoActual = DIAGNOSTICO;
                PIN_LED_INICIAL = 0; // apago el led para indicar que se termino el estado inicial
            }
            if (tickRead() - tInicio > 5000 && !PIN_OPTO == 0) {
                estadoActual = FALLAS;
            }
            break;
        case CORRECTO:
            PIN_LED_INICIAL = 0; //Apago LED indicador de etapa de inicializacion
            PIN_LED_FALLA_CI = 0;        
            PIN_LED_NORMAL = 0;           
            PIN_LED_INICIAL = 0;        
            PIN_LED_FALLA_TRAFO = 0;   
            PIN_LED_FALLA_ZENER = 0;     
            PIN_LED_FALLA_GENERAL = 0;
            PIN_LED_NORMAL = 1; //Prendo LED indicador de estado de respuesta normal
            for (uint8_t i; i < sizeof MSJ_Normal; i++) {
                 uart2WriteByte(MSJ_Normal[i]);
            }
            estadoActual = DIAGNOSTICO;
            break;
            
            
            
        case FALLAS:
            PIN_LED_NORMAL = 0;
            PIN_LED_FALLA_GENERAL = 1;
            
            if (FallaFuse == 1){
                for (uint8_t i; i < sizeof MSJ_FallaFuse; i++) {
                    uart2WriteByte(MSJ_FallaFuse[i]);
                }   
                for (uint8_t i; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                }
            }   
            if (FallaCI == 1){
                for (uint8_t i; i < sizeof MSJ_FallaCI; i++) {
                    uart2WriteByte(MSJ_FallaCI[i]);
                }
                for (uint8_t i; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                }
            PIN_LED_FALLA_CI = 1;
            }
            if(FallaTrafPrim == 1){
                for (uint8_t i; i < sizeof MSJ_FallaTrafPrim; i++) {
                    uart2WriteByte(MSJ_FallaTrafPrim[i]);
                }
                for (uint8_t i; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                }
            PIN_LED_FALLA_TRAFO = 1;
            }
            if(FallaTrafSecu == 1){
                for (uint8_t i; i < sizeof MSJ_FallaTrafSecu; i++) {
                    uart2WriteByte(MSJ_FallaTrafSecu[i]);
                }
                for (uint8_t i; i < sizeof MSJ_FallaGeneral; i++) {
                    uart2WriteByte(MSJ_FallaGeneral[i]);
                }
             PIN_LED_FALLA_TRAFO = 1;
            }
            estadoActual = DIAGNOSTICO;
            break;
            
            
            
        case DIAGNOSTICO:
            PIN_LED_INICIAL = 0; //Apago LED indicador de etapa de inicializacion
            if (PIN_FUSIBLE == 1) {            
                FallaFuse = 1;
                Fallas++;
            }


            if (PIN_QA_CI == 1) {
                if ((tickRead() - tInicio > 11 && !PIN_QA_CI == 0)||(PIN_QA_CI == 1 && PIN_QB_CI ==1) ) {             
                    FallaCI = 1;
                    Fallas++;
                }
            }
            if (PIN_QB_CI == 1) {
                if (tickRead() - tInicio > 11 && !PIN_QB_CI == 0) {
                    FallaCI = 1;
                    Fallas++;
                }
            }


            if (PIN_QA == 1) {
                if (tickRead() - tInicio > 11 && !PIN_QA == 0) {                
                    FallaTrafPrim = 1;
                    Fallas++;
                }
            }
            if (PIN_QB == 1) {
                if (tickRead() - tInicio > 11 && !PIN_QB == 0) {
                    FallaTrafPrim = 1;
                    Fallas++;
                }
            }
            

            if (!PIN_OPTO == 0) {
                FallaTrafSecu;
                Fallas++;
            }
                        
            if(Fallas = 0) {estadoActual = CORRECTO;}   
            if(Fallas != 0){estadoActual = FALLAS;}

            break;


        default:
            //Si algo modificó la variable estadoActual 
            // a un estado no válido llevo la MEF a un 
            // lugar seguro, por ejemplo, la reinicio:
            InicializarMEF();
    }
}