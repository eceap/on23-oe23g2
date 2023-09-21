#include "system.h"
#include <stdio.h>
#include <xc.h>


/*==================[tipos de datos declarados por el usuario]===============*/
typedef enum{IAN0, IAN1, IAN2, IAN3, IAN4, IAN5, IAN6, IAN7, IAN8, IAN9, IAN10, IAN11, IAN12, IAN13, IAN14, IAN15, IAN16, IAN17, IAN18, IAN19, IAN20, IAN21, IAN22, IAN23, IAN24, IAN25, IAN26, IAN27, Reserved, CTMU, DAC, FVR_BUF2} adcMap_t;

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/
void adcInit( void );                      /* Inicializa el ADC */
uint16_t adcRead(adcMap_t analogInput);    /* Realiza una conversión AD */
uint16_t adcRead_mV(adcMap_t analogInput); /* Realiza una conversión AD en mV */
/*==================[fin del archivo]========================================*/
