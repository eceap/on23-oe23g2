/*==================[inlcusiones]============================================*/
#include <xc.h>         /* Archivo de Encabezados General XC8 */
#include <stdint.h>     /* para las definiciones de uint8_t por ej.*/

#include "system.h"     /* Funciones/Parametros System, como conf de osc/perif */
#include "user.h"       /* Funciones/Parametros User, como InitApp */
//#include "uart.h"       /* Funciones/Parametros Uart */
#include "tick.h"       /* Funciones/Parametros Tick */

void appInit(void) {
    /* TODO: Inicializar Ports/Periféricos/Interrupciones */

    /* Configura funciones analógicas y dirección de los Ports de I/O */
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSC3 = 1;          //Entrada 15 Analogica
    TRIS_TEC1 = 1;
 
    
    TRIS_ZENNER = 1;
    TRIS_QA = 1;
    TRIS_QB = 1;
    TRIS_FUSIBLE = 1;
            
    TRIS_FALLA_CI = 0;
    TRIS_NORMAL = 0;
    TRIS_INICIAL = 0;
    TRIS_FALLA_TRAFO = 0;
    TRIS_FALLA_ZENER = 0;
    TRIS_FALLA_GENERAL = 0;
    
    tickInit();

    //Habilita Interrupciones si es necesario
    TMR0IE = 1;     //Habilita interrupcion para tick
    //PEIE = 1;
    GIE = 1;
    
    SWDTEN = 1;     // Habilito el WDT
    
    
}
