#include <xc.h>             /* Archivo de Encabezados General XC8 */
#include "system.h"
#include "tick.h"           /* Funciones/Parametros Tick */

/*==================[declaraciones de funciones internas]====================*/
void isr(void);

/*==================[definiciones de funciones internas]=====================*/

void __interrupt() isr(void) {
    /* El sig. ejemplo de código muestra el manejo de interrupciones.
    Note que no son bloques if separados sino que son if else ecadenados.
    No use bloques if separados para cada flag de interrupción para evitar
    errores en tiempo de ejecución. */
    /* TODO: Completar con el código de las rutinas de interrupción aquí. */
    /* Determina que flag ha generado la interrupción */
    //if(TMR0IF) {
    TMR0IF = 0;       /* Baja Interrupt Flag */
    TMR0 += 7;        /* Ajusta desborde a 1 mSeg aprox */
    tickCounter++;    /* Incrementa el contador */
    //else if(TMR2IF) {
    //  TMR2IF=0; /* Baja Interrupt Flag 2 */
    //  ...
    //}
    //else {
    //  /* Interrupción de tipo inesperado */
    //}
}
