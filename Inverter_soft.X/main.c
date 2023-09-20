#include <xc.h>         /* Archivo de Encabezados General XC8 */
#include <stdint.h>     /* Para las definiciones de uint8_t por ej.*/
#include "system.h"     /* Funciones/Parametros System, como osc/peripheral config */
#include "user.h"       /* Funciones/Parametros User, como InitApp */
//#include "uart.h"       /* Funciones/Parametros Uart */
#include "tick.h"       /* Funciones/Parametros Tick */
#include <stdio.h>      /* Archivo de Encabezados StdIO */

/*==================[definiciones y macros]==================================*/
// Nuevo tipo de datos enumerado llamado estadoMEF_t
typedef enum {INICIAL, CORRECTO, FALLAS, DIAGNOSTICO} estadoMEF_t;

/*==================[definiciones de datos internos]=========================*/
estadoMEF_t  estadoActual; // Variable de estado (global)
tick_t tInicio; 
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void InicializarMEF(void);
void ActualizarMEF(void);

/*==================[funcion principal]======================================*/
void main(void) {
    /* TODO: Agregar el Cod1iogo de la Aplicación aquí */
    appInit(); /* Inicializa I/O y Periféricos de la aplicación */
    InicializarMEF();
    while (1) {
        ActualizarMEF();
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
            if(!PIN_OPTO == 1){
                estadoActual = CORRECTO;
            }
            if(tickRead() - tInicio > 5000 && !PIN_OPTO == 0){
                estadoActual = FALLAS;
            }
            break;
        case CORRECTO:
            PIN_LED_INICIAL = 0;       //Apago LED indicador de etapa de inicializacion
            PIN_LED_NORMAL = 1;        //Prendo LED indicador de estado de respuesta normal
            
            break;
        case FALLAS:
            PIN_LED_INICIAL = 0;       //Apago LED indicador de etapa de inicializacion
            PIN_LED_FALLA_GENERAL = 1; //Prendo LED indicador de falla general
            
            
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